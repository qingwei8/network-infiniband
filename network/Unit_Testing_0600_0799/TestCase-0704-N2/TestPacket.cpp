#include "TestPacket.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/HiveCommonMicro.h"
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
void CTestPacket::processV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	std::string MD5 = hiveCommon::hiveComputeMD5(m_Message);

	std::string Output = _BOOST_STR1("The MD5 of received packet [%1%] is ", m_Message.size());
	if (m_MD5 == MD5)
		std::cout << Output << "correct." << std::endl;
	else
		std::cout << Output << "wrong." << std::endl;
}