#pragma once
#include <string>
#include "common/HiveCommonMicro.h"

namespace hiveNetworkCommon
{
	const std::string DEFAULT_BROADCAST_IP = "255.255.255.255";
	const std::string DUMMY_IP = "0.0.0.0";
	const int		  DUMMY_PORT = 0;

	enum ESendMode
	{
		UNICAST = 0,
		BROADCAST,
		MULTICAST,
	};

	struct SSendOptions
	{
		unsigned short SourcePort;
		unsigned short TargetPort;  
		std::string    TargetIP;
		ESendMode      SendMode;
		bool DeletePacketAfterSendingHint;

		SSendOptions() : SendMode(UNICAST), DeletePacketAfterSendingHint(true), SourcePort(DUMMY_PORT), TargetPort(DUMMY_PORT) {}
	
		bool isValid() const
		{
			if ((TargetIP.empty()) || (TargetIP == DUMMY_IP)) return false;
			if ((TargetIP == DEFAULT_BROADCAST_IP) && (SendMode != BROADCAST)) return false;
			return true;
		}
	};

	namespace CONFIG_KEYWORD
	{
		const std::string NODE = "NODE";
		const std::string IP   = "IP";
		const std::string PROTOCAL = "PROTOCAL";
		const std::string OBJECT_CREATION_SIG   = "OBJECT_CREATION_SIG";
		const std::string START_NODE_AFTER_CREATION = "START_NODE_AFTER_CREATION";
	}
}