#include "TestPacket.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void)
{
}

CTestPacket::CTestPacket(const CTestPacket* vOther) : hiveNetworkCommon::IUserPacket(vOther)
{
	m_Message = vOther->m_Message;
}

CTestPacket::~CTestPacket(void)
{
}

//*********************************************************************************
//FUNCTION:
void CTestPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	std::cout << getPacketTargetIP() << std::endl;
	std::cout << getPacketTargetPort() << std::endl;
	std::cout << m_Message << std::endl;
}

hiveNetworkCommon::IUserPacket* CTestPacket::clonePacketV()
{
	return new CTestPacket(this);
}