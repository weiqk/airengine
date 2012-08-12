#include "AirEngineLight.h"
#include "AirEngineScene.h"
#include "AirRenderSystem.h"
#include "AirRenderTarget.h"
#include "AirGlobalSetting.h"
#include "AirEngineCamera.h"
namespace Air{
	
	namespace	Client{
	
	
		Light::Light( CAString& strName ,Info*	pInfo):Camera(strName){
			m_pShadowCamera	=	NULL;
			m_Info			=	*pInfo;
			SetPosition(Float3(m_Info.vPosition.x,m_Info.vPosition.y,m_Info.vPosition.z));
	
			//m_bCastShadow	=	m_Info.bCastShadow;
			Camera::SetType(enCT_SHADOW);
			m_fAttenuation	=	1000;
		}
	
		U1 Light::Create(){
			return true;
		}
	
		U1 Light::Destroy(){
			return true;
		}
	
		void Light::SetLightType( enumType type ){
			if(m_Info.type==type){
				return;
			}
			m_Info.type	=	type;
			
		}
	
		Light::enumType Light::GetLightType(){
			return m_Info.type;
		}
		Light::Info Light::GetInfo(){
			return	m_Info;
		}
	
		void Light::SetColor( const Float4& color ){
			m_Info.vDiff	=	color;
		}
	
		Float4 Light::GetLightPosition(){
			Float3	v	=	GetRealPosition();
			Float4	vv	=	Float4(v.x,v.y,v.z,-1);
			if(	m_Info.type ==	enPoint	||
				m_Info.type	==	enSpot)
				vv.w	=	m_fAttenuation;
			return vv;	
		}
	
	
		void Light::SetAttenuation( Real fAttenuation ){
			m_fAttenuation	=	fAttenuation;
		}
	
		void	Light::SetSpecular( Real fSpec ){
			m_Info.vDiff.w	=	fSpec;
		}
	}
};