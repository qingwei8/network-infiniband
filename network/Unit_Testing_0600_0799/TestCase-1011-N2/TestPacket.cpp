#include "TestPacket.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/ProductFactory.h"
#include "common/CommonInterface.h"
#include <numeric>

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
		std::cout << "IMPORTANT!!!    " << Output << "wrong." << std::endl;

	std::cout << m_PacketNum << " packets" << std::endl;

	calRate();
}

void CTestPacket::calRate()
{
	std::cout << "the time of start receiving packet: " << m_StartTime << std::endl;
	
	clock_t EndTime = clock();
	std::cout <<"the time of end receiving packet: " << EndTime << std::endl;

	double TimesOfReceived = (double)((EndTime - m_StartTime)- (m_PacketNum-1)*100) / CLOCKS_PER_SEC;
	std::cout << "duration time: " << TimesOfReceived << std::endl;

	double DateSize = (m_TotalPacketOfPast * 8)/(1000 * 1000);
	std::cout << "Data size: " << DateSize << "Mbit." << std::endl;   

	double Rate = DateSize / TimesOfReceived;
	std::cout << "the rate of receiving packet: " << Rate << "Mbps." << std::endl;

	std::cout << "***********************************************" << std::endl;
}
