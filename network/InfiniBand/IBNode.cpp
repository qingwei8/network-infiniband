#include "IBNode.h"
#include <boost/format.hpp>
#include "common/HiveCommonMicro.h"
#include "common/CommonInterface.h"
#include "common/ProductFactory.h"
#include "PhysicalNode.h"
#include "IBConnection.h"
#include "BuildIBConnectionReqPacket.h"
#include "BuildIBConnectionAckPacket.h"
#include "NetworkCommon.h"
#include "PrepareDataReqPacket.h"
#include "IBPacket.h"

using namespace hiveInfiniband;

hiveCommon::CProductFactory<CIBNode> theCreator(DEFAULT_IB_NODE_SIG);

CIBNode::CIBNode(void) : m_pChannelAdapter(nullptr), m_pAuxNode(nullptr)
{
}

CIBNode::~CIBNode(void)
{
	_closeAllConnections();

	//NTOE: 注意这里并没有处理m_pAuxNode，因为从配置文件来看，m_pAuxNode实际是一个独立的节点，它除了为IB服务外，可能还有其他用途，所以这里并不在停止IB节点时，去停止m_pAuxNode
	m_pChannelAdapter->closeAdapter();
	delete m_pChannelAdapter;
}

//*******************************************************************
//FUNCTION:
bool CIBNode::__startAuxNode(const std::string& vAuxNodeName)
{
	_ASSERT(getHostPhysicalNode());
	m_pAuxNode = dynamic_cast<hiveNetworkCommon::CLogicalNode*>(getHostPhysicalNode()->findLogicalNodeByName(vAuxNodeName));
	_HIVE_EARLY_RETURN(!m_pAuxNode, "Fail to start IB adapter because the auxiliary IP-based node cannot be found.", false);
	m_pAuxNode->start();
	hiveCommon::hiveOutputEvent("Succeed to start the auxiliary node.");
	return true;
}

//******************************************************************* 
//FUNCTION:
bool CIBNode::__extraInitV()
{
	_ASSERTE(!m_pChannelAdapter);

	const hiveConfig::CHiveConfig* pIBNodeConfig = getConfig();
	_ASSERT(pIBNodeConfig && !m_pChannelAdapter);
	m_pChannelAdapter = new CIBChannelAdapter();
	m_pChannelAdapter->setHostNode(this);

	_HIVE_EARLY_RETURN(!__startAuxNode(pIBNodeConfig->getAttribute<std::string>(CONFIG_KEYWORD::IB_AUX_NODE)), "Fail to initialize the IB node due to the failure of starting the auxiliary node", false);
	_HIVE_EARLY_RETURN(!m_pChannelAdapter->openAdapter(), "Fail to initialize the IB node due to the failure of staring channel adapter.", false);
	return true;
}

//*******************************************************************
//FUNCTION:
bool CIBNode::isConnectionReqPacketReceived(const std::string& vRemoteIP, const std::string& vRemoteNodeName) const
{
	for (auto Connection : m_ConnectionSet)
	{
		if ((Connection->getRemoteIP() == vRemoteIP) && (dynamic_cast<CIBConnection*>(Connection)->getRemoteNodeName() == vRemoteNodeName))
			return true;
	}
	return false;
}

//*******************************************************************
//FUNCTION:
void CIBNode::sendBuildConnectionReqPacket(const std::string& vTargetIP, int vTargetPort, ib_net16_t vRequesterLID, const std::vector<ib_net32_t>& vRequesterQueuePairIDSet)
{
	CBuildCIBConnectionReqPacket* pReqPacket = dynamic_cast<CBuildCIBConnectionReqPacket*>(hiveCommon::hiveCreateProduct(DEFAULT_IB_CONNECTION_REQ_PACKET));
	_ASSERTE(m_pAuxNode && pReqPacket);

	pReqPacket->setRequesterInfo(vRequesterLID, getName(), vRequesterQueuePairIDSet);
	hiveNetworkCommon::SSendOptions SendOption;
	SendOption.TargetIP   = vTargetIP;
	SendOption.TargetPort = vTargetPort;
	m_pAuxNode->sendPacket(pReqPacket, SendOption);
}

//*******************************************************************
//FUNCTION:
void CIBNode::__sendBuildConnectionAckPacket(const std::string& vTargetIP)
{
	_ASSERT(m_pAuxNode);
	CIBConnection* pConn = findConnection(vTargetIP);
	_ASSERT(pConn);
	CBuildCIBConnectionAckPacket *pAckPacket = dynamic_cast<CBuildCIBConnectionAckPacket*>(hiveCommon::hiveCreateProduct(DEFAULT_IB_CONNECTION_ACK_PACKET));
	_HIVE_EARLY_EXIT(!pAckPacket, _BOOST_STR1("Fail to generate the IB connection requirement packet due to bad packet signature [%1%].", DEFAULT_IB_CONNECTION_ACK_PACKET));
	pAckPacket->setAckInfo(m_pChannelAdapter->getLocalID(IB_PORT), pConn->getQueuePairIDSet(), getName());

	hiveNetworkCommon::SSendOptions SendOption;
	SendOption.TargetIP = vTargetIP;
	m_pAuxNode->sendPacket(pAckPacket, SendOption);

	pConn->_setState(hiveNetworkCommon::INetworkConnection::STATE_CONNECTED);

	hiveCommon::hiveOutputEvent(_BOOST_STR1("A connection acknowledge packet has been sent back to [%1%].", vTargetIP));
}

