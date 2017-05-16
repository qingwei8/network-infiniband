#include "IPPacketHeader.h"
#include <crtdbg.h>
#include "common/HiveConfig.h"
#include "NetworkInterface.h"

using namespace hiveTCPIP;

CIPPacketHeader::CIPPacketHeader() : m_IPPacketID(0), m_IPPacketSize(0), m_TotalNumIPPacket(0), m_UserPacketID(0), m_UserPacketSize(0)
{
}

CIPPacketHeader::CIPPacketHeader(unsigned int vUserPacketID, unsigned int vUserPacketSize, unsigned int vNumTotalIPPacket, unsigned int vIPPacketID, unsigned int vIPPacketSize)
	: m_UserPacketID(vUserPacketID), m_UserPacketSize(vUserPacketSize), m_TotalNumIPPacket(vNumTotalIPPacket), m_IPPacketID(vIPPacketID), m_IPPacketSize(vIPPacketSize)
{
	_ASSERTE(isValid());
}

CIPPacketHeader::~CIPPacketHeader(void)
{
}

//**********************************************************************************
//FUNCTION:
unsigned int CIPPacketHeader::computeOffsetInUserPacket() const
{
	return (m_IPPacketID == 0) ? 0 : m_IPPacketID * g_MaxUserDataInIPPacket;
}

//******************************************************************
//FUNCTION:
bool CIPPacketHeader::isValid() const
{
	if (m_TotalNumIPPacket == 0) return false;
	if (m_IPPacketID >= m_TotalNumIPPacket) return false;
	return true;
}

//******************************************************************
//FUNCTION:
bool CIPPacketHeader::isSoleIPPacket() const 
{
	return ((m_IPPacketID == 0) && (m_TotalNumIPPacket == 1));
}

//***********************************************************
//FUNCTION:
void CIPPacketHeader::dump2Buffer(char *voIPPacketBuffer) const
{
	_ASSERTE(isValid());

	unsigned int *pBuffer = (unsigned int*) voIPPacketBuffer;
	pBuffer[0] = m_IPPacketSize;
	pBuffer[1] = m_UserPacketID;
	pBuffer[2] = m_UserPacketSize;   //NOTE: 发送m_UserPacketSize的大小是为了在UDP协议下（UDP不保证接受顺序），接收到任何一个IP packet都可以直接为user packet分配内存空间
	pBuffer[3] = m_TotalNumIPPacket;
	pBuffer[4] = m_IPPacketID;
}

//***********************************************************
//FUNCTION:
void CIPPacketHeader::extractFromBuffer(const char *vIPPacketBuffer)
{
	unsigned int *pBuffer = (unsigned int*) vIPPacketBuffer;

	m_IPPacketSize     = pBuffer[0];
	m_UserPacketID     = pBuffer[1];
	m_UserPacketSize   = pBuffer[2];
	m_TotalNumIPPacket = pBuffer[3];
	m_IPPacketID       = pBuffer[4];

	_ASSERTE(isValid());
}