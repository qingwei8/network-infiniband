#include "UserPacketAssembler.h"
#include "common/CommonInterface.h"
#include "Packet.h"
#include "TCPIPCommon.h"

using namespace hiveTCPIP;

CUserPacketAssembler::CUserPacketAssembler(void)
{
}

CUserPacketAssembler::~CUserPacketAssembler(void)
{
	for (auto e=m_AssemblingUserPacketSet.begin(); e!=m_AssemblingUserPacketSet.end(); e++)
		e->second.first.release();
}

//***********************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CUserPacketAssembler::addIPPacket(const SRawDataBuffer& vIPPacket)
{
	_ASSERTE(vIPPacket.isValidBuffer());

	if ((vIPPacket.getBufferSize() <= CIPPacketHeader::getIPPacketHeaderSize()) || (vIPPacket.getContentAt(vIPPacket.getBufferSize()-1) != END_OF_PACKET))
	{
#ifdef _DEBUG
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "A packet is discarded because of incompletion.");
#endif
		return nullptr;
	}

	CIPPacketHeader IPPacketHeader;
	IPPacketHeader.extractFromBuffer(vIPPacket.fetchBufferAddress());
	if (IPPacketHeader.isSoleIPPacket())
	{
		SRawDataBuffer t;
		t.incompleteShallowCopy(IPPacketHeader.getUserPacketSize(), vIPPacket, IPPacketHeader.getIPPacketHeaderSize());
		return __createUserPacket(t);
	}
	else
	{
		return __assembleUserPacket(vIPPacket.fetchBufferAddress(), IPPacketHeader);
	}
}

//***********************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CUserPacketAssembler::__assembleUserPacket(const char *vIPPacketBuffer, const CIPPacketHeader& vIPPacketHeader)
{
	_ASSERTE(vIPPacketBuffer && (vIPPacketHeader.getTotalNumIPPacket() > 1));

	std::pair<unsigned int, unsigned int> UserPacketSig = std::make_pair(vIPPacketHeader.getUserPacketID(), vIPPacketHeader.getUserPacketSize());
	auto itr = m_AssemblingUserPacketSet.find(UserPacketSig);
	if ((itr == m_AssemblingUserPacketSet.end()) || m_AssemblingUserPacketSet.empty())
	{
		__onReceiveFirstIPPacketOfUserPacket(vIPPacketBuffer, vIPPacketHeader, UserPacketSig);
		return nullptr;
	}
	else
	{
		__copyUserDataFromIPPacket2UserPacketBuffer(vIPPacketBuffer, vIPPacketHeader, itr->second.first.fetchBufferAddress());
		itr->second.second++;
		if (itr->second.second < vIPPacketHeader.getTotalNumIPPacket()) return nullptr;
		hiveNetworkCommon::IUserPacket *pAssembledPacket = __createUserPacket(itr->second.first);
		itr->second.first.release();
		m_AssemblingUserPacketSet.erase(itr);
		return pAssembledPacket;
	}
}

//******************************************************************
//FUNCTION:
void CUserPacketAssembler::__onReceiveFirstIPPacketOfUserPacket(const char *vIPPacketBuffer, const CIPPacketHeader& vIPPacketHeader, const std::pair<unsigned int, unsigned int>& vUserPacketSig)
{
	_ASSERTE(vIPPacketBuffer && (vIPPacketHeader.getTotalNumIPPacket() > 1) && (m_AssemblingUserPacketSet.count(vUserPacketSig) == 0));

	SRawDataBuffer UserPacketRawData(vIPPacketHeader.getUserPacketSize());
	__copyUserDataFromIPPacket2UserPacketBuffer(vIPPacketBuffer, vIPPacketHeader, UserPacketRawData.fetchBufferAddress());

	m_AssemblingUserPacketSet[vUserPacketSig] = std::make_pair(UserPacketRawData, 1);
}

//******************************************************************
//FUNCTION:
void CUserPacketAssembler::__copyUserDataFromIPPacket2UserPacketBuffer(const char *vIPPacketBuffer, const CIPPacketHeader& vIPPacketHeader, char *voUserPacketBuffer)
{
	unsigned int Offset = vIPPacketHeader.computeOffsetInUserPacket();
	_ASSERTE(Offset < vIPPacketHeader.getUserPacketSize());
	memcpy(voUserPacketBuffer+Offset, vIPPacketBuffer+CIPPacketHeader::getIPPacketHeaderSize(), vIPPacketHeader.getUserDataSizeInCurrentIPPacket());
}

//******************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CUserPacketAssembler::__createUserPacket(const SRawDataBuffer& vRawData) const
{
	std::stringstream SerializedStream;
	SerializedStream.write(vRawData.fetchBufferAddress(), vRawData.getBufferSize());
	return dynamic_cast<hiveNetworkCommon::IUserPacket*>(hiveCommon::hiveDeserializeProduct(hiveCommon::MODE_BINARY, SerializedStream));
}