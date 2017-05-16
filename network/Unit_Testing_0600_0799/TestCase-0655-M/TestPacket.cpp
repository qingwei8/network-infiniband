#include "TestPacket.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void) : m_Double(123.435), m_Int(120321), m_Char(-3)
{
}

CTestPacket::CTestPacket(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig), m_Double(123.435), m_Int(120321), m_Char(-3)
{
}

CTestPacket::~CTestPacket(void)
{
}