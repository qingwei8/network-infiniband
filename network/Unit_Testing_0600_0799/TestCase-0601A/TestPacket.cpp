#include "TestPacket.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void)
{
}

CTestPacket::CTestPacket(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig)
{
}

CTestPacket::~CTestPacket(void)
{
}

//****************************************************************************
//FUNCTION:
void CTestPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	std::cout << "Message: " << m_Message << std::endl;
	std::cout << "  Double: " << m_Double << std::endl;
	std::cout << "  Int: " << m_Int << std::endl;
	//_ASSERT(m_Char == -3);
}
