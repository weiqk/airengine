#include "AirEngineSystem.h"

#include "AirGlobalSetting.h"
#include "AirInterfaceUISystem.h"
#include "AirInterfaceResourceSystem.h"
#include "AirInterfaceNetSystem.h"
#include "AirSlkReader.h"

//����ͷ�ļ�
#include "AirCommonLog.h"
#include "AirCommonPlugin.h"
#include "AirEngineCharacterResource.h"
#include "AirEngineCharacterAnimationModel.h"
#include "AirEngineCharacterTextureAnimationModel.h"
#include "AirEngineTerrainCommon.h"
#include "AirEngineSubEntity.h"
#include "AirEngineMaterial.h"
#include "AirEngineLight.h"
#include "AirEngineCamera.h"
#include "AirEngineMesh.h"
#include "AirEngineScreenEntity.h"
#include "AirEnginePostComposer.h"
#include "AirEngineParticleEntity.h"
#include "AirInterfaceShootParticle.h"
#include "AirEngineFont.h"
#include "AirEngineTextInstance.h"
#include "AirStaticMesh.h"
#include "AirMeshEntity.h"
#include "AirManualMeshEntity.h"

#include "AirCommonIniFile.h"
#include "AirEngineCharacterManager.h"
#include "AirInterfaceInputSystem.h"

#include "AirEnginePipeline.h"
#define USE_PROFILE
#include "AirCommonProfile.h"
#include "AirCommonWindow.h"
#include "AirRenderWindow.h"
#include "AirEngineMaterialParse.h"

#include "AirEngineMaterialTemplate.h"
#include "AirEngineMaterial.h"
#include "AirEngineLight.h"

#include <fstream>

namespace Air{
	
	
	namespace	Client{
	
		Character::Manager	g_CharacterMgr;
		const	static	UInt	g_uiNumPlugin		=	16;
		const	static	AString	g_strPluginArray[]	=	{
			"Resource",
			"Render",
			"Physx",
			"Audio",
			"Script",
			"Particle",
			"Input",
			"UI",
			"FlashUI",
			"Net",
			"Game",
			"Custom",
			"Custom1",
			"Custom2",
			"Custom3",
			"Custom4",
			"Custom5"
		};
	
	
		EngineSystem::EngineSystem(){
			
			m_strPluginNameArray.resize(g_uiNumPlugin);
			//m_strPluginFileName				=	"..\\Data\\Config\\Plugin.ini";
			
			m_pPipeline						=	NULL;
		}
	
		EngineSystem::~EngineSystem(){
			//Release();
			
		}
	
		void EngineSystem::ExecuteOneFrame(const FrameTime& frameTime){
			//PROFILE_FUNCTION
			//Render::System::GetSingleton()->OnFrameMove(fTime,fElapsedTime,pUserContext);
			//����Ƿ���Ҫ�ͷŻ������е�����
			ReleaseOneObject();
	
			//��������ϵͳ
			//Input::Capture();
			if(GetGlobalSetting().m_pInputSystem!=NULL)
				GetGlobalSetting().m_pInputSystem->Capture();
	
	
			//��Ⱦ����
			UI::ISystem*	pUISystem		=	GetGlobalSetting().m_pUISystem;
			if(pUISystem!=NULL){
				pUISystem->Updata();
			}

			if(m_pPipeline!=NULL){
				m_pPipeline->Update(frameTime);
				m_pPipeline->RenderOneFrame(frameTime);
			}
		}
	
		U1 EngineSystem::Initialization(){
			//ENGINE_LOG_INFO;
			//������ʾ����
			ParseDisplayParam();
			//��������
			CreateWin();

 			Data	data;
 			Common::File::Load("..\\Data\\Material\\TestState.State",data.buff,data.size);
 			MaterialParse::GetSingleton()->CompileState(data.buff,data.size);
 			data.Clear();
 			Common::File::Load("..\\Data\\Material\\TestMaterial.MaterialTemplate",data.buff,data.size);
 			MaterialParse::GetSingleton()->CompileMaterialTemplate(data.buff,data.size);
 			data.Clear();
 			Common::File::Load("..\\Data\\Material\\TestMaterial.Material2",data.buff,data.size);
 			MaterialParse::GetSingleton()->CompileMaterial(data.buff,data.size);
 			data.Clear();
			Common::File::Load("..\\Data\\AirMesh\\Material #1.material",data.buff,data.size);
			MaterialParse::GetSingleton()->CompileMaterialSet(data.buff,data.size);
			data.Clear();
			
			//AddFactory(new	Common::PluginFactory());
			AddFactory(new	NoParamFactory<Pipeline>("Pipeline"));
			AddFactory(new	FontFactory());
			AddFactory(new	ExtraOptionParamFactory<MaterialTemplate,MaterialTemplateInfo*>("MaterialTemplate"));
			AddFactory(new	OptionParamFactory<Material>("Material"));
			AddFactory(new	SubEntityFactory());
			//AddFactory(new	CameraFactory());
			//AddFactory(new	LightFactory());
			AddFactory(new	ExtraOptionParamFactory<Character::Resource,AString*>("Character"));
			AddFactory(new	ParamFactory<Character::Animation::Model>("Model"));
			//AddFactory(new	Character::Animation::TextureModelFactory());
			AddFactory(new	Terrain::IPageFactory());
			AddFactory(new	MeshFactory());
			AddFactory(new	ScreenEntityFactory());
			AddFactory(new	PostComposerFactory());
			AddFactory(new	ParticleEntityFactory());
			AddFactory(new	IParticleFactory());
			AddFactory(new	ShootParticleFactory());
			AddFactory(new	TextInstanceFactory());

			AddFactory(new	NoParamFactory<StaticMesh>("StaticMesh"));
			AddFactory(new	ParamFactory<MeshEntity>("MeshEntity"));
			AddFactory(new	ParamFactory<ManualMeshEntity>("ManualMeshEntity"));
			AddFactory(new	ParamFactory<Light>("Light"));
	
	
			//�������в��
 			if(!LoadPlugin())
 				return false;

			Render::System::GetSingleton()->Initialization();
	
			//��ʼ������ϵͳ������
			InitAllSystemManager();

			//����Pipeline
			m_pPipeline	=	CreateProduct<Pipeline*>("DefaultPipeline","Pipeline");
	
			//����ϵͳ
			StartAllSystemManager();

			

			return true;
		}
	
