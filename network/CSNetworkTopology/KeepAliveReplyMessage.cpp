#include "KeepAliveReplyMessage.h"
#include "common\ProductFactory.h"
#include "common\HiveCommon.h"

using namespace hiveNetworkTopology;

hiveCommon::CProductFactory<CKeepAliveReplyMessage> theCreator("MESSAGE_KEEPALIVE_REPLY");

CKeepAliveReplyMessage::CKeepAliveReplyMessage()
{ 
	m_MessageType = KEEP_ALIVE_REPLY_MESSAGE;
}

//**************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CKeepAliveReplyMessage::clonePacketV()
{
	return new CKeepAliveReplyMessage(*this);
}