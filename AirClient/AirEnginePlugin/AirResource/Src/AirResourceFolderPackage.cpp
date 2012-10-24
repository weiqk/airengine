#include "AirResourceFolderPackage.h"

namespace	Air{
	namespace	Client{
		namespace	Resource{

			FolderPackage::FolderPackage( CAString& strName ):IPackage(strName){
				m_pTempListener	=	NULL;
			}

			U32			FolderPackage::Find(CAString&	strName,Data&	data){
				return	FindWithFullPath(m_strProductName	+	"\\"	+	strName,data);
			}

			void FolderPackage::FindWithPostfix( CAString& strPostfix,IFindFileListener* pListener ){
				m_pTempListener		=	pListener;
				m_strTempPostfix	=	AString(".")+strPostfix;
				File::FolderTraversal(m_strProductName,this);
				m_pTempListener		=	NULL;
				//strPostfix			=	AString("");
			}

			Air::U1 FolderPackage::Create(){
				return	true;
			}

			Air::U1 FolderPackage::OnFindFile( CAString& strName ){
				if(!Common::EndWith(strName,m_strTempPostfix))
					return	false;

				Data	data;
				FindWithFullPath(strName,data);
				if(	m_pTempListener	!=	NULL){
						m_pTempListener->OnFindFile(strName,&data);
				}
				return	true;
			}

			Air::U1 FolderPackage::OnFindDirectory(CAString& strName ){
				return	true;
			}

			U32		FolderPackage::FindWithFullPath( CAString& filename ,Data&	data){
				U8*	p			=	NULL;
				U32		uiSize	=	0;
				if(Common::File::Load(filename,p,uiSize)){
					data.Clear();
					data.buff	=	p;
					data.size	=	uiSize;
					return	data.size;
				}
				return	NULL;
			}
		}
	}
}