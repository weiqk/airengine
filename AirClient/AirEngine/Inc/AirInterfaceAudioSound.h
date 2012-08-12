#ifndef INTERFACEAUDIOSOUND_HEAD_FILE
#define INTERFACEAUDIOSOUND_HEAD_FILE

#include "AirEngineHeader.h"
#include "AirEngineMovableObject.h"
namespace Air{
	
	namespace Client{
	
		namespace	Audio{
			/**	\brief	属性
			*
			*	属性
			*
			***/
			class	IProperty{
			public:
				IProperty(){
					m_pParent	=	NULL;
					m_fVolume	=	1.0f;
					m_fPitch	=	1.0f;
					m_bIsChange	=	false;
				};
				/**	\brief	设置音量
				*   
				*	@remarks 	设置音量
				*	@see		IProperty
				*	@return   	void
				*	@param		Real f
				*	@note
				*
				**/
				virtual	void	SetVolume(Real	f){
					m_bIsChange	=	true;
					m_fVolume	=	f;
				};
				/**	\brief	获取音量
				*   
				*	@remarks 	获取音量
				*	@see		IProperty
				*	@return   	Real
				*	@note
				*
				**/
				virtual	Real	GetVolume(){
					if(m_pParent!=NULL){
						return	m_pParent->GetVolume()*m_fVolume;
					}else{
						return	m_fVolume;
					}
				};
	
				/**	\brief	设置音调
				*   
				*	@remarks 	设置音调
				*	@see		IProperty
				*	@return   	void
				*	@param		Real f
				*	@note
				*
				**/
				virtual	void	SetPitch(Real	f){
					m_bIsChange	=	true;
					m_fPitch	=	f;
				};
				/**	\brief	获取声调
				*   
				*	@remarks 	获取声调
				*	@see		IProperty
				*	@return   	Real
				*	@note
				*
				**/
				virtual	Real	GetPitch(){
					if(m_pParent!=NULL){
						return	m_pParent->GetPitch()*m_fPitch;
					}else{
						return	m_fPitch;
					}
				};
				/**	\brief	判断是否被改变
				*   
				*	@remarks 	判断是否被改变
				*	@see		IProperty
				*	@return   	U1
				*	@note
				*
				**/
				inline	U1		HasChange(){return	m_bIsChange;};
			protected:
				IProperty*	m_pParent;		///<	父节点属性
				Real		m_fVolume;		///<	音量
				Real		m_fPitch;		///<	音调
				U1			m_bIsChange;	///<	是否已经改变
			};
			/**	\brief	声音
			*
			*	声音
			*
			***/
			class	ISound	:
				public	IProperty,
				public	MovableObject,
				public	Common::IProduct{
			public:
				/**	\brief	声音类型
				*
				*	声音类型
				*
				***/
				enum	enumType{
					enStatic,		///<	静态	比如背景音乐
					enDynamic,		///<	动态	比如场景里面可以移动的音乐
					enDirection,	///<	方向声音
					enMax
				};
				enum	enumState{
					enInit		=	0x1011,
					enPlaying	=	0x1012,
					enPause		=	0x1013,
					enStop		=	0x1014,
					enUnknown
				};
				/**	\brief	声音创建信息
				*
				*	声音创建信息
				*
				***/
				struct	Info{
					Info(){
						type		=	enStatic;
						strName		=	"..\\Data\\Audio\\1.wav";
						vPosition	=	Float3(0,0,0);
						fPitch		=	1.0f;
						fVolume		=	1.0f;
						iNumRepeat	=	1;
						fInner		=	360;
						fOuter		=	360;
						fDistance	=	5000.0f;
						pParentNode	=	NULL;
					};
					void	SetDynamic(AString	strSoundName,Float3	v	=	Float3(0,0,0),UInt	iRepeat	=	1){
						type		=	enDynamic;
						strName		=	strSoundName;
						vPosition	=	v;
						iNumRepeat	=	iRepeat;
						fInner		=	360;
						fOuter		=	360;
						fDistance	=	1000.0f;
						pParentNode	=	NULL;
					};
					void	SetDirection(AString	strSoundName,UInt	fInnerAngle,UInt	fOuterAngle,Float3	v	=	Float3(0,0,0),UInt	iRepeat	=	1){
						type		=	enDirection;
						strName		=	strSoundName;
						fInner		=	fInnerAngle;
						fOuter		=	fOuterAngle;
						vPosition	=	v;
						iNumRepeat	=	iRepeat;
						fDistance	=	2000.0f;
						pParentNode	=	NULL;
					};
					enumType	type;			///<	类型
					AString		strName;		///<	声音文件名
					Float3		vPosition;		///<	初始位置
					Real		fVolume;		///<	初始音量
					Real		fPitch;			///<	初始声音速度
					SInt		iNumRepeat;		///<	重复次数
					UInt		fInner;			///<	内圈
					UInt		fOuter;			///<	外圈
					Real		fDistance;		///<	声音范围
					SceneNode*	pParentNode;	///<	要绑定到的节点
				};
			public:
				ISound(CAString&	strName,Info*	pInfo):Common::IProduct(strName){

				};
				/**	\brief	获取声音类型
				*   
				*	@remarks 	获取声音类型
				*	@see		ISound
				*	@return   	Client::Audio::ISound::enumType
				*	@note
				*
				**/
				virtual	enumType	GetType()						=	NULL;
				/**	\brief	声音文件不需要渲染
				*   
				*	@remarks 	声音文件不需要渲染
				*	@see		ISound
				*	@return   	void
				*	@note
				*
				**/
				virtual	void		Render(){};
	
				/**	\brief	获取当前状态
				*   
				*	@remarks 	获取当前状态
				*	@see		ISound
				*	@return   	Client::Audio::ISound::enumState
				*	@note
				*
				**/
				virtual	enumState	GetState()=	NULL;
				/**	\brief	播放
				*   
				*	@remarks 	播放
				*	@see		ISound
				*	@return   	void
				*	@note
				*
				**/
				virtual	void		Play()=	NULL;
				/**	\brief	暂停
				*   
				*	@remarks 	暂停
				*	@see		ISound
				*	@return   	void
				*	@note
				*
				**/
				virtual	void		Pause()=	NULL;
				/**	\brief	停止
				*   
				*	@remarks 	停止
				*	@see		ISound
				*	@return   	void
				*	@note
				*
				**/
				virtual	void		Stop()=	NULL;
				/**	\brief	重新开始播放
				*   
				*	@remarks 	重新开始播放
				*	@see		ISound
				*	@return   	void
				*	@note
				*
				**/
				virtual	void		RePlay()=	NULL;
	
				/**	\brief	获取剩余次数
				*   
				*	@remarks 	获取剩余次数
				*	@see		ISound
				*	@return   	UInt
				*	@note
				*
				**/
				virtual	SInt		GetRepeatCount()	=	NULL;
	
				/**	\brief	更新声音
				*   
				*	@remarks 	更新位置信息
				*	@see		ISound
				*	@return   	void
				*	@note
				*
				**/
				virtual	void		UpdataSound()		=	NULL;
			};
		}
	
	};
};
#endif // INTERFACEAUDIOSOUND_HEAD_FILE