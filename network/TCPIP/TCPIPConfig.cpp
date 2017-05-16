#include "TCPIPConfig.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "NetworkCommon.h"
#include "TCPIPCommon.h"
#include "IPPacketHeader.h"

using namespace hiveTCPIP;

unsigned int hiveTCPIP::g_MaxUserDataInIPPacket;

CTCPIPConfig::CTCPIPConfig()
{
	__defineAcceptableAttributeSet();
}

CTCPIPConfig::~CTCPIPConfig()
{
}

//****************************************************************************
//FUNCTION:
void CTCPIPConfig::__defineAcceptableAttributeSet()
{
	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::IP,                  hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::NODE,	            hiveConfig::ATTRIBUTE_SUBCONFIG);
	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::PROTOCAL,            hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::OBJECT_CREATION_SIG, hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::START_NODE_AFTER_CREATION, hiveConfig::ATTRIBUTE_BOOL);

	defineAttribute(CONFIG_KEYWORD::PORT,				     hiveConfig::ATTRIBUTE_INT);
	defineAttribute(CONFIG_KEYWORD::SOCKET,			     	 hiveConfig::ATTRIBUTE_SUBCONFIG);
	defineAttribute(CONFIG_KEYWORD::UDP_SOCKET_USAGE,		 hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(CONFIG_KEYWORD::TARGET_IP,			     hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(CONFIG_KEYWORD::UDP_RECEIVE_BUFFER_SIZE, hiveConfig::ATTRIBUTE_INT);
	defineAttribute(CONFIG_KEYWORD::UDP_MAX_PACKET_SIZE,     hiveConfig::ATTRIBUTE_INT);
}

//******************************************************************
//FUNCTION:
void CTCPIPConfig::_onLoadedV()
{
	for (unsigned int i=0; i<getNumSubConfig(); i++)
	{
		hiveConfig::CHiveConfig *pNodeConfig = fetchSubConfigAt(i);
		_ASSERTE(pNodeConfig);
		if (pNodeConfig->getType() == hiveNetworkCommon::CONFIG_KEYWORD::NODE)
		{
			if (!pNodeConfig->isAttributeExisted(hiveNetworkCommon::CONFIG_KEYWORD::START_NODE_AFTER_CREATION)) pNodeConfig->setAttribute(hiveNetworkCommon::CONFIG_KEYWORD::START_NODE_AFTER_CREATION, true);
			if (pNodeConfig->isAttributeExisted(hiveNetworkCommon::CONFIG_KEYWORD::PROTOCAL) && !pNodeConfig->isAttributeExisted(hiveNetworkCommon::CONFIG_KEYWORD::OBJECT_CREATION_SIG))
			{
				std::string Protocal = pNodeConfig->getAttribute<std::string>(hiveNetworkCommon::CONFIG_KEYWORD::PROTOCAL);
				if (Protocal == "UDP") pNodeConfig->setAttribute(hiveNetworkCommon::CONFIG_KEYWORD::OBJECT_CREATION_SIG, DEFAULT_UDP_NODE_SIG);
				if (Protocal == "TCP") pNodeConfig->setAttribute(hiveNetworkCommon::CONFIG_KEYWORD::OBJECT_CREATION_SIG, DEFAULT_TCP_NODE_SIG);
			    __extraOnLoadedV(pNodeConfig);
			}
		}
	}

	if (!isAttributeExisted(CONFIG_KEYWORD::UDP_RECEIVE_BUFFER_SIZE)) setAttribute(CONFIG_KEYWORD::UDP_RECEIVE_BUFFER_SIZE, UDP_DEFAULT_RECEIVE_BUFFER_SIZE);
	if (!isAttributeExisted(CONFIG_KEYWORD::UDP_MAX_PACKET_SIZE))     setAttribute(CONFIG_KEYWORD::UDP_MAX_PACKET_SIZE, UDP_DEFAULT_MAX_PACKET_SIZE);
	if (!isAttributeExisted(CONFIG_KEYWORD::TCP_RECEIVE_BUFFER_SIZE)) setAttribute(CONFIG_KEYWORD::TCP_RECEIVE_BUFFER_SIZE, TCP_DEFAULT_RECEIVE_BUFFER_SIZE);
	if (!isAttributeExisted(CONFIG_KEYWORD::TCP_MAX_PACKET_SIZE))     setAttribute(CONFIG_KEYWORD::TCP_MAX_PACKET_SIZE, TCP_DEFAULT_MAX_PACKET_SIZE);
	if (!isAttributeExisted(CONFIG_KEYWORD::TCP_CONNECTION_MONITOR_INTERVAL_TIME)) setAttribute(CONFIG_KEYWORD::TCP_CONNECTION_MONITOR_INTERVAL_TIME, TCP_CONNECTION_MONITOR_DEFAULT_INTERVAL_TIME);

	g_MaxUserDataInIPPacket = getAttribute<int>(CONFIG_KEYWORD::UDP_MAX_PACKET_SIZE) - CIPPacketHeader::getIPPacketHeaderSize() - 1;
}

//******************************************************************
//FUNCTION:
bool CTCPIPConfig::_verifyConfigV() const
{
	bool IsValid = true;

	if (!isAttributeExisted(hiveNetworkCommon::CONFIG_KEYWORD::IP))
	{//NOTE: 需要配置本机IP，在发包时，本机IP将作为种子来生成随机数作为包的ID
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The configuration of local IP is missed.");
		IsValid = false;
	}

	if (getAttribute<int>(CONFIG_KEYWORD::UDP_RECEIVE_BUFFER_SIZE) < getAttribute<int>(CONFIG_KEYWORD::UDP_MAX_PACKET_SIZE))
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The size of maximum UDP packet should be no bigger than receiving buffer size.");
		IsValid = false;
	}

	for (unsigned int i=0; i<getNumSubConfig(); i++)
	{
		const hiveConfig::CHiveConfig *pNodeConfig = getSubConfigAt(i);
		_ASSERTE(pNodeConfig);
		if (((pNodeConfig->getType() == hiveNetworkCommon::CONFIG_KEYWORD::NODE)) && !__isValidNodeConfiguration(pNodeConfig)) IsValid = false;
	}
	return IsValid;
}

