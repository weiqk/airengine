#ifndef COMMONLOCK_HEAD_FILE
#define COMMONLOCK_HEAD_FILE

#include "AirCommonHeader.h"
namespace Air{
	
	namespace Common{
		/**	\brief	�ٽ�������
		*
		*	�ٽ�������
		*
		***/
		class	CriticalSection{
		public:
			inline	CriticalSection(){
				InitializeCriticalSection(&m_CriticalSection);
			};
	
			~CriticalSection(){
				DeleteCriticalSection(&m_CriticalSection);
			};
	
			/**	\brief	�����ٽ���
			*   
			*	@remarks 	�����ٽ���
			*	@see		CriticalSection
			*	@return   	void
			*	@note
			*
			**/
			inline	void	Enter(){
				EnterCriticalSection(&m_CriticalSection); 
			};
	
			/**	\brief	�뿪�ٽ���
			*   
			*	@remarks 	�뿪�ٽ���
			*	@see		Lock
			*	@return   	void
			*	@note
			*
			**/
			inline	void	Leave(){
				LeaveCriticalSection(&m_CriticalSection); 
			};
	
			CRITICAL_SECTION	m_CriticalSection;//�ٽ������ü���
		};
	
		/**	\brief	�������
		*
		*	�������
		*
		***/
		class	Mutex{
		public:
			Mutex(AString	strName	=	""){
				m_hMutex	=	CreateMutexA(NULL,FALSE,strName.c_str());
			};
			~Mutex(){
				CloseHandle(m_hMutex);
			}
	
			/**	\brief	�ȴ��������
			*   
			*	@remarks 	�ȴ��������
			*	@see		Mutex
			*	@return   	void
			*	@param		U32 uiTime
			*	@note
			*
			**/
			inline	void	Enter(U32	uiTime	=	INFINITE){
				WaitForSingleObject(m_hMutex,uiTime);
			};
	
			/**	\brief	�ͷŻ������
			*   
			*	@remarks 	�ͷŻ������
			*	@see		Mutex
			*	@return   	void
			*	@note
			*
			**/
			inline	void	Leave(){
				ReleaseMutex(m_hMutex);
	
			};
	
			HANDLE	m_hMutex;	///<	���������
		};
		
		/**	\brief	�¼�
		*
		*	�¼�
		*
		***/
		class	Event{
		public:
			Event(AString&	strName){
				m_hEvent	=	CreateEventA(NULL,FALSE,FALSE,strName.c_str());
			};
			~Event(){
				CloseHandle(m_hEvent);
			}

			/**	\brief	�����¼�
			*   
			*	@remarks 	�����¼�
			*	@see		Event
			*	@return   	void
			*	@note
			*
			**/
			void	Reset(){
				SetEvent(m_hEvent);
			}
			/**	\brief	�ȴ��¼�
			*   
			*	@remarks 	�ȴ��¼�
			*	@see		Event
			*	@return   	void
			*	@param		U32		uiTime
			*	@note
			*
			**/
			void	Wait(U32	uiTime	=	INFINITE){
				WaitForSingleObject(m_hEvent,uiTime);
			}
			HANDLE	m_hEvent;
		};
#ifndef	LOCK_ADD
#define LOCK_ADD(x,val)	InterlockedAdd(&x,val)
#endif
#ifndef	LOCK_ADD_1
#define LOCK_ADD_1(x)	InterlockedIncrement(&x)
#endif
#ifndef	LOCK_SUB_1
#define LOCK_SUB_1(x)	InterlockedDecrement(&x)
#endif
	};
};
#endif // COMMONLOCK_HEAD_FILE