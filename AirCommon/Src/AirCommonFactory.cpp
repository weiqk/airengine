#include "AirCommonFactory.h"


namespace Air{
	
	namespace Common{
	
	
		IBaseFactory::IBaseFactory(){
			m_strTypeName = ("BaseFactory");
		}
	
		IBaseFactory::~IBaseFactory(){
	
		}
	
		void IBaseFactory::SetType( const AString& strTypeName ){
			m_strTypeName = strTypeName;
		}
	
		const AString& IBaseFactory::GetType()const{
			return m_strTypeName;
		}
	
		IFactory::IFactory():IBaseFactory(){
			m_pFactoryMgr	= NULL;
		}
	
		IFactory::~IFactory(){
			DestroyAllProduct();
	
		}
	
		bool IFactory::Destroy( IProduct* pProduct ){
			if(pProduct!=NULL){
				pProduct->ReleaseRef();
			}
			return true;
		}
	
		U32 IFactory::GetProductCount()const{
			return m_mapProduct.size();
		}
	
		IProduct* IFactory::GetProduct( const AString& strName ){
			IProduct*	pProduct	=	NULL;
			m_CS.Enter();
			ProductMap::const_iterator itr = m_mapProduct.find(strName.c_str());
			if(itr!=m_mapProduct.end())
				pProduct	= itr->second;
			m_CS.Leave();
			if(pProduct!=NULL)
				pProduct->AddRef();
			return pProduct;
		}
	
		bool IFactory::Exist( const AString& strName )const{
			ProductMap::const_iterator itr = m_mapProduct.find(strName);
			return	(itr!=m_mapProduct.end());
		}
	
		bool IFactory::Erase(const AString& strName ){
			m_CS.Enter();
			ProductMapItr itr = m_mapProduct.find(strName.c_str());
			if(itr!=m_mapProduct.end()){
				m_mapProduct.erase(itr);
			}
			m_CS.Leave();
			return	true;
		}
	
		bool IFactory::Erase( IProduct* pProduct ){
			if(pProduct==NULL)
				return false;
			return Erase(pProduct->GetProductName());
		}
	
		void IFactory::Insert( IProduct* pProduct ){
			if(pProduct==NULL)
				return;
			m_CS.Enter();
			m_mapProduct[pProduct->GetProductName().c_str()]	=	pProduct;
			m_CS.Leave();
		}
	
		void IFactory::SetFactoryManager( IFactoryManager* pFactoryMgr ){
			m_pFactoryMgr = pFactoryMgr;
		}
	
		bool IFactory::DestroyAllProduct(){
			//����Ʒ�б�����������һ���б� ��ԭ�����б��������
			//�������� ��DELETE IProduct��ʱ�� ����� IProduct���������� �������õ�IFactory::Erase ���յ��µ�����������������
			while(!m_mapProduct.empty()){
				ProductMap lst = m_mapProduct;
			
				ProductMapItr i = lst.begin();
				for(;i!=lst.end();i++){
					IProduct* &p = i->second;
					if(p!=NULL){
						p->ReleaseRef();
					}
				}
				lst.clear();
			}
			return true;
		}
	
		const IFactory::ProductMap& IFactory::GetProductList()const{
			return	m_mapProduct;
		}
		IFactoryManager::IFactoryManager(): IBaseFactory(){
			
		}
	
		IFactoryManager::~IFactoryManager(){
			DestroyAllFactory();
		}
	
		void IFactoryManager::AddFactory( IFactory* pFactory ){
			if(pFactory==NULL)
				return;
			FactoryMapItr	i	=	m_mapFactory.find(pFactory->GetType());
			if(i!=m_mapFactory.end()){
				m_lstFactory.remove(i->second);
				SAF_D(i->second);
				i->second	=	pFactory;
				m_lstFactory.push_back(pFactory);
			}else{
				m_lstFactory.push_back(pFactory);
				m_mapFactory.insert(FactoryMapPair(pFactory->GetType().c_str(),pFactory));
			}
			pFactory->SetFactoryManager(this);
		}
	
		void IFactoryManager::RemoveFactory( IFactory* pFactory ){
	
			FactoryMapItr itr = m_mapFactory.begin();
			for(;itr!=m_mapFactory.end();itr++){
				IFactory* p = itr->second;
				if(pFactory == p){
					m_lstFactory.remove(pFactory);
					SAF_D(p);
					m_mapFactory.erase(itr);
					return;
				}
			}
		}
	
