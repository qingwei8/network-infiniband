#pragma once
#include "NetworkCommon.h"

namespace hiveTCPIP
{
	const unsigned int TCP_KEEP_ALIVE_TIME              = 5000;
	const unsigned int TCP_KEEP_ALIVE_INTERVAL_TIME     = 1000;
	const unsigned int CONNECTION_MONITOR_INTERVAL_TIME = 10000;
	const unsigned int AUTO_RECONNECT_INTERVAL_TIME     = 50000;

	enum EDisconnectionType
	{
		NORMAL = 0,
		ACCIDENT,
	};

	enum EConnectionType
	{
		INCOMING,
		OUTGOING,
		UNDEFINED,
	};
}