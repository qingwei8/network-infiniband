#pragma once
#include <string>

namespace hiveNetworkTopology
{
	const unsigned int MAX_UDP_SEND_SIZE              = 65535;

	const unsigned int CLIENT_UDPLISTENING_PORT       = 6001;
	const unsigned int CLIENT_TCPLISTENING_PORT       = 7001;

	const unsigned int REGISTER_INTERVAL_TIME         = 3000;
	const unsigned int KEEPALIVE_INTERVAL_TIME        = 5000;
	const unsigned int MANAGECLIENT_INTERVAL_TIME     = 1000;
	const unsigned int CONFIRMCONNECTED_INTERVAL_TIME = 1000;

	const double LASTCONNECTED_CURRENT_THRESHOLD_INTERVAL_TIME = 0.004 * KEEPALIVE_INTERVAL_TIME;

	struct SClientInfo
	{
		std::string  ClientIP;
		unsigned int ClientTCPListeningPort;
		unsigned int ClientUDPListeningPort;
		time_t       LastConnectTime;
		bool         TCPConnectionFlag;
		bool         UDPConnectionFlag;
		bool         KeepAliveOpenFlag;
		
		SClientInfo() : ClientIP(""), ClientTCPListeningPort(0), ClientUDPListeningPort(0), 
			LastConnectTime(0), TCPConnectionFlag(false), UDPConnectionFlag(false), KeepAliveOpenFlag(true){}
	};

	enum ENodeType
	{
		CLIENT_NODE,
		SERVER_NODE
	};

	enum ESendToType
	{
		TO_CLIENT,
		TO_MASTER
	};

	struct SConnectionInfo
	{
		std::string  ConnectionIP;
		unsigned int ConnectionTCPPort;
		unsigned int ConnectionUDPPort;
		time_t		 ConnectionUpdateTime;

		SConnectionInfo() : ConnectionIP(""), ConnectionTCPPort(0), ConnectionUDPPort(0), ConnectionUpdateTime(0) {}

		SConnectionInfo(const SConnectionInfo& vConnectInfo)
		{
			ConnectionIP         = vConnectInfo.ConnectionIP;
			ConnectionTCPPort    = vConnectInfo.ConnectionTCPPort;
			ConnectionUDPPort    = vConnectInfo.ConnectionUDPPort;
			ConnectionUpdateTime = vConnectInfo.ConnectionUpdateTime;
		}

		SConnectionInfo(const std::string& vConnectionIP, unsigned int vConnectionTCPPort, unsigned int vConnectionUDPPort, time_t vCurrentTime)
		{
			ConnectionIP         = vConnectionIP;
			ConnectionTCPPort    = vConnectionTCPPort;
			ConnectionUDPPort    = vConnectionUDPPort;
			ConnectionUpdateTime = vCurrentTime;
		}
	};

	struct SMasterInfo
	{
		std::string  MasterIP;
		unsigned int MasterUDPListeningPort;
		unsigned int MasterTCPListeningPort;
		unsigned int MasterIBListeningPort;
	};
}