#pragma once
#include <string>
#include "common/Singleton.h"
#include "common/HiveConfig.h"

namespace hiveNetworkCommon
{
	class CNetworkNodeConfig;
}

namespace hiveNetworkTopology
{
	class CMasterInfoConfig : public hiveCommon::CSingleton<CMasterInfoConfig> , public hiveConfig::CHiveConfig
	{
	public:
		CMasterInfoConfig();
		virtual ~CMasterInfoConfig(){};

		const std::string& getMasterIP() const               {return m_MasterIP;}
		const unsigned int getMasterTCPListeningPort() const {return m_MasterTCPListeningPort;}
		const unsigned int getMasterUDPListeningPort() const {return m_MasterUDPListeningPort;}
		const unsigned int getLocalTCPListeningPort() const  {return m_LocalTCPListeningPort;}
		const unsigned int getLocalUDPListeningPort() const  {return m_LocalUDPListeningPort;}

		void initMasterInfo();
		void getInfoFromPhysicalNodeConfig(const hiveNetworkCommon::CNetworkNodeConfig* vConfig);

	private:
		std::string  m_MasterIP;
		unsigned int m_MasterTCPListeningPort;
		unsigned int m_MasterUDPListeningPort;
		unsigned int m_LocalTCPListeningPort;
		unsigned int m_LocalUDPListeningPort;
		
		friend class hiveCommon::CSingleton<CMasterInfoConfig>;
	};
}