#include "MyIBPacket.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CMyIBPacket> PacketCreator("MyIBPacket");

CMyIBPacket::CMyIBPacket()
{
}

CMyIBPacket::~CMyIBPacket()
{
}

//*******************************************************************
//FUNCTION:
void CMyIBPacket::processPacketV(hiveNetworkCommon::CLogicalNode* vHostNode)
{
	std::cout << "The product signature: " << getProductCreationSig() << std::endl;
}