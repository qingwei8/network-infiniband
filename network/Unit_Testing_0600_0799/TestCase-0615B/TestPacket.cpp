#include "TestPacket.h"
#include "common/ProductFactory.h"
#include "NetworkInterface.h"

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

	hiveNetworkCommon::IUserPacket* pRespondPacket = new CTestPacket;
	pRespondPacket->setProductCreationSig("TESTPACKET_SIG");
	pRespondPacket->setPacketTargetIP(getPacketSourceIP());
	pRespondPacket->setPacketTargetPort(8000);

	hiveNetworkCommon::buildConnection(getPacketSourceIP(), 8000);

	std::string Message = getPacketSourceIP() + " has received the UNICAST packet successfully\n";

	if (getSendMode() == hiveNetworkCommon::ESendMode::BROADCAST)
		Message = "has received this Board_Cast packet successfully\n";

	dynamic_cast<CTestPacket*>(pRespondPacket)->setMessage(Message);
	hiveNetworkCommon::sendPacket(pRespondPacket);
}

hiveNetworkCommon::IUserPacket* CTestPacket::clonePacketV()
{
	return new CTestPacket(this);
}