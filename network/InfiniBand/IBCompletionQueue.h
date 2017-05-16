#pragma once
#include "IBCommon.h"

namespace hiveInfiniband
{
	class CIBChannelAdapter;
	class CIBConnection;
	class CIBWCEHandlingThread;

	class CIBCompletionQueue
	{
	public:
		enum ECompletionQueueType
		{
			UNDEFINED,
			EVENT_DRIVEN,
			CALLBACK_DRIVEN
		};

	public:
		CIBCompletionQueue(void);
		~CIBCompletionQueue(void);

		void close();
		bool createEventDrivenCQ(const CIBChannelAdapter *vAdapter, uint32_t vQueueSize, const std::string& vCQHandlingThreadSig);
		void startMonitorThread();
		void setHostConnection(const CIBConnection* vConn) {_ASSERT(vConn && !m_pHostConnection); m_pHostConnection = vConn;}
		
		const CIBConnection*        getHostConnection()  const {return m_pHostConnection;}
		const ECompletionQueueType  getType()            const {return m_Type;}
		const ib_cq_handle_t	    getCompletionQueue() const {return m_CompletionQueue;}

		HANDLE getCompletionOccurEvent()           const {_ASSERT(m_Type == EVENT_DRIVEN); return m_CompletionEvent;}

		CIBConnection* fetchHostConnection() const { return const_cast<CIBConnection*>(m_pHostConnection); }

	private:
		HANDLE         m_CompletionEvent;
		ib_cq_handle_t m_CompletionQueue;
		ECompletionQueueType   m_Type;
		const CIBConnection*   m_pHostConnection;
		CIBWCEHandlingThread*  m_pWorkCompletionHandleThread;
	};
}