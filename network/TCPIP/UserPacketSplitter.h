#pragma once
#include <vector>
#include "TCPIPCommon.h"

namespace hiveNetworkCommon
{
	class IUserPacket;
}

namespace hiveTCPIP
{
	class CUserPacketSplitter
	{
	public:
		CUserPacketSplitter();
		~CUserPacketSplitter();

		void splitUserPacket(const hiveNetworkCommon::IUserPacket *vUserPacket, std::vector<SRawDataBuffer>& voOutput) const;
		void setMaxIPPacketSize(unsigned int vMaxSize) {_ASSERTE(m_MaxIPPacketSize > 0); m_MaxIPPacketSize = vMaxSize;}

	private:
		unsigned int m_MaxIPPacketSize;

		SRawDataBuffer __createIPPacketAndFillHeader(unsigned int vUserPacketID, unsigned int vUserPacketSize, unsigned int vNumTotalIPPacket, unsigned int vIPPacketID, unsigned int vIPPacketSize) const;
	};
}