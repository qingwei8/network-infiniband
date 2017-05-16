#pragma once
#include "TCPIPConfig.h"
#include "InfinibandExport.h"

namespace hiveInfiniband
{
	class INFINIBAND_DLL_EXPORT CIBConfig : public hiveTCPIP::CTCPIPConfig
	{
	public:
		CIBConfig();
		~CIBConfig();

	private:
		bool __findAuxNodeConfigByName(const std::string& vAuxNodeName) const;
		virtual bool __otherVerifyOnNodeV(const hiveConfig::CHiveConfig *vNodeConfig) const override;
		virtual void __extraOnLoadedV(hiveConfig::CHiveConfig* vNodeConfig) override;
	};
}