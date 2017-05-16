#pragma once
#include "IBCommon.h"
#include "IBChannelAdapter.h"
#include "IBCompletionQueue.h"

namespace hiveInfiniband
{
	class CIBConnection;
	class CIBCompletionQueue;

	class CIBQueuePair
	{
	private:
		struct SQueueAttributes
		{
			unsigned int QueueSize;
			unsigned int MaxNumScatterGatherElements;
			unsigned int CompletionQueueSize;
			bool         isEventDrivenCQ; //NOTE: otherwise is callback-driven CQ, but not implemented yet
			std::string  CompletionQueueHandlerThreadSig;

			SQueueAttributes() : QueueSize(512), MaxNumScatterGatherElements(16), CompletionQueueSize(DEFAULT_COMPLETION_QUEUE_SIZE), isEventDrivenCQ(true), CompletionQueueHandlerThreadSig(DEFAULT_IB_CQ_HANDLER_THREAD_SIGNATURE) {}
			bool isValid(const CIBChannelAdapter *vAdapter) const;
		};

	public:
		CIBQueuePair(void);
		~CIBQueuePair(void);

		bool createQueuePair(const CIBChannelAdapter *vAdapter, const SQueueAttributes& vSendQueue, const SQueueAttributes& vRecvQueue, bool vShareCompletionQueue);
		void close();
		
		void setReomoteIBInfo(ib_net16_t vLId, ib_net32_t vQPId) {m_RemoteIBInfo.LocalID = vLId; m_RemoteIBInfo.QueuePairID = vQPId;}
		ib_net32_t getRemoteQueuePairID() const {return m_RemoteIBInfo.QueuePairID;}

		bool changeState2RTR();
		bool changeState2RTS();
		void startCompletionQueueMonitorThread();
		void setHostConnection(const CIBConnection* vConn);
		ib_net32_t getQueuePairID() const {return m_LocalQueuePairID;}
		ib_qp_handle_t getQueuePairHandle() const   {return m_QueuePairHandle;}

	private:
		void __createCompletionQueues(const SQueueAttributes& vSendQueue, const SQueueAttributes& vRecvQueue, bool vShareCompletionQueue);
		void __outputQueuePairAttributes(const ib_qp_attr_t& vInfo);
		bool __changeState2INIT();
		bool __queryAttributes(bool vISOutputAttributes);

		std::string __getStateString(ib_qp_state_t vState);
		CIBCompletionQueue* __createCompletionQueue(const SQueueAttributes& vQueueAttr);

	private:
		struct SInfinibandNodeInfo
		{
			ib_net16_t	LocalID;  //An address assigned to a port by the subnet manager, unique within the subnet, used for directing packets within the subnet
			ib_net32_t	QueuePairID;

			SInfinibandNodeInfo() : LocalID(0), QueuePairID(0) {}
			SInfinibandNodeInfo(ib_net16_t vLId, ib_net32_t vQPId): LocalID(vLId), QueuePairID(vQPId) {}
		};

		const CIBChannelAdapter *m_pHostAdapter;
		SInfinibandNodeInfo      m_RemoteIBInfo;  
		ib_qp_handle_t           m_QueuePairHandle;
		ib_net32_t               m_LocalQueuePairID;  //the ID of local Queue Pair
		ib_qp_attr_t             m_CurrentState;      //?
		CIBCompletionQueue      *m_pRecvCompletionQueue;
		CIBCompletionQueue	    *m_pSendCompeletionQueue;
		bool                     m_IsShareCompletionQueue; 

		friend class CIBConnection;  //使得IBConnection可以访问SQueueAttributes
	};
}