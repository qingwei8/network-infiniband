#pragma once
#include <map>
#include "IPPacketHeader.h"
#include "TCPIPCommon.h"

namespace hiveNetworkCommon
{
	class IUserPacket;
}

namespace hiveTCPIP
{
	class CUserPacketAssembler
	{
	public:
		CUserPacketAssembler(void);
		~CUserPacketAssembler(void);

		hiveNetworkCommon::IUserPacket* addIPPacket(const SRawDataBuffer& vIPPacket);

	private:
		std::map<std::pair<unsigned int, unsigned int>, std::pair<SRawDataBuffer, unsigned int>> m_AssemblingUserPacketSet;

		hiveNetworkCommon::IUserPacket* __assembleUserPacket(const char *vIPPacketBuffer, const CIPPacketHeader& vIPPacketHeader);
		hiveNetworkCommon::IUserPacket* __createUserPacket(const SRawDataBuffer& vRawData) const;

		void __onReceiveFirstIPPacketOfUserPacket(const char *vIPPacketBuffer, const CIPPacketHeader& vIPPacketHeader, const std::pair<unsigned int, unsigned int>& vUserPacketSig);
		void __copyUserDataFromIPPacket2UserPacketBuffer(const char *vIPPacketBuffer, const CIPPacketHeader& vIPPacketHeader, char *voUserPacketBuffer);
	};
}