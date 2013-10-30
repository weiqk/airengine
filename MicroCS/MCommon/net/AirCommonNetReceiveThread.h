#ifndef COMMONNETRECEIVETHREAD_HEAD_FILE
#define COMMONNETRECEIVETHREAD_HEAD_FILE

#include "AirCommonNet.h"
#include "AirCommonThread.h"
#include "AirCommonLock.h"

#define DEFAULT_RECEIVE_SIZE	1024*1024

namespace Air{
	
	
	namespace Common{
	
		/**	\brief	�����߳�
		*
		*	�����߳�
		*
		***/
		class	MCOMMON_EXPORT	NetReceiveThread	
			:	public	Thread{
		public:
			NetReceiveThread();
	
			/**	\brief	����
			*   
			*	@remarks 	����
			*	@see		NetReceiveThread
			*	@return   	U1
			*	@param		U32 uiSocket
			*	@param		NetListener * pListener
			*	@note
			*
			**/
			virtual	U1	Start(U32	uiSocket,NetListener*	pListener);
	
			/**	\brief	��Ϣ����ѭ��
			*   
			*	@remarks 	��Ϣ����ѭ��
			*	@see		NetReceiveThread
			*	@return   	U1
			*	@note
			*
			**/
			virtual	U1	RepetitionRun();
	
			U32				m_Socket;					///<	SOCKET	ID
			S32				m_ReceiveSize;				///<	���յ������ݴ�С
			AChar			m_ReceiveBuffer[DEFAULT_RECEIVE_SIZE];	///<	���ջ�����
			NetListener*	m_pListener;				///<	������
		};
		
		typedef	std::map<U32,NetReceiveThread*>	ReceiveThreadMap;
		typedef	std::list<NetReceiveThread*>	ReceiveThreadList;
	
	};
};
#endif // COMMONNETRECEIVETHREAD_HEAD_FILE