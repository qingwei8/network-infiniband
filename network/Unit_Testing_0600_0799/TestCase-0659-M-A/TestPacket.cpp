#include "TestPacket.h"
#include "common/ProductFactory.h"
#include <fstream>

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void) : m_pData(NULL)
{
}

CTestPacket::CTestPacket(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig), m_pData(NULL)
{
}

CTestPacket::~CTestPacket(void)
{
}
 
//****************************************************************************
//FUNCTION:
void CTestPacket::processV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	if (m_pData) std::cout << "succeed !" << std::endl;

	std::ofstream FileOut("data.txt");
	boost::archive::text_oarchive OA(FileOut);
	OA << this;
	FileOut.close();
}