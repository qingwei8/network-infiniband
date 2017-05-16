#pragma once

namespace hiveTCPIP
{
	class CIPPacketHeader
	{
	public:
		CIPPacketHeader(void);
		CIPPacketHeader(unsigned int vUserPacketID, unsigned int vUserPacketSize, unsigned int vNumTotalIPPacket, unsigned int vIPPacketID, unsigned int vIPPacketSize);
		~CIPPacketHeader(void);

		static unsigned int getIPPacketHeaderSize() {return 5 * sizeof(unsigned int);}

		void dump2Buffer(char *voIPPacketBuffer) const;
		void extractFromBuffer(const char *vIPPacketBuffer);
		bool isValid() const;
		bool isSoleIPPacket() const;

		unsigned int getIPPacketSize() const	 {return m_IPPacketSize;}
		unsigned int getIPPacketID() const       {return m_IPPacketID;}
		unsigned int getTotalNumIPPacket() const {return m_TotalNumIPPacket;}
		unsigned int getUserPacketID() const     {return m_UserPacketID;}
		unsigned int getUserPacketSize() const   {return m_UserPacketSize;}
		unsigned int getUserDataSizeInCurrentIPPacket() const     {return m_IPPacketSize - getIPPacketHeaderSize() - 1;}

		unsigned int computeOffsetInUserPacket() const;

	private:
		unsigned int m_IPPacketSize;
		unsigned int m_UserPacketID;
		unsigned int m_UserPacketSize;
		unsigned int m_TotalNumIPPacket;
		unsigned int m_IPPacketID;
	};

	extern unsigned int g_MaxUserDataInIPPacket;
}