#include "BuildIBConnectionReqPacket.h"
#include <boost\format.hpp>
#include "common\ProductFactory.h"
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "IBNode.h"
#include "LogicalNode.h"
#include "PhysicalNode.h"

using namespace hiveInfiniband;

hiveCommon::CProductFactory<CBuildCIBConnectionReqPacket> theCreator(DEFAULT_IB_CONNECTION_REQ_PACKET);

CBuildCIBConnectionReqPacket::CBuildCIBConnectionReqPacket()
{
}

CBuildCIBConnectionReqPacket::~CBuildCIBConnectionReqPacket()
{
}

CBuildCIBConnectionReqPacket::CBuildCIBConnectionReqPacket(const CBuildCIBConnectionReqPacket *vPacket) : hiveNetworkCommon::IUserPacket(vPacket)
{
	m_RequesterLID		    = vPacket->m_RequesterLID;
	m_RequesterNodeName	    = vPacket->m_RequesterNodeName;
	m_RequesterQueuePairSet = vPacket->m_RequesterQueuePairSet;
}

//*********************************************************************************
//FUNCTION:
void CBuildCIBConnectionReqPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	_ASSERTE(vHostNode);
	hiveCommon::hiveOutputEvent(_BOOST_STR1("An IB connection request is received from [%1%].", getPacketSourceIP()));

	CIBNode* pIBNode = dynamic_cast<CIBNode*>(vHostNode->getHostPhysicalNode()->findLogicalNodeByName("IB"));
	_ASSERT(pIBNode);

	if (!pIBNode->isConnectionReqPacketReceived(getPacketSourceIP(), m_RequesterNodeName))
	{
		pIBNode->__onConnectionReqPacketReceived(m_RequesterLID, m_RequesterQueuePairSet, getPacketSourceIP(), m_RequesterNodeName);
	}
	else
	{
		_HIVE_UNIMPLEMENTED_FUNC
	}
}

//*********************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CBuildCIBConnectionReqPacket::clonePacketV()
{
	return new CBuildCIBConnectionReqPacket(this);
}