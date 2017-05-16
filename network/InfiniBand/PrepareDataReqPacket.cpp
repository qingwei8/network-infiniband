#include "PrepareDataReqPacket.h"
#include <boost/format.hpp>
#include "common/ProductFactory.h"
#include "common/HiveCommonMicro.h"
#include "common/CommonInterface.h"
#include "IBConnection.h"
#include "PhysicalNode.h"
#include "IBNode.h"
#include "PrepareDataAckPacket.h"

using namespace hiveInfiniband;

hiveCommon::CProductFactory<CPrepareDataReqPacket> TheCreator(DEFAULT_IB_CQ_SENDING_THREAD_SIG);

CPrepareDataReqPacket::CPrepareDataReqPacket() : m_BufferSize(0), m_SendRequestID(UINT_MAX)
{
}

CPrepareDataReqPacket::CPrepareDataReqPacket(const CPrepareDataReqPacket* vPacket) : hiveNetworkCommon::IUserPacket(vPacket)
{
	m_BufferSize	= vPacket->getBufferSize();
	m_SendRequestID = vPacket->getSendRequestID();
}

CPrepareDataReqPacket::~CPrepareDataReqPacket()
{
}

//*********************************************************************************
//FUNCTION:
void CPrepareDataReqPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
//NOTE: 这里的vHostNode是TCP节点，因为这个包是TCP负责接收
	CIBNode *pIBNode =  dynamic_cast<CIBNode*>(hiveNetworkCommon::CPhysicalNode::getInstance()->findLogicalNodeByName("IB"));
	_ASSERT(pIBNode && (m_SendRequestID != UINT_MAX));

	CIBConnection* pConnection = pIBNode->findConnection(getPacketSourceIP());
	_HIVE_EARLY_EXIT(!pConnection, _BOOST_STR1("A prepare data request packet from [%1%] is ignored because no IB connection has been built with it.", getPacketSourceIP()));
	
	unsigned int WorkRequestID = pConnection->postIBReceiveRequest(m_BufferSize);
	_HIVE_EARLY_EXIT((WorkRequestID == UINT_MAX), _BOOST_STR1("Fail to post IB receive request for [%1%].", getPacketSourceIP()));
	_ASSERTE(!m_IBPacketSig.empty());
	pIBNode->registerIBPacketSignature(WorkRequestID, m_IBPacketSig);

	CPrepareDataAckPacket *pAckPacket = dynamic_cast<CPrepareDataAckPacket*>(hiveCommon::hiveCreateProduct(DEFAULT_IB_CQ_RECEIVING_THREAD_SIG));
	hiveNetworkCommon::CLogicalNode *pAuxNode = pIBNode->fetchAuxNode();
	_ASSERTE(pAckPacket && pAuxNode);
	pAckPacket->setSendRequestID(m_SendRequestID);

	hiveNetworkCommon::SSendOptions SendOption;
	SendOption.TargetIP = getPacketSourceIP();
	pAuxNode->sendPacket(pAckPacket, SendOption);
}