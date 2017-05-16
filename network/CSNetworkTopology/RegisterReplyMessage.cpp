#include "RegisterReplyMessage.h"
#include "common\ProductFactory.h"
#include "common\HiveCommon.h"

using namespace hiveNetworkTopology;

hiveCommon::CProductFactory<CRegisterReplyMessage> theCreator("MESSAGE_REGISTER_REPLY");

CRegisterReplyMessage::CRegisterReplyMessage() : m_IsNeedBuildTCPConnection(true)
{ 
	m_MessageType = REGISTER_REPLY_MESSAGE;
}

//**************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CRegisterReplyMessage::clonePacketV()
{
	return new CRegisterReplyMessage(*this);
}