#include "TestPacket1.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/ProductFactory.h"
#include "common/CommonInterface.h"

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

//****************************************************************************
//FUNCTION:
void CTestPacket1::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	std::string MD5 = hiveCommon::hiveComputeMD5(m_Message);

	std::string Output = _BOOST_STR1("The MD5 of received packet [%1%] is ", m_Message.size());
	if (m_MD5 == MD5)
		std::cout << Output << "correct." << std::endl;
	else
		std::cout << "IMPORTANT!!!    " << Output << "wrong." << std::endl;
}