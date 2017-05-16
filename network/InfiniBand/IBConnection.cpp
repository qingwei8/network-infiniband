#include "IBConnection.h"
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "common/ProductFactory.h"
#include "IBQueuePair.h"
#include "IBNode.h"
#include "BuildIBConnectionReqPacket.h"
#include "NetworkCommon.h"
#include "IBChannelAdapter.h"
#include "IBMemoryBlock.h"
#include "PrepareDataReqPacket.h"
#include "NetworkInterface.h"
#include "TCPIPInterface.h"
#include "IBConfig.h"

using namespace hiveInfiniband;

hiveCommon::CProductFactory<CIBConnection> theCreator("IB_CONNECTION");

CIBConnection::CIBConnection(): m_ConnectionType(IB_UNKNOWN), m_RemoteLID(0), m_pChannelAdatper(nullptr), m_CompletionQueueHandlerThreadSig(""), m_RecvQPIndex(0), m_SendQPIndex(0)
{

}

CIBConnection::CIBConnection(const CIBChannelAdapter* vAdapter) : m_ConnectionType(IB_UNKNOWN), m_RemoteLID(0), m_CompletionQueueHandlerThreadSig(""), m_RecvQPIndex(0), m_SendQPIndex(0)
{
	_ASSERT(vAdapter);
    m_pChannelAdatper = vAdapter;
}

CIBConnection::~CIBConnection(void)
{
}

//*******************************************************************
//FUNCTION:
bool CIBConnection::__buildV(const std::string& vRemoteIP, unsigned short vRemotePort)
{
	_ASSERT((m_ConnectionType != IB_INBOUND) && !vRemoteIP.empty() && isConnecting() && getHostNode());

	if (!hiveNetworkCommon::hiveIsConnectionBuilt(vRemoteIP, "TCP"))
	{
		const hiveConfig::CHiveConfig *pConfig = getHostNode()->getConfig();
		_ASSERTE(pConfig && pConfig->isAttributeExisted(CONFIG_KEYWORD::NUM_TRY_BUILDING_CONN));
		int Counter = 0;
		while (!hiveNetworkCommon::hiveBuildConnection(vRemoteIP, vRemotePort, "TCP") && (Counter < pConfig->getAttribute<int>(CONFIG_KEYWORD::NUM_TRY_BUILDING_CONN)))
		{
			boost::this_thread::sleep(boost::posix_time::milliseconds(50));
			Counter++;
		}
		_HIVE_EARLY_RETURN((Counter == pConfig->getAttribute<int>(CONFIG_KEYWORD::NUM_TRY_BUILDING_CONN)),
			_BOOST_STR1("Fail to build IB Connection because the failure of building auxilary TCP connection to [%1%].", vRemoteIP), false);
	}	

	_initRemoteInfo(vRemoteIP, vRemotePort);
	
	CIBNode* pIBNode = dynamic_cast<CIBNode*>(fetchHostNode());
	_ASSERT(pIBNode);
	pIBNode->registerConnection(this);	
	const CIBChannelAdapter* pChannelAdapter = pIBNode->getChannelAdapter();
	_ASSERT(pChannelAdapter);
	__setChannelAdapter(pChannelAdapter);
	
	const hiveConfig::CHiveConfig* pIBNodeConfig = pIBNode->getConfig();
	_ASSERTE(pIBNodeConfig && pIBNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_CQ_HANDLE_THREAD_SIG) && pIBNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_NUM_QUEUE_PAIR));
	m_CompletionQueueHandlerThreadSig = pIBNodeConfig->getAttribute<std::string>(CONFIG_KEYWORD::IB_CQ_HANDLE_THREAD_SIG);

	
	_HIVE_EARLY_RETURN(!__createQueuePairObjects(pIBNodeConfig->getAttribute<int>(CONFIG_KEYWORD::IB_NUM_QUEUE_PAIR)), "Failed to create queue pair.", false);
	__sendBuildConnectionReqPacket(vRemoteIP, vRemotePort, pChannelAdapter->getLocalID(IB_PORT), getQueuePairIDSet());

	return true;
}

