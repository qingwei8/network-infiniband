#include "TestPacket1.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestPacket1> theCreator("TESTPACKET1_SIG");

CTestPacket1::CTestPacket1(void)
{
}

CTestPacket1::CTestPacket1(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig)
{
}

CTestPacket1::~CTestPacket1(void)
{
}