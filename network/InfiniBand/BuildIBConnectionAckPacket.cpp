#include "BuildIBConnectionAckPacket.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "common\ProductFactory.h"
#include "IBNode.h"
#include "PhysicalNode.h"
#include "IBConnection.h"

using namespace hiveInfiniband;

hiveCommon::CProductFactory<CBuildCIBConnectionAckPacket> theCreator(DEFAULT_IB_CONNECTION_ACK_PACKET);

CBuildCIBConnectionAckPacket::CBuildCIBConnectionAckPacket()
{
}

CBuildCIBConnectionAckPacket::CBuildCIBConnectionAckPacket(const CBuildCIBConnectionAckPacket *vPacket) : hiveNetworkCommon::IUserPacket(vPacket)
{
	m_AckLID			= vPacket->m_AckLID;
	m_AckNodeName		= vPacket->m_AckNodeName;
	m_AckQueuePairIDSet = vPacket->m_AckQueuePairIDSet;
}

CBuildCIBConnectionAckPacket::~CBuildCIBConnectionAckPacket()
{
}

//*********************************************************************************
//FUNCTION:
void CBuildCIBConnectionAckPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	hiveCommon::hiveOutputEvent(_BOOST_STR1("An IB connection acknowledge packet has been received from [%1%].", getPacketSourceIP()));

	const hiveNetworkCommon::CPhysicalNode* pPhysicNode = vHostNode->getHostPhysicalNode();
	CIBNode* pIBNode = dynamic_cast<CIBNode*>(pPhysicNode->findLogicalNodeByName("IB"));
	_ASSERT(pIBNode);

	CIBConnection *pConn = pIBNode->findConnection(getPacketSourceIP());
	_HIVE_EARLY_EXIT(!pConn, _BOOST_STR2("Cannot find the outbound connection according to the returned information [%1%, %2%].", getPacketSourceIP(), m_AckNodeName));

	pConn->setRemoteIBInfo(getPacketSourceIP(), m_AckLID, m_AckQueuePairIDSet);
	_HIVE_EARLY_EXIT(!pConn->__makeQueuePairReady(), "Fail to make queue pair ready.");
	pConn->_setState(hiveNetworkCommon::INetworkConnection::STATE_CONNECTED);
	pConn->setConnectionType(IB_OUTBOUND);
}

//*********************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CBuildCIBConnectionAckPacket::clonePacketV()
{
	return new CBuildCIBConnectionAckPacket(this);
}