#include "TestPacket.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/ProductFactory.h"
#include "common/CommonInterface.h"

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void)
{
}

CTestPacket::CTestPacket(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig)
{
}

CTestPacket::CTestPacket(const CTestPacket* vOther) : hiveNetworkCommon::IUserPacket(vOther)
{
	m_Message = vOther->m_Message;
}

CTestPacket::~CTestPacket(void)
{
}

//****************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CTestPacket::clonePacketV()
{
	return new CTestPacket(this);
}

//****************************************************************************
//FUNCTION:
void CTestPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	std::string MD5 = hiveCommon::hiveComputeMD5(m_Message);

	std::string Output = _BOOST_STR1("The MD5 of received packet [%1%] is ", m_Message.size());
	if (m_MD5 == MD5)
		std::cout << Output << "correct." << std::endl;
	else
		std::cout << "IMPORTANT!!!    " << Output << "wrong." << std::endl;
}