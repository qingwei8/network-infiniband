#include "TestPacket.h"
#include "common/ProductFactory.h"


hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void): m_pImageData(nullptr), m_ImageDataSize(0)
{
}

CTestPacket::CTestPacket(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig), m_pImageData(nullptr), m_ImageDataSize(0)
{
}

CTestPacket::~CTestPacket(void)
{
	_SAFE_DELETE_ARRAY(m_pImageData);
}

//***********************************************************
//FUNCTION:
void CTestPacket::setImageData(const std::pair<unsigned char*, unsigned int>& vImageData)
{
	m_pImageData = vImageData.first;
	m_ImageDataSize = vImageData.second;
}
