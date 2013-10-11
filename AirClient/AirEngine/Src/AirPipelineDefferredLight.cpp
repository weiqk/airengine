#include "AirPipelineDefferredLight.h"
#include "AirEngineSystem.h"
#include "AirMeshEntity.h"
#include "AirEngineMaterial.h"
#include "AirEnginePipeline.h"
#include "AirQuadRenderable.h"
#include "AirGlobalSetting.h"
namespace	Air{
	namespace	Engine{


		DefferredLight::DefferredLight()
		{
			m_pPointMaterial	=	NULL;
			m_pLightBuffer		=	NULL;
			m_pSphere			=	NULL;
			m_pPipeline			=	NULL;
		}

		Air::U1 DefferredLight::Initialization(Pipeline* pPipeline)
		{
			m_pPipeline			=	pPipeline;
			RenderTarget::Info	info;
			info.SetSingleTargetScreen(enTFMT_R16G16B16A16_FLOAT,1.0f,true,pPipeline->GetMainWindow());
			m_pLightBuffer		=	RenderSystem::GetSingleton()->CreateProduct<RenderTarget>("DefferredLight",&info);
			m_pLightBuffer->AddPhaseFlag(enPI_DeferredLight);
			m_pLightBuffer->AddCamera(pPipeline->GetMainCamera());
			m_pLightBuffer->SetClearFlag(false,true,false);
			

			m_pPointMaterial	=	EngineSystem::GetSingleton()->CreateProduct<Material>("PointLight");
			MeshEntity::Info meshInfo;
			meshInfo.strMeshName	=	"AirMesh/Sphere.ame";
			meshInfo.strMaterial	=	"NoMaterial";
			m_pSphere			=	EngineSystem::GetSingleton()->CreateProduct<MeshEntity>("PointLight",&meshInfo);


			return true;
		}

		Air::U1 DefferredLight::Release()
		{
			m_pPipeline	=	nullptr;
			SAFE_RELEASE_REF(m_pPointMaterial);
			SAFE_RELEASE_REF(m_pSphere);
			SAFE_RELEASE_REF(m_pLightBuffer);
			return true;
		}

		void DefferredLight::Update(const FrameTime& frameTime)
		{
			
			if(m_pLightBuffer->BeforeUpdate()){
				if(m_vecPointLight.empty()){
					m_pLightBuffer->AfterUpdate();
					return;
				}
					
				Render::Device* pDevice	=	RenderSystem::GetSingleton()->GetDevice();

				struct {
					Float4	vParam[2];
					Float44 matViewProjInv;
				}cb;

				cb.matViewProjInv	=	m_pPipeline->GetMainCamera()->GetViewProjMatrix();
				cb.matViewProjInv.Inverse();
				memcpy(cb.vParam,&m_vecPointLight[0],sizeof(PointLightInfo));
				m_pPointMaterial->GetConstantBuffer()->UpdateData(&cb);

				m_pPointMaterial->RenderOneObject(m_pSphere);

				
				U32 uiCount	=	m_vecPointLight.size();
				for(U32 i=1;i<uiCount;i++){
					PointLightInfo& info	=	m_vecPointLight[i];
					memcpy(cb.vParam,&info,sizeof(info));
					m_pPointMaterial->GetConstantBuffer()->UpdateData(&cb);
					pDevice->DrawOpt(m_pSphere->GetMesh()->GetDrawBuffer().m_DrawOption);
				}

				m_pLightBuffer->AfterUpdate();
			}
			//m_vecPointLight.clear();
			
		}

		void DefferredLight::AddPointLight( const Float3& pos,float fSize,const Float3& vColor )
		{
			PointLightInfo	point;
			point.vPos		=	pos;
			point.fSize		=	fSize;
			point.vColor	=	vColor;
			m_vecPointLight.push_back(point);
		}


		TileBaseLight::TileBaseLight()
		{
			m_pCSRenderable	=	NULL;
			m_pLightPosColor=NULL;
			m_vecPointLight.reserve(1024);
		}

