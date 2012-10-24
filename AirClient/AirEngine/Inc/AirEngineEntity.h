#ifndef ENGINEENTITY_HEAD_FILE
#define ENGINEENTITY_HEAD_FILE

#include "AirEngineHeader.h"
#include "AirEngineSceneNode.h"
namespace Air{
	
	namespace Client{
		namespace	Physics{
			class	ICollision;
			class	IScene;
			class	ITriggerCallback;
		}
		class	SubEntity;
	
		/**	\brief	�������������
		*
		*	�������������
		*
		***/
		class	Entity	:
			public	IProduct,
			public	Common::INodeListener{
		public:
			struct Info{
				Info(){
					pParentNode	=	NULL;
					pXScene		=	NULL;
					pCallback	=	NULL;
				};
				AString						strConfigName;		///<	�����ļ���
				AString						strConfigMemery;	///<	��������
				SceneNode*					pParentNode;		///<	���ڵ�
				Physics::IScene*				pXScene;			///<	����ָ��
				Physics::ITriggerCallback*	pCallback;			///<	�������ص�
			};
	
		public:
			Entity(CAString&	strName,Info*	pInfo);
	
			/**	\brief	����
			*   
			*	@remarks 	����
			*	@see		Entity
			*	@return   	U1
			*	@note
			*
			**/
			virtual	U1					Create();
	
			/**	\brief	�ݻ�
			*   
			*	@remarks 	�ݻ�
			*	@see		Entity
			*	@return   	U1
			*	@note
			*
			**/
			virtual	U1					Destroy();
	
			/**	\brief	��ȡ�ڵ�
			*   
			*	@remarks 	��ȡ�ڵ�
			*	@see		Entity
			*	@return   	SceneNode*
			*	@note
			*
			**/
			virtual	SceneNode*			GetSceneNode();
	
			/**	\brief	��ȡ��ײģ��
			*   
			*	@remarks 	��ȡ��ײģ��
			*	@see		Entity
			*	@return   	Physx::ICollision*
			*	@note
			*
			**/
			virtual	Physics::ICollision*	GetCollision();
	
			/**	\brief	��ȡ��Ⱦģ��
			*   
			*	@remarks 	��ȡ��Ⱦģ��
			*	@see		Entity
			*	@return   	SubEntity*
			*	@note
			*
			**/
			virtual	SubEntity*			GetSubEntity();
	
			/**	\brief	����
			*   
			*	@remarks 	�˺����ɽڵ�(INode)������ ���������һЩ���¶���
			*	@see		IListener
			*	@return   	void
			*	@param		Float3 vPos
			*	@param		Float4 vQuat
			*	@param		Float3 vScale
			*	@note
			*
			**/
			virtual void UpdataMatrix(Float44* pMatrix,Float4*	pQuat,Float3*	pScale);
		protected:
			/**	\brief	���������ļ�
			*   
			*	@remarks 	���������ļ�
			*	@see		Entity
			*	@return   	U1
			*	@note
			*
			**/
			U1	ParseConfig();
	
		protected:
			SubEntity*		m_pRenderObj;			///<	��Ⱦ����
			Physics::ICollision*	m_pCollisionObj;		///<	��ײģ��
			SceneNode*			m_pNode;				///<	�ڵ�
			Info				m_Info;					///<	������Ϣ
		};
	
		class	EntityFactory	:
			public	IFactory{
		public:
			EntityFactory(){
				m_strTypeName	=	"Entity";
			};
	
			virtual	IProduct*	NewProduct(CAString& strName,IFactoryParamList* lstParam /* = NULL */){
				if(lstParam==NULL)
					return	NULL;
				return	new	Entity(strName,(Entity::Info*)lstParam);
			};
		};
	
	};
};
#endif // ENGINEENTITY_HEAD_FILE