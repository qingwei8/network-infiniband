#pragma once
#include "Packet.h"
#include "iba/ib_types.h"
#include "InfinibandExport.h"

namespace hiveInfiniband
{
	class INFINIBAND_DLL_EXPORT CIBPacket : public hiveNetworkCommon::IUserPacket
	{
	public:
		CIBPacket();
		CIBPacket(const std::string& vPacketSig);
		virtual ~CIBPacket();

		const unsigned char* getData() const { _ASSERTE(m_pData); return m_pData; }

		unsigned int getDataSize() const { return m_DataSize; }

		void setData(unsigned char *vData, unsigned int vDataSize, bool vIsResponsible2ReleaseData=false) { _ASSERTE(!m_pData && vDataSize && vData); m_pData = vData; m_DataSize = vDataSize; m_IsResponsible2ReleaseData = vIsResponsible2ReleaseData; }
		void extractSourceIP(const ib_wc_t* vWorkCompletion);

		virtual bool isReady2SendV() const override;

	private:
		unsigned char *m_pData;
		unsigned int   m_DataSize;
		bool           m_IsResponsible2ReleaseData;
	};
}

