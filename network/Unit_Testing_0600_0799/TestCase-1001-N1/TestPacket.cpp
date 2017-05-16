#include "TestPacket.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void) : m_Double(123.435), m_Int(120321), m_Char(-5)
{
}

CTestPacket::CTestPacket(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig), m_Double(123.435), m_Int(120321), m_Char(-5)
{
}

CTestPacket::~CTestPacket(void)
{
}

//****************************************************************************
//FUNCTION:
void CTestPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	std::cout << m_Data.m_Int << " " << m_Data.m_Double << " " << m_Data.m_Char << " " << m_Data.m_String << std::endl;
	std::cout << "Message: " << m_Message << std::endl;
	std::cout << "  Double: " << m_Double << std::endl;
	std::cout << "  Int: " << m_Int << std::endl;
	_ASSERT(m_Char == -3);
}