//******************************************************************
//FUNCTION:
bool CTCPIPConfig::__isValidNodeConfiguration(const hiveConfig::CHiveConfig *vNodeConfig) const
{
	bool IsValid = true;

	_ASSERTE(vNodeConfig);
	if (!vNodeConfig->isAttributeExisted(hiveNetworkCommon::CONFIG_KEYWORD::PROTOCAL))
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("Invalid node configuration [%1%] due to missing definition of protocal.", vNodeConfig->getName()));
		IsValid = false;
	}

	for (unsigned int i=0; i<vNodeConfig->getNumSubConfig(); i++)
	{
		const hiveConfig::CHiveConfig *pSocketConfig = vNodeConfig->getSubConfigAt(i);
		_ASSERTE(pSocketConfig);
		if ((pSocketConfig->getType() == CONFIG_KEYWORD::SOCKET) && !__isValidSocketConfiguration(pSocketConfig)) IsValid = false;
	}

	if (!__otherVerifyOnNodeV(vNodeConfig)) IsValid = false;
	
	return IsValid;
}

//******************************************************************
//FUNCTION:
bool CTCPIPConfig::__isValidSocketConfiguration(const hiveConfig::CHiveConfig *vSocketConfig) const
{
	bool IsValid = true;

	_ASSERTE(vSocketConfig);
	if (vSocketConfig->isAttributeExisted(hiveTCPIP::CONFIG_KEYWORD::UDP_SOCKET_USAGE))
	{
		if (vSocketConfig->getAttribute<std::string>(hiveTCPIP::CONFIG_KEYWORD::UDP_SOCKET_USAGE) == std::string("MULTICAST"))
		{
			//TODO: make sure the port is defined for broadcasting and multicasting
			if (!vSocketConfig->isAttributeExisted(hiveTCPIP::CONFIG_KEYWORD::TARGET_IP))
			{
				hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Fail to specify 'Target_IP' for multicast socket");
				IsValid = false;
			}
		}
	}
	return true;
}