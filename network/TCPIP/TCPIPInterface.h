#pragma once
#include <string>
#include "TCPIPExport.h"

namespace hiveTCPIP
{
	TCPIP_DLL_EXPORT bool hiveInitTCPIP(const std::string& vConfigFile);

	unsigned int  _hiveIP2Integer(const std::string& vIP);
	unsigned int  _hiveGenerateUserPacketID();
	unsigned char _hiveInterpretSocketUsage(const std::string& vUsage);

	void _hiveGetLocalIP();
}