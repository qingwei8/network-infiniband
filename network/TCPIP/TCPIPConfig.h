#pragma once
#include "common/HiveConfig.h"
#include "TCPIPExport.h"

namespace hiveTCPIP
{
	class TCPIP_DLL_EXPORT CTCPIPConfig : public hiveConfig::CHiveConfig
	{
	public:
		CTCPIPConfig();
		virtual ~CTCPIPConfig();

	protected:
		virtual void _onLoadedV() override;
		virtual bool _verifyConfigV() const override;

	private:
		bool __isValidNodeConfiguration(const hiveConfig::CHiveConfig *vNodeConfig) const;
		bool __isValidSocketConfiguration(const hiveConfig::CHiveConfig *vSocketConfig) const;
		void __defineAcceptableAttributeSet();

		virtual bool __otherVerifyOnNodeV(const hiveConfig::CHiveConfig *vNodeConfig) const {return true;}
		virtual void __extraOnLoadedV(hiveConfig::CHiveConfig* vNodeConfig) {}
	};

	extern unsigned int g_MaxUserDataInIPPacket;
}