		U1 EngineSystem::Start(){
			//ENGINE_LOG_INFO;

// 					static	ShaderShareParam&	sParam	=	GetGlobalSetting().m_ShaderParam;
// 					static	Real	fLastTime		=	timeGetTime()*0.001f;
// 					//ѭ���߼�
// 					Real	fCurrentTime	=	timeGetTime()*0.001f;
// 					sParam.m_fEngineTimeDelta		=	fCurrentTime	-	fLastTime;
// 					fLastTime						=	fCurrentTime;
// 					sParam.m_fEngineTotalTime		+=	sParam.m_fEngineTimeDelta	;
// 	
// 	  				static	UInt	uiFrameCount	=	0;
// 	  				uiFrameCount++;
// 	  				static	Real	fTime			=	0;
// 	  				fTime+=	sParam.m_fEngineTimeDelta;
// 	  
// 	  				if(fTime	>	2.0f){
// 	  					Real	fps	=	uiFrameCount/fTime;
// 	  					AChar	str[128];
// 	  					sprintf_s(str,128,"[Engine]FPS=%f\n",fps);
// 	  					OutputDebugStringA(str);
// 	  					uiFrameCount=0;
// 	  					fps	=	0;fTime=0;
// 	  				}
// 					Sleep(16);
			MSG msg;
			::ZeroMemory(&msg, sizeof(MSG));
			while(true){
				if(::PeekMessage(&msg, 0, 0, 0, PM_REMOVE)){
					::TranslateMessage(&msg);
					::DispatchMessage(&msg);
				}else{

					Sleep(10);
					GetTimer().AddFrame();
					ExecuteOneFrame(GetTimer().m_FrameTime);
				}

				if(msg.message	==	WM_QUIT){
					break;
				}
			}


			return true;
	
		}
	
		U1 EngineSystem::Release(){
			//ENGINE_LOG_INFO;

			MaterialParse::ReleaseSingleton();

			//ֹͣϵͳ
			StopAllSystemManager();

			//�ͷ����л������е�����
			ReleaseAllAutoObject();
	
			if(m_pPipeline!=NULL){
				m_pPipeline->ReleaseRef();
				m_pPipeline=NULL;
			}
			
			
			
	
			//�ͷ�����ϵͳ������
			ReleaseAllSystemManager();
	


			DestroyAllFactory();

			Render::System::GetSingleton()->Release();
	
			//MaterialParse::GetSingleton()->ReleaseSingleton();
			Render::System::ReleaseSingleton();

			//����ͷ����в��
			UnLoadPlugin();
			return true;
	
		}
	
	
	