		Air::U1 TileBaseLight::Initialization( Pipeline* pPipeline )
		{
			m_pCSRenderable		=	new	CSRenderable;
			m_pPipeline			=	pPipeline;
			RenderTarget::Info	info;
			info.SetSingleTargetScreen(enTFMT_R16G16B16A16_FLOAT,1.0f,true,pPipeline->GetMainWindow());
			info.vecTextureInfo[0].SetViewFlag(enVF_SRV|enVF_UAV);
			m_pLightBuffer		=	RenderSystem::GetSingleton()->CreateProduct<RenderTarget>("TileBaseLighting",&info);
			m_pLightBuffer->AddPhaseFlag(enPI_DeferredLight);
			m_pLightBuffer->AddCamera(pPipeline->GetMainCamera());
			m_pLightBuffer->SetClearFlag(false,true,false);

			Buffer::Info binfo;
			binfo.SetStructureBuffer(1024,sizeof(PointLightInfo));
			binfo.SetViewFlag(enVF_SRV);
			m_pLightPosColor		=	RenderSystem::GetSingleton()->CreateProduct<Buffer>("TBL_PosColor",&binfo);

			m_pPointMaterial	=	EngineSystem::GetSingleton()->CreateProduct<Material>("TileBaseLighting");
			m_pSphere			=	NULL;//EngineSystem::GetSingleton()->CreateProduct<MeshEntity>("PointLight",&meshInfo);

			
			for(U32 i=0;i<1024;i++){
				Float3 vPos(
					Common::Number::RandomF(),
					Common::Number::RandomF(),
					Common::Number::RandomF()
					);
				Float3 vColor(
					Common::Number::RandomF(),
					Common::Number::RandomF(),
					Common::Number::RandomF()
					);
				AddPointLight(vPos*100,1,vColor);
			}
			return true;
		}

		Air::U1 TileBaseLight::Release()
		{
			SAFE_DELETE(m_pCSRenderable);
			return __super::Release();
		}

		void TileBaseLight::Update( const FrameTime& frameTime )
		{
			if(m_vecPointLight.empty()){
				return;
			}

			//BuildSO();

			Render::Device* pDevice	=	RenderSystem::GetSingleton()->GetDevice();
			void* p	=	m_pLightBuffer->GetUAV();
			pDevice->SetUAV(1,(void**)&p);

			PointLightVector vecLight = m_vecPointLight;
			const Matrix& mView	=	m_pPipeline->GetMainCamera()->GetViewMatrix();
			for(U32 i=0;i<vecLight.size();i++){
				vecLight[i].vPos	=	mView*vecLight[i].vPos;
			}
			
			m_pLightPosColor->UpdateData(&vecLight[0]);
			pDevice->SetSRV(enCS,2,m_pLightPosColor->GetSRV());

			Float4 v[10];
			v[0]=Float4(m_pLightBuffer->GetWidth(),m_pLightBuffer->GetHeight(),0,0);
			Matrix* pProjInvMat	=	(Matrix*)&v[1];
			m_pPipeline->GetMainCamera()->GetProjMatrix(*pProjInvMat);
			pProjInvMat->Inverse();
			Matrix* pViewMat	=	(Matrix*)&v[5];
			m_pPipeline->GetMainCamera()->GetViewMatrix(*pViewMat);


			Float3 v1=(*pProjInvMat)*Float3(1,1,0);
			
			v[0].z=	v1.x;
			v[0].w=v1.y;
			//Float4 v2[4];
			//for(int i=0;i<4;i++){
			//	v2[i]	=	XMVector4Transform(v1[i].ToXM(),pProjInvMat->ToXM());
			//}
			//Common::Plane p0(Float3(0,0,0),v2[0],v2[1]);
			//Common::Plane p1(Float3(0,0,0),v2[2],v2[3]);

			
			v[9]	=	Float4(
				Engine::GetGlobalSetting().m_pInputSystem->m_iX,
				Engine::GetGlobalSetting().m_pInputSystem->m_iY,
				m_vecPointLight.size(),
				0
				);


			m_pPointMaterial->GetConstantBuffer()->UpdateData(v);
			int x	=	(m_pLightBuffer->GetWidth()+15)>>4;
			int y	=	(m_pLightBuffer->GetHeight()+15)>>4;
			m_pCSRenderable->m_Dispatch[0]	=	x;
			m_pCSRenderable->m_Dispatch[1]	=	y;
			m_pPointMaterial->RenderOneObject(m_pCSRenderable);

			pDevice->SetShader(enCS,NULL);
			p=NULL;
			pDevice->SetUAV(1,(void**)&p);
			//m_vecPointLight.clear();
		}
		void TileBaseLight::AddPointLight( const Float3& pos,float fSize,const Float3& vColor )
		{
			
			PointLightInfo	point;
			point.vPos		=	pos;
			point.fSize		=	fSize;
			point.vColor	=	vColor;
			m_vecPointLight.push_back(point);
		}

