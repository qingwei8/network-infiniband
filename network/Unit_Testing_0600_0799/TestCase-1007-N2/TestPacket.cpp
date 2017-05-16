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

void CTestPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode){
	std::cout << "ReqLID: " << m_ReqLID << std::endl;
	std::cout << "ReqNodeName: " << m_ReqNodeName << std::endl;
	std::cout << "ReqQueuePairIDSet: " << std::endl;
	for(auto reqQueuePairID : m_ReqQueuePairIDSet){
		std::cout << "\t" << reqQueuePairID << std::endl;		
	}
}