		U1 EngineSystem::LoadPlugin(){
			ENGINE_LOG_INFO
			EngineParam*	pDst	=	&GetGlobalSetting().m_EngineParam;
			AString strPluginName	=	"..\\Data\\Config\\Plugin.ini";//GetGlobalSetting().m_EngineParam.strPlugin;
			File	file(strPluginName);
	
			if(!file.Open())
				return	false;
	
			IniFile	inifile;
			if(!inifile.LoadFileFromMemory(file.GetData()))
				return false;
	
			StringVector*	pLstString	=	inifile.GetStringList("Plugin","Plugins");
			StringVector&	lstString	=	*pLstString;
			UInt	uiSize	=	lstString.size();



			for(UInt i=0;i<uiSize;i++){
				Common::Plugin*	pPlugin	=	new Common::Plugin(lstString[i],NULL);
				pPlugin->Create();
				m_vecPlugin.push_back(pPlugin);
			}

			
	
// 			for(SInt	i=0;i<g_uiNumPlugin;i++){
// 				//��ȡ�����
// 				m_strPluginNameArray[i]	=	cfgFile.Get("Plugin",(AChar*)g_strPluginArray[i].c_str());
// 				if(!m_strPluginNameArray[i].empty()){
// 					//���ز��	����ʼ��
// 					Plugin*		p		=	m_pPluginMgr->Create<Plugin*>(m_strPluginNameArray[i],"Plugin");
// 					p->AddRef();
// 					p->SetFactory(m_pPluginMgr);
// 					m_pPluginMgr->Insert(p);
// 					p->Excute(Plugin::enInit);
// 	
// 	// 				Log::GetSingleton()->Write(m_strPluginNameArray[i]);
// 	// 				Log::GetSingleton()->Write("LoadPlugin");
// 				}
// 			}
	
			
			return	true;
		}

		Air::U1 EngineSystem::LoadPlugin( CAString& strPluginName ){
			//CreateProduct(strPluginName,"Plugin");
			return	true;
		}

		U1 EngineSystem::UnLoadPlugin(){

			U32	uiSize	=	m_vecPlugin.size();
			for(int	i	=	uiSize-1;i>=0;i--){
				m_vecPlugin[i]->Destroy();
				delete m_vecPlugin[i];
			}
			m_vecPlugin.clear();
//			ENGINE_LOG_INFO
			//��ȡ�������
//  			IFactory*	pPluginFactory	=	GetFactory("Plugin");
//  			if(pPluginFactory==NULL)
//  				return false;
// 			for(SInt	i=g_uiNumPlugin	-	1;i>=0;i--){
// 				if(m_strPluginNameArray[i].empty())
// 					continue;
// 				//ж�ز��
// 				Plugin*	p	=	dynamic_cast<Plugin*>(pPluginFactory->GetProduct(m_strPluginNameArray[i]));
// 				p->Excute(Plugin::enRelease);
// 				p->ReleaseRef();
// 			}
// 			pPluginFactory->DestroyAllProduct();
// 	
// 			delete	m_pPluginMgr;
// 			m_pPluginMgr	=	NULL;
			return	true;
		}
	
		Scene* EngineSystem::CreateScene( AString strName ){
			
			Scene*	pScene	=	new	Scene(strName);
	
			pScene->Initialization();
	
			return	pScene;
		}
	
		U1 EngineSystem::DestroyScene( Scene* pScene ){
			if(pScene!=NULL){
				pScene->Release();
				delete	pScene;
			}
			return true;
		}
	
		U1 EngineSystem::Stop(){
			//::PostQuitMessage(0);
			return	true;
		}
	
		U1 EngineSystem::ParseDisplayParam(){
			EngineParam*	pSdt	=	&GetGlobalSetting().m_EngineParam;
			AString strDisplayName	=	GetGlobalSetting().m_EngineParam.strDisplay;
			File	file(strDisplayName);
	
			if(!file.Open())
				return	false;
	
			IniFile	cfgFile;
			if(!cfgFile.LoadFileFromMemory(file.GetData()))
				return false;	
			DisplayParam&	dParam	=	GetGlobalSetting().m_DisplayParam;
			AString	str;
			str	=	cfgFile.Get("Display","Width");			dParam.iWidth		=	Converter::ToS32(str);
			str	=	cfgFile.Get("Display","Height");		dParam.iHeight		=	Converter::ToS32(str);
			str	=	cfgFile.Get("Display","Window");		dParam.bWindow		=	Converter::ToU1(str);
			str	=	cfgFile.Get("Display","MSAA");			dParam.iMSAAType	=	Converter::ToS32(str);
			str	=	cfgFile.Get("Display","MSAAQuality");	dParam.iMSAAQuality	=	Converter::ToS32(str);
	
			m_strWindowName	=	cfgFile.Get("Display","WindowName");
			return	true;
	
		}

		void EngineSystem::BeforeSwitchSystem( Common::ISystemManager* pSystemMgr, Common::ISystem* pOldSys, Common::ISystem* pNewSystem ){

		}

		void EngineSystem::OnSwitchSystem( Common::ISystemManager* pSystemMgr, Common::ISystem* pOldSys, Common::ISystem* pNewSystem ){

		}

		void EngineSystem::AfterSwitchSystem( Common::ISystemManager* pSystemMgr, Common::ISystem* pOldSys, Common::ISystem* pNewSystem ){

		}

		Air::U1 EngineSystem::CreateWin(){
			U32	uiWidth		=	GetGlobalSetting().m_DisplayParam.iWidth	+	16;
			U32	uiHeight	=	GetGlobalSetting().m_DisplayParam.iHeight	+	38;
			GetGlobalSetting().m_EngineParam.hWnd	=	Common::NewWindow("AirEngine",uiWidth,uiHeight,WNDPROC(WndProc));
			return	true;
		}
	}
};