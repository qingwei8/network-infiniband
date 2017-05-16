#pragma once
#include <string>
#include "../common/NetworkCommon.h"
#include "CSNetworkTopologyExport.h"

namespace hiveNetworkCommon
{
	class IUserPacket;
	class CNetworkNodeConfig;
}

namespace hiveNetworkTopology
{
	enum   ESendToType;
	struct SMasterInfo;
	class  CConnectionUpdateInfo;
}

namespace hiveNetworkTopology
{
	NETWORK_TOPOLOGY_DLL_EXPORT bool hiveInitNetwork(const std::string& vNetWorkConfigFile, const std::string& vNetWorkType, const std::string& vTopologyConfigFile = "");
	NETWORK_TOPOLOGY_DLL_EXPORT bool hiveInitNetwork(const hiveNetworkCommon::CNetworkNodeConfig* vConfig, const std::string& vNetWorkTypeconst, const std::string& vTopologyConfigFile = "");
	NETWORK_TOPOLOGY_DLL_EXPORT bool sendPacket(hiveNetworkCommon::IUserPacket *vPacket, ESendToType vSendToType, bool vDeletePacketAfterSentHint = true);
	NETWORK_TOPOLOGY_DLL_EXPORT bool sendPacket(hiveNetworkCommon::IUserPacket *vPacket, ESendToType vSendToType, const char* vNetworkNodeName, bool vDeletePacketAfterSentHint = true);
	NETWORK_TOPOLOGY_DLL_EXPORT bool sendPacket(hiveNetworkCommon::IUserPacket *vPacket, ESendToType vSendToType, const std::string& vNetworkNodeName, bool vDeletePacketAfterSentHint = true);
	NETWORK_TOPOLOGY_DLL_EXPORT bool sendFile(const std::string& vFilePath, ESendToType vSendToType, const std::string& vTargetIP, unsigned int vTargetPort, const std::string& vSendType = "TCP", hiveNetworkCommon::ESendMode vSendMode = hiveNetworkCommon::UNICAST);
	NETWORK_TOPOLOGY_DLL_EXPORT void stopNetwork();
	NETWORK_TOPOLOGY_DLL_EXPORT void queryConnectionUpdate(CConnectionUpdateInfo& voUpdatedConnectionInfo);
	NETWORK_TOPOLOGY_DLL_EXPORT void queryMasterInfo(SMasterInfo& voMasterInfo);
}