//*******************************************************************
//FUNCTION:
bool CIBNode::__onConnectionReqPacketReceived(ib_net16_t vRequesterLID, const std::vector<ib_net32_t>& vRequesterQPIDSet, const std::string& vRequesterIP, const std::string& vRequesterNodeName)
{
	_ASSERT(!vRequesterQPIDSet.empty() && !vRequesterIP.empty() && !vRequesterNodeName.empty());

	CIBConnection* pConn = dynamic_cast<CIBConnection*>(hiveCommon::hiveCreateProduct("IB_CONNECTION"));
	if (!pConn->onConnectionReqPacketReceived(this, vRequesterNodeName, vRequesterIP, vRequesterLID, vRequesterQPIDSet, m_pChannelAdapter)) return false;
	
	registerConnection(pConn);
	__sendBuildConnectionAckPacket(vRequesterIP);

	return true;
}

//********************************************************************
//FUNCTION:
//NOTE: 这里实际并没有发送IB数据包。根据IB协议，发送IB数据需要经过如下几个步骤：1.发送端通过辅助节点发送数据接收请求包到远端；2.远端接收到请求包后返回
//      一个ACK包；3.发送端在接收到ACK包后才真正通过IB发送数据。而本函数实际只是完成第一个步骤，而发送IB数据在CPrepareDataAckPacket::processPacketV()中完成
void CIBNode::__sendPacketV(const hiveNetworkCommon::IUserPacket *vPacket, const hiveNetworkCommon::SSendOptions& vSendOption)
{
	_ASSERTE(m_pAuxNode && !vSendOption.TargetIP.empty());

	const CIBPacket *pIBPacket = dynamic_cast<const CIBPacket*>(vPacket);
	_ASSERTE(pIBPacket);

	CIBConnection *pConn = findConnection(vSendOption.TargetIP);
	_HIVE_EARLY_EXIT(!pConn, _BOOST_STR1("Fail to find the IB connection with [%1%]", vSendOption.TargetIP));

	std::pair<const unsigned char*, unsigned int> IBData = __convertPacket2IBData(pIBPacket);
	_HIVE_EARLY_EXIT((IBData.second == 0), _BOOST_STR1("Fail to convert packet to IB data for [%1%].", vSendOption.TargetIP));

	unsigned int SendRequestID = pConn->prepareData2Send(IBData.first, IBData.second);
	if (SendRequestID < UINT_MAX)
	{
		CPrepareDataReqPacket *pPrepareDataReqPacket = dynamic_cast<CPrepareDataReqPacket*>(hiveCommon::hiveCreateProduct(DEFAULT_IB_CQ_SENDING_THREAD_SIG));
		_ASSERT(pPrepareDataReqPacket);
		pPrepareDataReqPacket->setIBPacketSig(pIBPacket->getProductCreationSig().empty() ? DEFAULT_IB_PACKET_SIG : pIBPacket->getProductCreationSig());
		pPrepareDataReqPacket->setBufferSize(IBData.second);
		pPrepareDataReqPacket->setSendRequestID(SendRequestID);

		m_pAuxNode->sendPacket(pPrepareDataReqPacket, vSendOption);
	}
}

//*******************************************************************
//FUNCTION:
std::pair<const unsigned char*, unsigned int> CIBNode::__convertPacket2IBData(const CIBPacket *vPacket) const
{
	_ASSERT(vPacket);
	return std::make_pair(vPacket->getData(), vPacket->getDataSize());
}

//*******************************************************************
//FUNCTION:
CIBConnection* CIBNode::findConnection(const std::string& vTargetIP)
{
	boost::mutex::scoped_lock Lock(m_Mutex4Connection);
	for (auto& Connection : m_ConnectionSet)
	{
		if (Connection->getRemoteIP() == vTargetIP)
			return dynamic_cast<CIBConnection*>(Connection);
	}
	return nullptr;
}

//*********************************************************************************
//FUNCTION:
std::string CIBNode::queryRemoteIP(ib_net16_t vRemoteLID) const
{
	for (unsigned int i = 0; i < m_ConnectionSet.size(); i++)
	{
		CIBConnection *pConn = dynamic_cast<CIBConnection*>(m_ConnectionSet[i]);
		if (pConn)
		{
			if (pConn->getRemoteLID() == vRemoteLID) return pConn->getRemoteIP();
		}
	}
	return hiveNetworkCommon::DUMMY_IP;
}

//*********************************************************************************
//FUNCTION:
std::string CIBNode::queryIBPacketSignature(unsigned int vWorkRequestID)
{
	std::string t;
	for (auto i = m_ReceiveRequest2PacketSigMap.begin(); i != m_ReceiveRequest2PacketSigMap.end(); i++)
	{
		if (i->first == vWorkRequestID)
		{
			t = i->second;
			m_ReceiveRequest2PacketSigMap.erase(i);
			break;
		}
	}
	return t;
}