//*******************************************************************
//FUNCTION:
bool CIBConnection::__createQueuePairObjects(unsigned int vNumQueuePair)
{
	_ASSERT(!m_CompletionQueueHandlerThreadSig.empty() && (vNumQueuePair > 0));

	for (unsigned int i=0; i<vNumQueuePair; ++i)
	{
		CIBQueuePair::SQueueAttributes SendAttr, RecvAttr;
		SendAttr.CompletionQueueHandlerThreadSig = m_CompletionQueueHandlerThreadSig;
		RecvAttr.CompletionQueueHandlerThreadSig = m_CompletionQueueHandlerThreadSig;

		CIBQueuePair* pQueuePair = new CIBQueuePair;
		if (!pQueuePair->createQueuePair(m_pChannelAdatper, SendAttr, RecvAttr, true))
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("Fail to create [%1%] queue pairs.", vNumQueuePair));
			for (unsigned int k=0; k<m_QueuePairSet.size(); ++k)
			{
				m_QueuePairSet[k]->close();
				delete m_QueuePairSet[k];
			}
			m_QueuePairSet.clear();
			delete pQueuePair;
			return false;
		}

		pQueuePair->setHostConnection(this);
		m_QueuePairSet.push_back(pQueuePair);
	}

	return true;
}

//*******************************************************************
//FUNCTION:
void CIBConnection::__sendBuildConnectionReqPacket(const std::string& vTargetIP, int vTargetPort, ib_net16_t vRequesterLID, const std::vector<ib_net32_t>& vRequesterQueuePairIDSet)
{
	CIBNode* pIBNode = dynamic_cast<CIBNode*>(fetchHostNode());
	pIBNode->sendBuildConnectionReqPacket(vTargetIP, vTargetPort, vRequesterLID, vRequesterQueuePairIDSet);
	hiveCommon::hiveOutputEvent(_BOOST_STR2("An IB connection request packet has been sent to [%1%:%2%].", vTargetIP, vTargetPort));
}

//*******************************************************************
//FUNCTION:
std::vector<ib_net32_t> CIBConnection::getQueuePairIDSet()
{
	std::vector<ib_net32_t> QueuePairIDSet;
	for (unsigned int i=0; i<m_QueuePairSet.size(); ++i)
		QueuePairIDSet.push_back(m_QueuePairSet[i]->getQueuePairID());
	return QueuePairIDSet;
}

//*******************************************************************
//FUNCTION:
void CIBConnection::setRemoteIBInfo(const std::string& vRemoteIP, ib_net16_t vRemoteLID, const std::vector<ib_net32_t>& vRemoteQPIDSet)
{
	_ASSERT(!vRemoteIP.empty() && !vRemoteQPIDSet.empty() && (vRemoteQPIDSet.size() == m_QueuePairSet.size()));

	setRemoteIP(vRemoteIP);
	m_RemoteLID = vRemoteLID;
	m_RemoteQueuePairIDSets = vRemoteQPIDSet;
}

//*******************************************************************
//FUNCTION:
bool CIBConnection::__makeQueuePairReady()
{
	_ASSERT(!m_QueuePairSet.empty() && (m_RemoteQueuePairIDSets.size() == m_QueuePairSet.size()));
	for (unsigned int i=0; i<m_QueuePairSet.size(); i++)
	{
		m_QueuePairSet[i]->setReomoteIBInfo(m_RemoteLID, m_RemoteQueuePairIDSets[i]);
		if (!m_QueuePairSet[i]->changeState2RTR()) return false;
		if (!m_QueuePairSet[i]->changeState2RTS()) return false;
	}
	return true;
}

