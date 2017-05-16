#include "IBWCEHandlingThread.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/ProductFactory.h"
#include "common/HiveConfig.h"
#include "NetworkInterface.h"
#include "IBCompletionQueue.h"
#include "IBConnection.h"
#include "IBMemoryBlock.h"
#include "IBPacket.h"
#include "IBNode.h"

using namespace hiveInfiniband;

hiveCommon::CProductFactory<CIBWCEHandlingThread> theCreator(DEFAULT_IB_CQ_HANDLE_THREAD_SIG);

unsigned int CPerformanceTester::m_NumReceiveRequest = 0;
unsigned int CPerformanceTester::m_NumSendRequest = 0;
double CPerformanceTester::m_ReceivedMegaBytes = 0;
double CPerformanceTester::m_SentMegaBytes = 0;
boost::mutex CPerformanceTester::m_ReceiveMutex;
boost::mutex CPerformanceTester::m_SendMutex;
hiveCommon::CTimer CPerformanceTester::m_ReceiveTimer;
hiveCommon::CTimer CPerformanceTester::m_SendTimer;

CIBWCEHandlingThread::CIBWCEHandlingThread(void)
{
	_setThreadName("CIBWorkCompletionEHandlingThread");
}

CIBWCEHandlingThread::~CIBWCEHandlingThread(void)
{
}

//*********************************************************************************
//FUNCTION:
void CIBWCEHandlingThread::setHostCompletionQueue(CIBCompletionQueue *vCompletionQueue)
{
	_ASSERT(vCompletionQueue);
	m_pHostCompletionQueue = vCompletionQueue;
	_setHostLogicalNode(hiveNetworkCommon::hiveFindLogicalNodeByProtocal(PROTOCAL_IB));
}

//*********************************************************************************
//FUNCTION:
void CIBWCEHandlingThread::__runV()
{
	_ASSERT(m_pHostCompletionQueue && (m_pHostCompletionQueue->getType() == CIBCompletionQueue::EVENT_DRIVEN) && getHostLogicalNode());

	const hiveConfig::CHiveConfig *pConfig = getHostLogicalNode()->getConfig();
	_ASSERTE(pConfig);
	if (pConfig->getAttribute<bool>(CONFIG_KEYWORD::IB_PERFORMANCE_TEST_MODE))
	{
		__performanceTest(pConfig->getAttribute<int>(CONFIG_KEYWORD::IB_PERFORMANCE_TEST_NUM_REQUEST));
	}
	else
	{
		while (isRunning())
		{
			boost::this_thread::interruption_point();

			ib_wc_t  CompletionData;
			ib_wc_t* pComletionData;
			ib_wc_t* pSucceededRequest;
			ib_api_status_t rc;

			while (true)
			{
				pSucceededRequest = nullptr;
				CompletionData.p_next = nullptr;
				pComletionData = &CompletionData;

				boost::this_thread::interruption_point();

				rc = ib_poll_cq(m_pHostCompletionQueue->getCompletionQueue(), &pComletionData, &pSucceededRequest);
				if (rc == IB_NOT_FOUND) break;
				if (rc == IB_SUCCESS)
				{
					if (pSucceededRequest->status == IB_WCS_SUCCESS)
					{
						CIBMemoryBlock *pMemoryBlock = nullptr;
						switch (pSucceededRequest->wc_type)
						{
						case IB_WC_SEND:
							pMemoryBlock = __fetchMemoryBlockOfWorkCompletion(pSucceededRequest, SEND_TYPE);
							_ASSERT(pMemoryBlock);
							pMemoryBlock->unlockIBMemoryBlock();
#ifdef _DEBUG
							hiveCommon::hiveOutputEvent(_BOOST_STR1("The IB send work request [%1%] is done.", pMemoryBlock->getWorkRequestID()));
#endif
							break;
						case IB_WC_RECV:
							pMemoryBlock = __fetchMemoryBlockOfWorkCompletion(pSucceededRequest, RECV_TYPE);
							_ASSERT(pMemoryBlock);
							pMemoryBlock->setIBDataSize(pSucceededRequest->length);
							__onDataReceived(pSucceededRequest, pMemoryBlock);
							pMemoryBlock->unlockIBMemoryBlock();
#ifdef _DEBUG
							hiveCommon::hiveOutputEvent(_BOOST_STR1("The IB receive work request [%1%] is done.", pMemoryBlock->getWorkRequestID()));
#endif
							break;
						default:
							hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Unknown work completion type.");
						}
					}
				}
			}

			WaitForSingleObject(m_pHostCompletionQueue->getCompletionOccurEvent(), 100);  //FIXME-020: magical number here
		}
	}
}

