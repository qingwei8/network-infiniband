#include "TCPIPInterface.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/HiveCommonMicro.h"
#include "common/ConfigInterface.h"
#include "NetworkInterface.h"
#include "TCPIPConfig.h"
#include "TCPIPCommon.h"
#include "TCPNode.h"

using namespace hiveTCPIP;

//**************************************************************************************
//FUNCTION:
bool hiveTCPIP::hiveInitTCPIP(const std::string& vConfigFile)
{
	CTCPIPConfig NetworkConfig;

	_HIVE_EARLY_RETURN(!hiveConfig::hiveParseConfig(vConfigFile, hiveConfig::CONFIG_XML, &NetworkConfig), 
		_BOOST_STR1("Fail to parser network configuration file [%1%]!", vConfigFile), false);

	_HIVE_EARLY_RETURN(!NetworkConfig.isAttributeExisted(hiveNetworkCommon::CONFIG_KEYWORD::IP), 
		"Fail to initiailize network because the local IP address is not provided.", false);

	srand(_hiveIP2Integer(NetworkConfig.getAttribute<std::string>(hiveNetworkCommon::CONFIG_KEYWORD::IP)));

	return hiveNetworkCommon::hiveInitNetwork(&NetworkConfig);
}

//***********************************************************
//FUNCTION:
unsigned int hiveTCPIP::_hiveGenerateUserPacketID()
{
	return rand();  //FIXME: 通过_hiveIP2Integer()来生成随机数种子，如果多个网络程序运行在一台机器上，由于IP地址相同，将导致使用同一个种子
}

//***********************************************************
//FUNCTION:
unsigned int hiveTCPIP::_hiveIP2Integer(const std::string& vIP)
{
	_ASSERTE(!vIP.empty());

	int t[4];
	hiveCommon::hiveSplitLine2IntArray(vIP, ".", 4, t);
	
	return t[0] * 256 * 256 * 256 +
		   t[1] * 256 * 256 +
		   t[2] * 256 +
		   t[3];
}

//**************************************************************************************
//FUNCTION:
unsigned char hiveTCPIP::_hiveInterpretSocketUsage(const std::string& vUsage)
{
	unsigned char Usage = 0;

	std::vector<std::string> t;
	hiveCommon::hiveSplitLine(vUsage, "|", true, 0, t);
	_ASSERTE((t.size() == 1) || (t.size() == 2));
	for (unsigned int i=0; i<t.size(); i++)
	{
		if      (t[i] == "UNICAST")   Usage |= SOCKET_UNICAST;
		else if (t[i] == "BROADCAST") Usage |= SOCKET_BROADCAST;
		else if (t[i] == "MULTICAST") Usage |= SOCKET_MULTICAST;
		else if (t[i] == "RECEIVE")   Usage |= SOCKET_RECEIVE;
		else
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__,  _BOOST_STR1("Undefined socket usage keyword: [%1%]", t[i]));
			return SOCKET_UNDEFINED;
		}
	}

	if ((Usage != SOCKET_UNICAST) && (Usage != SOCKET_BROADCAST) && (Usage != SOCKET_MULTICAST) && (Usage != SOCKET_RECEIVE) &&
		(Usage != (SOCKET_UNICAST|SOCKET_RECEIVE)) && (Usage != (SOCKET_BROADCAST|SOCKET_RECEIVE)) && (Usage != (SOCKET_MULTICAST|SOCKET_RECEIVE)))
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Wrong combination of socket usage.");
		return SOCKET_UNDEFINED;
	}
	return Usage;
}