//********************************************************************
//FUNCTION:
void CIBConnection::setConnectionType(const EIBConnectionType& vConnType)
{
	_HIVE_SIMPLE_IF_ELSE((m_ConnectionType == IB_UNKNOWN), m_ConnectionType = vConnType, 
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Unknown IB connection type."));
}

//*********************************************************************************
//FUNCTION:
bool CIBConnection::onConnectionReqPacketReceived(CIBNode* vHostNode, const std::string& vRemoteNodeName, const std::string& vRemoteIP, ib_net16_t vRemoteLID, const std::vector<ib_net32_t>& vRemoteQPIDSet, const CIBChannelAdapter* vChannleAdapter)
{
	_ASSERT(vChannleAdapter && vHostNode);

	_setHostNode(vHostNode);
	m_RemoteNodeName = vRemoteNodeName;
	setRemoteIP(vRemoteIP);
	m_RemoteLID = vRemoteLID;
	m_RemoteQueuePairIDSets = vRemoteQPIDSet;
	m_pChannelAdatper = vChannleAdapter;

	const hiveConfig::CHiveConfig* pIBNodeConfig = vHostNode->getConfig();
	_ASSERT(pIBNodeConfig);
	m_CompletionQueueHandlerThreadSig = pIBNodeConfig->getAttribute<std::string>(CONFIG_KEYWORD::IB_CQ_HANDLE_THREAD_SIG);

	_HIVE_EARLY_RETURN(!__createQueuePairObjects(vRemoteQPIDSet.size()), "Fail to create queue pair set.", false);
	_HIVE_EARLY_RETURN(!__makeQueuePairReady(), "Fail to initialize the send/receive queue pair set.", false);

	return true;
}

//*******************************************************************
//FUNCTION:
unsigned int CIBConnection::prepareData2Send(const unsigned char *vBuffer, unsigned int vBufferSize)
{
	_ASSERT(vBuffer && !m_QueuePairSet.empty() && isConnected());

	boost::mutex::scoped_lock(m_Mutex);

	CIBMemoryBlock *pBlock = m_MemoryBlockPool.requestMemoryBlock(vBufferSize, m_SendWorkRequestID, SEND_TYPE);
	_HIVE_EARLY_RETURN(!pBlock, "Fail to send the buffer due to the failure of requesting a memory block.", false);
	_ASSERTE(pBlock->isLocked());
	pBlock->copyBuffer(vBufferSize, vBuffer);

	m_SendWorkRequestID++;
	return pBlock->getWorkRequestID();
}

//*******************************************************************
//FUNCTION:
void CIBConnection::__closeConnectionV()
{
	m_MemoryBlockPool.destroyMemoryPool();
	for (auto& p : m_QueuePairSet)
	{
		p->close();
		delete p;
	}
	m_QueuePairSet.clear();
	hiveCommon::hiveOutputEvent(_BOOST_STR1("The IB connection to [%1%] has been closed and all related resources have been released.", getRemoteIP()));
}

//*******************************************************************
//FUNCTION:
unsigned int CIBConnection::postIBReceiveRequest(unsigned int vRecvBufferSize)
{
	_ASSERT((vRecvBufferSize > 0) && !m_QueuePairSet.empty() && isConnected());

	boost::mutex::scoped_lock(m_Mutex);

	CIBMemoryBlock *pMemoryBlock = m_MemoryBlockPool.requestMemoryBlock(vRecvBufferSize, m_RecvWorkRequestID, RECV_TYPE);
	_HIVE_EARLY_RETURN(!pMemoryBlock, _BOOST_STR1("Fail to request data [%1%] due to the failure of requesting a memory block.", vRecvBufferSize), UINT_MAX);
	_ASSERTE((pMemoryBlock->getWorkReqType() == RECV_TYPE) && pMemoryBlock->isLocked() && (pMemoryBlock->getWorkRequestID() < UINT_MAX));

	ib_local_ds_t DataSegmemt;
	DataSegmemt.lkey = pMemoryBlock->getLocalKey();
	DataSegmemt.vaddr = (uintn_t)pMemoryBlock->getAddr();
	DataSegmemt.length = pMemoryBlock->getCapacity();

	ib_recv_wr_t RecvWorkRequest;
	RecvWorkRequest.p_next = nullptr;
	RecvWorkRequest.wr_id  = m_RecvWorkRequestID;
	RecvWorkRequest.num_ds = 1;  //?
	RecvWorkRequest.ds_array = &DataSegmemt;

	CIBQueuePair *pQueuePair = m_QueuePairSet[m_RecvQPIndex];
	_HIVE_CALL_IB_FUNC(ib_post_recv(pQueuePair->getQueuePairHandle(), &RecvWorkRequest, nullptr), "ib_post_recv", false);

#ifdef _DEBUG
	hiveCommon::hiveOutputEvent(_BOOST_STR1("Data is ready for receiving [%1%].", m_RecvWorkRequestID));
#endif

	m_RecvWorkRequestID++;
	m_RecvQPIndex = (m_RecvQPIndex + 1) % m_QueuePairSet.size();

	return RecvWorkRequest.wr_id;
}

//*******************************************************************
//FUNCTION:
bool CIBConnection::postIBSendRequest(unsigned int vSendRequestID)
{
	_ASSERTE(isConnected() && (vSendRequestID != UINT_MAX));

	CIBMemoryBlock *pMemoryBlock = m_MemoryBlockPool.findMemoryBlock(vSendRequestID, SEND_TYPE);
	_HIVE_EARLY_RETURN(!pMemoryBlock, _BOOST_STR1("Fail to find memory block [%1%] !", vSendRequestID), false);
	_ASSERTE(pMemoryBlock->isLocked() && (pMemoryBlock->getWorkReqType() == SEND_TYPE));

	ib_local_ds_t ib_local_ds;
	ib_local_ds.lkey   = pMemoryBlock->getLocalKey();
	ib_local_ds.vaddr  = (uintn_t)pMemoryBlock->getAddr();
	ib_local_ds.length = pMemoryBlock->getUsedBufferSize();

	ib_send_wr_t SendWorkRequest;
	SendWorkRequest.p_next   = nullptr;
	SendWorkRequest.wr_id    = vSendRequestID;
	SendWorkRequest.wr_type  = WR_SEND;
	SendWorkRequest.send_opt = 0;
	SendWorkRequest.num_ds   = 1;
	SendWorkRequest.ds_array = &ib_local_ds;
	_HIVE_CALL_IB_FUNC(ib_post_send(m_QueuePairSet[m_SendQPIndex]->getQueuePairHandle(), &SendWorkRequest, nullptr), "ib_post_send()", false);

#ifdef _DEBUG
	hiveCommon::hiveOutputEvent(_BOOST_STR1("Data is ready for sending [%1%].", vSendRequestID));
#endif

	m_SendQPIndex = (m_SendQPIndex + 1) % m_QueuePairSet.size();

	return true;
}
//*********************************************************************************
//FUNCTION:
void CIBConnection::__onConnectedV()
{
	_ASSERT(!m_QueuePairSet.empty() && getHostNode());

	m_SendWorkRequestID = 0;
	m_RecvWorkRequestID = 0;

	const hiveConfig::CHiveConfig *pNodeConfig = getHostNode()->getConfig();
	_ASSERTE(pNodeConfig && pNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_MEMORY_BLOCK_SIZE) && pNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_NUM_MEMORY_BLOCK));

	m_MemoryBlockPool.initMemoryPool(m_pChannelAdatper->getProtectionDomain(), pNodeConfig->getAttribute<int>(CONFIG_KEYWORD::IB_NUM_MEMORY_BLOCK), 
		pNodeConfig->getAttribute<int>(CONFIG_KEYWORD::IB_MEMORY_BLOCK_SIZE));

	for (unsigned int i=0; i<m_QueuePairSet.size(); i++) m_QueuePairSet[i]->startCompletionQueueMonitorThread();

	hiveCommon::hiveOutputEvent(_BOOST_STR1("An IB connection to [%1%] has been built successfully.", getRemoteIP()));
}

//*********************************************************************************
//FUNCTION:
CIBMemoryBlock* CIBConnection::findMemoryBlock(unsigned int vWorkRequestID, const EWorkRequestType& vWorkReqType)
{
	return m_MemoryBlockPool.findMemoryBlock(vWorkRequestID, vWorkReqType);
}