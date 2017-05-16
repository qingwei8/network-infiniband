#pragma once
#include "NetworkThread.h"
#include <boost/function.hpp>
#include "common/Timer.h"
#include "IBCommon.h"

namespace hiveInfiniband
{
	class CIBCompletionQueue;
	class CIBMemoryBlock;
	class CIBPacket;

	class CPerformanceTester
	{
	public:
		CPerformanceTester(void) : m_NumTargetRequest(0) {}
		~CPerformanceTester(void) {}

		void tickReceive(unsigned int vSize);
		void tickSend(unsigned int vSize);
		void setNumTargetRequest(unsigned int vNumTargetRequest) { m_NumTargetRequest = vNumTargetRequest; }

	private:
		static hiveCommon::CTimer  m_ReceiveTimer;
		static hiveCommon::CTimer  m_SendTimer;
		static unsigned int m_NumReceiveRequest;
		static unsigned int m_NumSendRequest;
		unsigned int        m_NumTargetRequest;
		static double       m_ReceivedMegaBytes;
		static double       m_SentMegaBytes;
		static boost::mutex m_SendMutex;
		static boost::mutex m_ReceiveMutex;
	};

	class CIBWCEHandlingThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		CIBWCEHandlingThread(void);
		~CIBWCEHandlingThread(void);

		void setHostCompletionQueue(CIBCompletionQueue *vCompletionQueue);

	private:
		virtual void __runV() override;

		void __onDataReceived(const ib_wc_t *vWorkCompletion, const CIBMemoryBlock *vMemoryBlock);

		CIBMemoryBlock* __fetchMemoryBlockOfWorkCompletion(ib_wc_t *vWorkCompletion, EWorkRequestType vWorkReqType);

	private:
		CIBCompletionQueue *m_pHostCompletionQueue;

		void __performanceTest(int vNumTargetRequest);
	};
}