		U32	XYZPlaneIntersectSphere(const Float3& xyz,const Float3& center,float fRadius){
			Float3 v =	xyz	- center;
			if(	abs(v.x)<fRadius	||	
				abs(v.y)<fRadius	||
				abs(v.z)<fRadius)
			{
				return 0xffffffff;
			}

			U32 X	=	v.x	<=	fRadius;
			U32	Y	=	v.y	<=	fRadius;
			U32	Z	=	v.z	<=	fRadius;

			return X<<2|Y<<1|Z;
		}

		class LightNode{
		public:
			LightNode(U32 depth=0){
				for(U32 i=0;i<8;i++){
					pChild[i]=NULL;
				}
				uiDepth	=	depth;
			};
			LightNode*	InjectLight(const Float3& pos,float fRadius){
				
				U32 uiChildIndex	=	XYZPlaneIntersectSphere(bound.GetCenter(),pos,fRadius);
				if(uiChildIndex==0xffffffff){
					vecLight.push_back(Float4(pos.x,pos.y,pos.z,fRadius));
					return this;
				}
				if(pChild[uiChildIndex]==NULL){
					pChild[uiChildIndex]	=	new LightNode(uiDepth+1);
					Float3 vHalfSize		=	bound.GetHalfSize();
					Float3 vChildMin		=	vHalfSize*Float3(uiChildIndex>>2,(uiChildIndex>>1)&1,uiChildIndex&1);
					BoundingBox& B			=	pChild[uiChildIndex]->bound;
					B.vMin					=	bound.vMin+vChildMin;
					B.vMax					=	B.vMin+vHalfSize;
				}
				return pChild[uiChildIndex]->InjectLight(pos,fRadius);
			}
			void	DebugPrint(){
				char str[]="--------------------------------------";
				sprintf(&str[uiDepth+1],"%d\n",vecLight.size());
				OutputDebugStringA(str);
				for(U32 i=0;i<8;i++){
					if(pChild[i]!=NULL){
						pChild[i]->DebugPrint();
					}
				}

			}
			U32					uiDepth;
			LightNode*			pChild[8];
			BoundingBox			bound;
			STD_VECTOR<Float4>	vecLight;
			
		};

		void TileBaseLight::BuildSO()
		{
			if(m_vecPointLight.empty())
				return;
			const Matrix& mView	=	m_pPipeline->GetMainCamera()->GetViewMatrix();
			//calc boundingbox
			LightNode lightNode;
			BoundingBox& box	=	lightNode.bound;
			for(U32 i=0;i<m_vecPointLight.size();i++){
				Float3& vPos	=	m_vecPointLight[i].vPos;
				//vPos			=	mView*vPos;
				float	fSize	=	m_vecPointLight[i].fSize;
				if(i==0){
					box.vMin	=	vPos-fSize;
					box.vMax	=	vPos+fSize;
				}else{
					box.Add(vPos-fSize);
					box.Add(vPos+fSize);
				}
			}
			//translate cuboid to cube
			Float3 vSize		=	box.vMax-box.vMin;
			float fMaxSize		=	max(vSize.x,max(vSize.y,vSize.z));
			vSize				=	Float3(fMaxSize,fMaxSize,fMaxSize);
			box.vMax			=	box.vMin+vSize;

			struct LightStruct{
				U16	child[8];
			};
			STD_VECTOR<LightStruct>	LightNodeOctree;
			LightNodeOctree.reserve(8192);

			

			U32 uiMaxDepth	=	10;
			for(U32 i=0;i<m_vecPointLight.size();i++){
				Float3	vLightPos	=	m_vecPointLight[i].vPos;
				float	fLightSize	=	m_vecPointLight[i].fSize;
				lightNode.InjectLight(vLightPos,fLightSize);
			}

			lightNode.DebugPrint();
		}

	}
}