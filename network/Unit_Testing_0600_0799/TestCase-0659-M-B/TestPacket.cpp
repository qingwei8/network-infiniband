#include "TestPacket.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void) : m_pData(nullptr)
{
}

CTestPacket::CTestPacket(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig), m_pData(nullptr)
{
}

CTestPacket::~CTestPacket(void)
{
}

//***********************************************************
//FUNCTION::
void CTestPacket::setData()
{
	m_pData = new unsigned char[1024*768*4];
	for (unsigned int i=0; i<1024*768*4; i++)
	{
		m_pData[i] = 255;
	}
}
