#include "TestPacket.h"
#include "common/ProductFactory.h"

hiveCommon::CProductFactory<CTestPacket> theCreator("TESTPACKET_SIG");

int CTestPacket::m_SuccessfulReceivedPacketsNum = 0;

CTestPacket::CTestPacket(void) : m_BufferSize(0),  m_AllPacketsNum(0)
{
}

CTestPacket::CTestPacket(const CTestPacket* vOther) : hiveNetworkCommon::IUserPacket(vOther)
{
	m_Buffer = vOther->m_Buffer;
	m_BufferSize = vOther->m_BufferSize;
	m_AllPacketsNum = m_AllPacketsNum;
}

CTestPacket::~CTestPacket(void)
{
}

//*********************************************************************************
//FUNCTION:
void CTestPacket::processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	if (m_Buffer.length() == m_BufferSize)
	{
		for (int i=0; i<m_BufferSize; ++i)
		{
			if (m_Buffer[i] != i%26 + 'a')
				return;
		}

		m_SuccessfulReceivedPacketsNum++;
	}

	if (m_SuccessfulReceivedPacketsNum == m_AllPacketsNum)
	{
		m_SuccessfulReceivedPacketsNum = 0;
		std::cout << "TCP Send Data Success, Buffer Size : " << m_BufferSize + 148 << std::endl;
	}
}

hiveNetworkCommon::IUserPacket* CTestPacket::clonePacketV()
{
	return new CTestPacket(this);
}

void CTestPacket::creatBuffer(int vBufferSize)
{
	m_BufferSize = vBufferSize;
	m_Buffer.resize(vBufferSize);
	for (int i=0; i<vBufferSize; ++i)
		m_Buffer[i] = i%26 + 'a';
}
