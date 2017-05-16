#include "RegisterMessage.h"
#include "common\ProductFactory.h"
#include "common\HiveCommon.h"

using namespace hiveNetworkTopology;

hiveCommon::CProductFactory<CRegisterMessage> theCreator("MESSAGE_REGISTER");

CRegisterMessage::CRegisterMessage() : m_KeepAliveOpenFlag(true)
{ 
	m_MessageType = REGISTER_MESSAGE;
}

//**************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CRegisterMessage::clonePacketV()
{
	return new CRegisterMessage(*this);
}