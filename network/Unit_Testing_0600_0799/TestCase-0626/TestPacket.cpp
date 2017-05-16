#include "TestPacket.h"
#include "common/ProductFactory.h"
#include "TestReplyPacket.h"
#include "TopologyCommon.h"

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
	std::cout << "Data Received Succeed!\n";

	CTestReplyPacket* pPacket = dynamic_cast<CTestReplyPacket*>(hiveCommon::hiveCreateProduct("TESTREPLYPACKET_SIG"));
	pPacket->setPacketTargetIP("127.0.0.1");
	dynamic_cast<CTestReplyPacket*>(pPacket)->setMessage("This is test reply packet!");
	hiveCSNetworkTopology::sendPacket(pPacket, hiveCSNetworkTopology::TO_CLIENT);
}

hiveNetworkCommon::IUserPacket* CTestPacket::clonePacketV()
{
	return new CTestPacket(this);
}