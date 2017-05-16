#include "MessageTestPacket.h"
#include "common\ProductFactory.h"
#include "common\CommonInterface.h"

hiveCommon::CProductFactory<CMessageTestPacket> theCreator("MessageTestPacket");

//*********************************************************************************
//FUNCTION:
CMessageTestPacket::CMessageTestPacket(void)
{
}

//*********************************************************************************
//FUNCTION:
CMessageTestPacket::~CMessageTestPacket(void)
{
}

//*********************************************************************************
//FUNCTION:
CMessageTestPacket::CMessageTestPacket(const CMessageTestPacket *vPacket) : hiveNetworkCommon::IUserPacket(vPacket)
{

}

//*********************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CMessageTestPacket::clonePacketV()
{
	return new CMessageTestPacket(this);
}

//*********************************************************************************
//FUNCTION:
void CMessageTestPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	hiveCommon::hiveOutputEvent(("Receive a IB Message packet."), hiveCommon::WARNING_LEVEL);
}