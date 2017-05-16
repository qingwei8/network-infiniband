#include "UserPacketSplitter.h"
#include "common/CommonInterface.h"
#include "Packet.h"
#include "TCPIPInterface.h"
#include "TCPIPCommon.h"
#include "IPPacketHeader.h"

using namespace hiveTCPIP;

CUserPacketSplitter::CUserPacketSplitter() : m_MaxIPPacketSize(8192)
{
}

CUserPacketSplitter::~CUserPacketSplitter()
{
}

//******************************************************************
//FUNCTION:
void CUserPacketSplitter::splitUserPacket(const hiveNetworkCommon::IUserPacket *vUserPacket, std::vector<SRawDataBuffer>& voOutput) const
{
	_ASSERTE(voOutput.empty() && vUserPacket);

	std::stringstream SerializedStream;
	hiveCommon::hiveSerializeProduct(const_cast<hiveNetworkCommon::IUserPacket*>(vUserPacket), hiveCommon::MODE_BINARY, true, SerializedStream);
	_ASSERTE(!SerializedStream.str().empty());

	int UserPacketSize = SerializedStream.str().length();
	
	unsigned int ActualMaxBytesInIPPacket = m_MaxIPPacketSize - CIPPacketHeader::getIPPacketHeaderSize() - 1;  //NOTES: ��������һ��1����Ϊ���ڰ�β��һ�������ַ�END_OF_PACKET��ע��END_OF_PACKETֻ��һ���ֽڣ�
	                                                                                                           //       ������������TCPЭ���������ж�һ�����Ƿ������END_OF_PACKET�������ֻ�����ڵ�����;�����ÿ�����ܵ��İ��Ƿ��Ը��ַ�����
	unsigned int NumIPPacket = UserPacketSize / ActualMaxBytesInIPPacket;
	if (UserPacketSize % ActualMaxBytesInIPPacket != 0) NumIPPacket++;
	unsigned int IPPacketID = 0;
	unsigned int UserPacketID = _hiveGenerateUserPacketID();  

	for (unsigned int i=0; i<NumIPPacket-1; i++)
	{
		SRawDataBuffer IPPacket = __createIPPacketAndFillHeader(UserPacketID, UserPacketSize, NumIPPacket, i, m_MaxIPPacketSize);
		SerializedStream.read(IPPacket.fetchBufferAddress() + CIPPacketHeader::getIPPacketHeaderSize(), ActualMaxBytesInIPPacket);
		IPPacket.fetchBufferAddress()[m_MaxIPPacketSize-1] = END_OF_PACKET;
		voOutput.push_back(IPPacket);

		IPPacketID++;
	}

	unsigned int UserPacketRemainedSize = UserPacketSize - ActualMaxBytesInIPPacket * (NumIPPacket - 1);
	unsigned int LastIPPacketSize = UserPacketRemainedSize + CIPPacketHeader::getIPPacketHeaderSize() + 1;
	SRawDataBuffer IPPacket = __createIPPacketAndFillHeader(UserPacketID, UserPacketSize, NumIPPacket, NumIPPacket-1, LastIPPacketSize);
	SerializedStream.read(IPPacket.fetchBufferAddress() + CIPPacketHeader::getIPPacketHeaderSize(), UserPacketRemainedSize);
	IPPacket.fetchBufferAddress()[LastIPPacketSize-1] = END_OF_PACKET;
	voOutput.push_back(IPPacket);
}

//***********************************************************
//FUNCTION:
SRawDataBuffer CUserPacketSplitter::__createIPPacketAndFillHeader(unsigned int vUserPacketID, unsigned int vUserPacketSize, unsigned int vNumTotalIPPacket, unsigned int vIPPacketID, unsigned int vIPPacketSize) const
{
	SRawDataBuffer IPPacket(vIPPacketSize);
	CIPPacketHeader Header(vUserPacketID, vUserPacketSize, vNumTotalIPPacket, vIPPacketID, vIPPacketSize);
	Header.dump2Buffer(IPPacket.fetchBufferAddress());
	
	return IPPacket;
}