//*********************************************************************************
//FUNCTION:
void CIBWCEHandlingThread::__performanceTest(int vNumTargetRequest)
{
	_ASSERTE(vNumTargetRequest > 0);
	
	CPerformanceTester PerformanceTester;
	PerformanceTester.setNumTargetRequest(vNumTargetRequest);

	while (isRunning())
	{
		boost::this_thread::interruption_point();

		ib_wc_t  CompletionData;
		ib_wc_t* pComletionData;
		ib_wc_t* pSucceededRequest;
		unsigned int IBDataSize = 0;
		ib_api_status_t rc;

		while (true)
		{
			pSucceededRequest = nullptr;
			CompletionData.p_next = nullptr;
			pComletionData = &CompletionData;

			boost::this_thread::interruption_point();

			rc = ib_poll_cq(m_pHostCompletionQueue->getCompletionQueue(), &pComletionData, &pSucceededRequest);
			if (rc == IB_NOT_FOUND) break;
			if (rc == IB_SUCCESS)
			{
				while (pSucceededRequest)
				{
					if (pSucceededRequest->status == IB_WCS_SUCCESS)
					{
						CIBMemoryBlock *pMemoryBlock = nullptr;
						switch (pSucceededRequest->wc_type)
						{
						case IB_WC_SEND:
							pMemoryBlock = __fetchMemoryBlockOfWorkCompletion(pSucceededRequest, SEND_TYPE);
							IBDataSize = pMemoryBlock->getUsedBufferSize();
							_ASSERT(pMemoryBlock);
							pMemoryBlock->unlockIBMemoryBlock();

							PerformanceTester.tickSend(IBDataSize);
							break;
						case IB_WC_RECV:
						{
							_ASSERTE(pSucceededRequest->length > 0);
							pMemoryBlock = __fetchMemoryBlockOfWorkCompletion(pSucceededRequest, RECV_TYPE);
							_ASSERT(pMemoryBlock);
							pMemoryBlock->setIBDataSize(pSucceededRequest->length);

							CIBPacket *pPacket = new CIBPacket;
							pPacket->setData(pMemoryBlock->getAddr(), pSucceededRequest->length);
							delete pPacket;

							pMemoryBlock->unlockIBMemoryBlock();

							PerformanceTester.tickReceive(pSucceededRequest->length);
						}
						break;
						default:
							hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Unknown work completion type.");
						}
					}
					pSucceededRequest = pSucceededRequest->p_next;
				}
			}
		}

		WaitForSingleObject(m_pHostCompletionQueue->getCompletionOccurEvent(), 10);  //FIXME-020: magical number here
	}
}

//*********************************************************************************
//FUNCTION:
void CIBWCEHandlingThread::__onDataReceived(const ib_wc_t *vWorkCompletion, const CIBMemoryBlock *vMemoryBlock)
{
	_ASSERTE(vMemoryBlock && vWorkCompletion);

	CIBNode *pNode = dynamic_cast<CIBNode*>(fetchHostLogicalNode());
	_ASSERTE(pNode);
	std::string PacketSig = pNode->queryIBPacketSignature(vWorkCompletion->wr_id);
	_ASSERTE(!PacketSig.empty());

	CIBPacket *pPacket = dynamic_cast<CIBPacket*>(hiveCommon::hiveCreateProduct(PacketSig));
	_ASSERTE(pPacket);
	pPacket->setData(vMemoryBlock->getAddr(), vWorkCompletion->length);
	pPacket->extractSourceIP(vWorkCompletion);
	pPacket->processPacketV(const_cast<hiveNetworkCommon::CLogicalNode*>(getHostLogicalNode()));
	delete pPacket;
}

//*********************************************************************************
//FUNCTION:
CIBMemoryBlock* CIBWCEHandlingThread::__fetchMemoryBlockOfWorkCompletion(ib_wc_t *vWorkCompletion, EWorkRequestType vWorkReqType)
{
	_ASSERT(m_pHostCompletionQueue);
	CIBConnection *pConnection = m_pHostCompletionQueue->fetchHostConnection();
	_ASSERT(pConnection);
	return pConnection->findMemoryBlock(vWorkCompletion->wr_id, vWorkReqType);
}

//*********************************************************************************
//FUNCTION:
void CPerformanceTester::tickReceive(unsigned int vSize)
{
	boost::mutex::scoped_lock Lock(m_ReceiveMutex);

	if (m_NumReceiveRequest == 0)
	{
		m_ReceivedMegaBytes = 0;
		m_ReceiveTimer.begin();
		m_NumReceiveRequest++;
	}
	else
	{
		m_ReceivedMegaBytes += vSize / (1024.0 * 1024);
		if (m_NumReceiveRequest == m_NumTargetRequest)
		{
			double ElapseTime = m_ReceiveTimer.end();
			double Speed = m_ReceivedMegaBytes / ElapseTime;
			std::cout << _BOOST_STR3("The speed of receiving IB data is [%1% / %2% = %3%]MB/s.", m_ReceivedMegaBytes, ElapseTime, Speed) << std::endl;  //NOTE: 这里强行将结果输出到屏幕而不是日志系统，一是因为一般日志只会输出到文件而不会到屏幕，二是日志系统有缓存，可能不会及时输出
			m_NumReceiveRequest = 0;
		}
		else
			m_NumReceiveRequest++;
	}
}

//*********************************************************************************
//FUNCTION:
void CPerformanceTester::tickSend(unsigned int vSize)
{
	boost::mutex::scoped_lock Lock(m_SendMutex);

	if (m_NumSendRequest == 0)
	{
		m_SentMegaBytes = 0;
		m_SendTimer.begin();
		m_NumSendRequest++;
	}
	else
	{
		m_SentMegaBytes += vSize / (1024.0 * 1024);
		if (m_NumSendRequest == m_NumTargetRequest)
		{
			double ElapseTime = m_SendTimer.end();
			double Speed = m_SentMegaBytes / ElapseTime;
			std::cout << _BOOST_STR3("The speed of sending IB data is [%1% / %2% = %3%]MB/s.", m_SentMegaBytes, ElapseTime, Speed) << std::endl;
			m_NumSendRequest = 0;
		}
		else
			m_NumSendRequest++;
	}
}