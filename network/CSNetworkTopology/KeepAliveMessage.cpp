#include "KeepAliveMessage.h"
#include "common\ProductFactory.h"
#include "common\HiveCommon.h"

using namespace hiveNetworkTopology;

hiveCommon::CProductFactory<CKeepAliveMessage> theCreator("MESSAGE_KEEPALIVE");

CKeepAliveMessage::CKeepAliveMessage()
{ 
	m_MessageType = KEEP_ALIVE_MESSAGE;
}

//**************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CKeepAliveMessage::clonePacketV()
{
	return new CKeepAliveMessage(*this);
}