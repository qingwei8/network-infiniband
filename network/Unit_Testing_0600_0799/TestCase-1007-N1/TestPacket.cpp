#include "TestPacket.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void) : m_ReqLID(1), m_ReqNodeName("QP1")
{
}

CTestPacket::CTestPacket(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig), m_ReqLID(1), m_ReqNodeName("QP1")
{
}

CTestPacket::~CTestPacket(void)
{
}