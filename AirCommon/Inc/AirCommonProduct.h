
/*****************************************************/
/* @All Right Reserved (C), 2008-2010, 82NET TECO CO.


 \author    InfernalHell	Xinhe
 \date      2009/11/05
 \version   ver 0.6.0.0
 \filepath  f:\Air\AirCommon\Inc
 \filename  InterfaceProduct.h
 \note     
       
 \attention:
			��Ʒģ��
 \remarks    created by InfernalHell version 0.6.0.0 
 \History:
   <author>      <time>      <version>         <desc>
******************************************************/

#ifndef COMMONPRODUCT_HEAD_FILE
#define COMMONPRODUCT_HEAD_FILE


#include "AirCommon.h"
#include "AirCommonHeader.h"
#include "AirCommonReference.h"
#include "AirCommonLock.h"
namespace Air{
	
	namespace Common{
		class IFactoryManager;
	
		/**	\brief	��Դ��Դ����
		*
		*	��Դ��Դ����
		*
		***/
		enum enumSourceType{
			SOURCE_TYPE_NEW,			///<	�½�
			SOURCE_TYPE_FILE,			///<	���ļ�����
			SOURCE_TYPE_MEMERY,			///<	���ڴ����
			SOURCE_TYPE_RESOURCE		///<	����Դ����
		};
		/**	\brief	��Ʒģ��
		*
		*	��Ʒģ��
		*
		***/
		class COMMON_EXPORT IProduct	:	
			public	IReference{
		friend class IFactory;
		public:
			IProduct(const AString& strName);
			///virtual ~IProduct();
		
			/**	\brief	��ȡ������
			*   
			*	@remarks 	
			*	@see		IProduct
			*	@return   	T_Scene*
			*	@note
			*
			**/
			template <typename T_Scene>
			T_Scene* GetCreator(){
				return (T_Scene*)m_pFactoryMgr;
			};
	
			/**	\brief	���ز�Ʒ������
			*   
			*	@remarks 	���ز�Ʒ������
			*	@see		IProduct
			*	@return   	String
			*	@note
			*
			**/
			virtual const	AString& GetProductTypeName()const;
			/**	\brief	���ز�Ʒ��
			*   
			*	@remarks 	���ز�Ʒ��
			*	@see		IProduct
			*	@return   	String
			*	@note
			*
			**/
			const AString& GetProductName()const;

			void		SetProductName(CAString& strName);
			/**	\brief	���ù���������
			*   
			*	@remarks 	���ù���������
			*	@see		IProduct
			*	@return   	void
			*	@param		IFactoryManager * pFactoryMgr
			*	@note
			*
			**/
			void		SetFactoryManager(IFactoryManager* pFactoryMgr);
	
			/**	\brief	��ȡ����������
			*   
			*	@remarks 	��ȡ����������
			*	@see		IProduct
			*	@return   	IFactoryManager*
			*	@note
			*
			**/
			IFactoryManager*	GetFactoryManager(){
				return	m_pFactoryMgr;
			};
			/**	\brief	���ù���ָ��
			*   
			*	@remarks 	���ù���ָ��
			*	@see		IProduct
			*	@return   	void
			*	@param		IFactory * pFactory
			*	@note
			*
			**/
			void				SetFactory(IFactory*	pFactory){
				m_pFactory	=	pFactory;
			};
			/**	\brief	�������ü���
			*   
			*	@remarks 	�������ü���
			*	@see		IProduct
			*	@return   	U32
			*	@note
			*
			**/
			virtual	U32		AddRef();
	
			/**	\brief	��������
			*   
			*	@remarks 	��������
			*	@see		IReference
			*	@return   	U32
			*	@note
			*
			**/
			virtual	U32		ReleaseRef();
			virtual	U32		GetSize();
			void			SetUserData(void*	pUserData);
			void*			GetUserData();
			void			SetUserString(CAString&	strUserString);
			AString			GetUserString();
			void			GetUserString(AString&	strUserString);
		protected:
			IFactoryManager*	m_pFactoryMgr;					///<	����ָ��
			IFactory*			m_pFactory;						///<	����ָ��
			AString				m_strProductName;				///<	��Ʒ��
			AString				m_strUserString;
			void*				m_pUserData;
		};

	};
	template<typename T>
	void	TReleaseRef(void*&	pProduct){
		if(pProduct!=NULL){
			T*	p	=	(T*)pProduct;
			p->ReleaseRef();
			pProduct	=	NULL;
		}
	}
	typedef	std::list<Common::IProduct*>	ProductList;
};

#endif // INTERFACEPRODUCT_HEAD_FILE