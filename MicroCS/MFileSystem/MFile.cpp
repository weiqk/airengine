#include "MFile.h"
#include "MIOSystem.h"
#include "MDownloadSystem.h"
#include "MClient.h"
#include "MFileSystem.h"

AString	MFile::ProductTypeName	=	"MFile";
MFile::MFile( CAString& strName,Info* pInfo ):Air::Common::IProduct(strName)
{
	m_pInfo	=	pInfo;
	m_bDownloading	=	false;
}

U1 MFile::Create()
{
	if((m_pInfo->idx&0xffff0000)!=0)
		if(MIOSystem::GetSingleton()->LoadFile(*m_pInfo,m_Data)){
			return true;
	}
	m_bDownloading	=	true;
	MDownloadSystem::GetSingleton()->AddFile(this);
	return true;
}

U1 MFile::Destroy()
{
	m_Data	=	STD_VECTOR<U8>();
	return true;
}

void MFile::OnDownloading( U32 uiOffset,const void* p,U32 uiSize )
{
	if(m_Data.size()<uiOffset+uiSize){
		m_Data.resize(uiOffset+uiSize);
	}
	memcpy(&m_Data[uiOffset],p,uiSize);
}

void MFile::OnDownloadComplated(U1	bOK)
{

	m_NotifyCS.Enter();
	m_bDownloading	=	false;
	STD_LIST<NotifyInfo>::iterator	i	=	m_lstNotify.begin();
	for(;i!=m_lstNotify.end();i++){
		if(!bOK){
			i->pClient->OnFileLoadFailed(i->uiOffset,this);
		}else{
			i->pClient->OnFileLoadComplated(i->uiOffset,this);
		}
		i->pClient->ReleaseRef();
	}
	m_lstNotify.clear();
	m_NotifyCS.Leave();
	
}

void MFile::AddNotify( const NotifyInfo& info )
{

	m_NotifyCS.Enter();
	if(!m_bDownloading){
		if(m_Data.empty()){
			info.pClient->OnFileLoadFailed(info.uiOffset,this);
		}else{
			info.pClient->OnFileLoadComplated(info.uiOffset,this);
		}
		m_NotifyCS.Leave();
		return;
	}
	info.pClient->AddRef();
	m_lstNotify.push_back(info);
	m_NotifyCS.Leave();
}

void* MFile::GetData()
{
	if(m_Data.empty())
		return NULL;
	return &m_Data[0];
	//U32 uiMinSize	=	min(uiSize,m_Data.size());
	//memcpy(pOut,&m_Data[0],uiMinSize);
}

U32 MFile::GetDataSize()
{
	return m_Data.size();
}

FileInfo& MFile::GetFileInfo()
{
	return *m_pInfo;
}

U32 MFile::GetFileIndexRA()
{
	U32	base	=	(U32)&MFileSystem::GetSingleton()->GetFileInfo(0);
	return ((U32)&m_pInfo->idx	-	base);
}