		void IFactoryManager::RemoveFactory( const AString& strFactoryName ){
			FactoryMapItr i = m_mapFactory.find(strFactoryName.c_str());
			if(i!=m_mapFactory.end()){
				IFactory*	pFactory	=	i->second;
				m_lstFactory.remove(pFactory);
				SAF_D(pFactory);
				m_mapFactory.erase(i);
			}
		}
	
		bool IFactoryManager::DestroyAllFactory(){
	// 		FactoryMapItr i = m_mapFactory.begin();
	// 		for(;i != m_mapFactory.end();i++){
	// 			IBaseFactory* pFactory = i->second;
	// 			SAF_D(pFactory);
	// 		}
			//�����ͷ����й���  �����빤��֮�������������
			FactoryList::reverse_iterator	i	=	m_lstFactory.rbegin();
			for(;i!=m_lstFactory.rend();i++){
				IFactory*	pFactory	=	(IFactory*)(*i);
				SAF_D(pFactory);
			}
			m_lstFactory.clear();
			m_mapFactory.clear();
			return true;
		}
	
		IFactory* IFactoryManager::GetFactory( const AString& strFactoryName )const{
			FactoryMap::const_iterator i = m_mapFactory.find(strFactoryName.c_str());
			if(i!=m_mapFactory.end()){
				IFactory* pFactory = i->second;//dynamic_cast<IFactory<T_Product>*>(i->second);
				return pFactory;
			}
			return NULL;
		}
	
		bool IFactoryManager::Destroy( IProduct* pProduct){
			if(pProduct==NULL)
				return false;
			IFactory* pFactory = GetFactory(pProduct->GetProductTypeName());
			if(pFactory == NULL)
				return false;
			return pFactory->Destroy(pProduct);
		}
	
		IProduct* IFactoryManager::CreateProduct( const AString& strName,const AString& strFactoryName,IFactoryParamList* lstParam /*= NULL*/ ){
			if(strName.empty())
				return	NULL;
			//���ҹ���
			IFactory* pFactory = GetFactory(strFactoryName);
			if(pFactory == NULL)
				return 0;
	
			m_CS.Enter();
			//�����Ƿ��Ѿ����� ����ֱ�ӷ���
			IProduct* pProduct = pFactory->GetProduct(strName);

			if(pProduct==NULL){
				//δ������ֱ�Ӵ���
				pProduct = pFactory->NewProduct(strName,lstParam);
				if(pProduct!=NULL){
					pProduct->SetFactory(pFactory);
					pProduct->SetFactoryManager(this);
					//���Ƕ��̼߳���
					//�������ü���
					if(pProduct->AddRef()==0){
						pFactory->Destroy(pProduct);
						m_CS.Leave();
						return NULL;
					}
					pFactory->Insert(pProduct);
				}
			}
			m_CS.Leave();
			return pProduct;
		}
	
		IProduct* IFactoryManager::GetProduct(const AString& strName,const AString& strFactoryName)const{
			//���ҹ���
			IFactory* pFactory = GetFactory(strFactoryName);
			if(pFactory == NULL)
				return 0;
			//�����Ƿ��Ѿ����� ����ֱ�ӷ���
			IProduct* pProduct = pFactory->GetProduct(strName);
			return	pProduct;
		}
	
		bool IFactoryManager::DestroyAllProduct(){
	// 		FactoryMapItr i = m_mapFactory.begin();
	// 		if(i!=m_mapFactory.end()){
	// 			IFactory* pFactory = i->second;//dynamic_cast<IFactory<T_Product>*>(i->second);
	// 			if(pFactory!=NULL){
	// 				pFactory->DestroyAllProduct();
	// 			}
	// 		}
			//�����ͷ����й���  �����빤��֮�������������
			FactoryList::reverse_iterator	i	=	m_lstFactory.rbegin();
			for(;i!=m_lstFactory.rend();i++){
				IFactory*	pFactory	=	(IFactory*)(*i);
				if(pFactory!=NULL){
					pFactory->DestroyAllProduct();
				}
			}
			return true;
		}
	
	// 	bool IFactoryManager::DestroyProduct( void* pProduct ){
	// 		IProduct*	p	=	dynamic_cast<IProduct*>(pProduct);
	// 		Destroy(p);
	// 		return true;
	// 	}	
	}
};