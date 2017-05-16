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
