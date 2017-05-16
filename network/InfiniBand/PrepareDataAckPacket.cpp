#include "PrepareDataAckPacket.h"
#include <boost/format.hpp>
#include "common/ProductFactory.h"
#include "common/HiveCommonMicro.h"
#include "common/CommonInterface.h"
#include "IBConnection.h"
#include "PhysicalNode.h"
#include "IBNode.h"

using namespace hiveInfiniband;

hiveCommon::CProductFactory<CPrepareDataAckPacket> TheCreator(DEFAULT_IB_CQ_RECEIVING_THREAD_SIG);

CPrepareDataAckPacket::CPrepareDataAckPacket() : m_SendRequestID(UINT_MAX)
{
}

CPrepareDataAckPacket::CPrepareDataAckPacket(const CPrepareDataAckPacket* vPacket) : hiveNetworkCommon::IUserPacket(vPacket)
{
	m_SendRequestID = vPacket->getSendRequestID();
}

CPrepareDataAckPacket::~CPrepareDataAckPacket()
{
}

//*******************************************************************
//FUNCTION:
void CPrepareDataAckPacket::processPacketV(hiveNetworkCommon::CLogicalNode* vHostNode)
{
	_ASSERTE(vHostNode && (m_SendRequestID != UINT_MAX));

#ifdef _DEBUG
	hiveCommon::hiveOutputEvent(_BOOST_STR1("A prepare data acknowledge packet has been received from [%1%]!", getPacketSourceIP()));
#endif

	CIBNode* pIBNode = dynamic_cast<CIBNode*>(vHostNode->getHostPhysicalNode()->findLogicalNodeByName("IB"));
	_ASSERT(pIBNode);
	CIBConnection *pConn = pIBNode->findConnection(getPacketSourceIP());
	_ASSERTE(pConn);

#ifdef _DEBUG
	if (!pConn->postIBSendRequest(m_SendRequestID)) hiveCommon::hiveOutputEvent(_BOOST_STR1("Fail to send IB packet to [%1%].", getPacketSourceIP()));
#else
	pConn->postIBSendRequest(m_SendRequestID);
#endif
}