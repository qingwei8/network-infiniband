#include "TestReplyPacket.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestReplyPacket> theCreator("TESTREPLYPACKET_SIG");

CTestReplyPacket::CTestReplyPacket(void)
{
}

CTestReplyPacket::CTestReplyPacket(const CTestReplyPacket* vOther) : hiveNetworkCommon::IUserPacket(vOther)
{
	m_Message = vOther->m_Message;
}

CTestReplyPacket::~CTestReplyPacket(void)
{
}

//*********************************************************************************
//FUNCTION:
void CTestReplyPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	std::cout << getPacketTargetIP() << std::endl;
	std::cout << getPacketTargetPort() << std::endl;
	std::cout << m_Message << std::endl;
	std::cout << "Data Received Succeed!\n";
}

hiveNetworkCommon::IUserPacket* CTestReplyPacket::clonePacketV()
{
	return new CTestReplyPacket(this);
}