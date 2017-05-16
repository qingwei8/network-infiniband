#include "TestPacket.h"
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

CTestPacket::CTestPacket(void) : m_pImageData(nullptr), m_ImageDataSize(0)
{
}

CTestPacket::CTestPacket(const std::string& vSig) : hiveNetworkCommon::IUserPacket(vSig), m_pImageData(nullptr), m_ImageDataSize(0)
{
}

CTestPacket::~CTestPacket(void)
{
	_SAFE_DELETE_ARRAY(m_pImageData);
}

//****************************************************************************
//FUNCTION:
void CTestPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	static time_t m_FirstTime = std::clock();
	static int i = 1;
	m_TotalTime = std::clock() - m_FirstTime;
	if ((m_TotalTime / CLOCKS_PER_SEC) >= 1.0)
	{
		std::cout << "Per second receive image " << i << std::endl;
		Sleep(5000);
	}

	std::cout << i << " ImageData: " << static_cast<const void*>(m_pImageData) << " ImageDataSize: " << m_ImageDataSize << std::endl;
	i++;
	//SOIL_save_image("1.bmp", SOIL_SAVE_TYPE_BMP, 1024, 768, 3, m_pImageData);
}