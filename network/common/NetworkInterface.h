#pragma once
#include <string>
#include <boost/any.hpp>
#include "NetworkExport.h"
#include "NetworkCommon.h"

namespace hiveConfig
{
	class CHiveConfig;
}

namespace hiveNetworkCommon
{
	class IUserPacket;
	class CLogicalNode;

	NETWORK_DLL_EXPORT bool hiveInitNetwork(const hiveConfig::CHiveConfig *vConfig);
	NETWORK_DLL_EXPORT bool hiveBuildConnection(const std::string& vRemoteIP, unsigned short vRemotePort, const std::string& vProtocal = "TCP");
	NETWORK_DLL_EXPORT bool hiveIsConnectionBuilt(const std::string& vIP, const std::string& vProtocal);
	NETWORK_DLL_EXPORT void hiveSendPacket(const IUserPacket *vUserPacket, const std::string& vNetworkNodeName, const SSendOptions& vSendOption);
	NETWORK_DLL_EXPORT void hiveStopNetwork();
	NETWORK_DLL_EXPORT void hiveStartLogicalNode(const std::string& vNodeName);
	NETWORK_DLL_EXPORT void hiveUpdateLogicalNodeConfig(const std::string& vNodeName, const std::string& vAttributePath, boost::any vAttributeValue);

	NETWORK_DLL_EXPORT const std::string& hiveGetLocalIP();

	NETWORK_DLL_EXPORT const hiveConfig::CHiveConfig* hiveGetNetworkConfig();

	NETWORK_DLL_EXPORT CLogicalNode* hiveFindLogicalNodeByProtocal(const std::string& vProtocal);

	NETWORK_DLL_EXPORT bool hiveIsAdapterAvailable(const std::string& vAdapterName);

	extern std::string g_LocalIP;
}