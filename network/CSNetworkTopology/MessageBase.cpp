#include "MessageBase.h"
#include "CSNetworkTopologyMessagePool.h"

hiveNetworkTopology::IMessage::IMessage(EMessageType vMessageType)
{
	m_MessageType = vMessageType;
}

hiveNetworkTopology::IMessage::IMessage(const IMessage* vMessage) : hiveNetworkCommon::IUserPacket(vMessage)
{
	m_MessageType = vMessage->getMessageType();
}

//**************************************************************************
//FUNCTION:
void hiveNetworkTopology::IMessage::processV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	CCSNTMessagePool::getInstance()->pushMessage(clonePacketV());
}

//**************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* hiveNetworkTopology::IMessage::clonePacketV()
{
	return new IMessage(*this);
}