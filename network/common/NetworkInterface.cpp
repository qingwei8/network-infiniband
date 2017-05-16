#include "NetworkInterface.h"
#include <WinSock2.h>
#include <IPHlpApi.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "common/ConfigInterface.h"
#include "common/HiveConfig.h"
#include "PhysicalNode.h"
#include "LogicalNode.h"
#include "Packet.h"

std::string hiveNetworkCommon::g_LocalIP;

//*********************************************************************************
//FUNCTION:
bool hiveNetworkCommon::hiveInitNetwork(const hiveConfig::CHiveConfig *vConfig)
{
	_ASSERTE(vConfig && vConfig->isAttributeExisted(CONFIG_KEYWORD::IP));
	g_LocalIP = vConfig->getAttribute<std::string>(CONFIG_KEYWORD::IP);
	_HIVE_EARLY_RETURN(!CPhysicalNode::getInstance()->init(vConfig), "Fail to initialize the physical network node!", false);

	hiveCommon::hiveOutputEvent("Succeed to initialize the physical network node!");
	return true;
}

//***********************************************************
//FUNCTION:
const std::string& hiveNetworkCommon::hiveGetLocalIP()
{
	_ASSERTE(!g_LocalIP.empty());
	return g_LocalIP;
}

//**********************************************************************************
//FUNCTION:
void hiveNetworkCommon::hiveUpdateLogicalNodeConfig(const std::string& vNodeName, const std::string& vAttributePath, boost::any vAttributeValue)
{
	CPhysicalNode::getInstance()->updateLogicalNodeConfig(vNodeName, vAttributePath, vAttributeValue);
}

//****************************************************************************
//FUNCTION:
void hiveNetworkCommon::hiveStartLogicalNode(const std::string& vNodeName)
{
	_ASSERTE(!vNodeName.empty());
	CPhysicalNode::getInstance()->startLogicalNode(vNodeName);
}

//**************************************************************************************
//FUCTION:
void hiveNetworkCommon::hiveSendPacket(const IUserPacket *vUserPacket, const std::string& vNetworkNodeName, const SSendOptions& vSendOption)
{
	CLogicalNode *pNode = CPhysicalNode::getInstance()->findLogicalNodeByName(vNetworkNodeName);
	_ASSERTE(pNode);
	pNode->sendPacket(vUserPacket, vSendOption);
}

//**************************************************************************************
//FUCTION:
void hiveNetworkCommon::hiveStopNetwork()
{
	CPhysicalNode::getInstance()->stop();
}

//**************************************************************************************
//FUCTION:
bool hiveNetworkCommon::hiveBuildConnection(const std::string& vRemoteIP, unsigned short vRemotePort, const std::string& vProtocal)
{
	std::string Protocal = boost::to_upper_copy(vProtocal);
	_ASSERTE((Protocal == "IB") || (Protocal == "TCP"));
	CLogicalNode* pNode = CPhysicalNode::getInstance()->findLogicalNodeByProtocal(vProtocal);
	_HIVE_EARLY_RETURN(!pNode, _BOOST_STR3("Fail to build a connection to [%1%:%2%] because a proper node for protocal [%3%] cannot be found.", vRemoteIP, vRemotePort, vProtocal), false);
	return pNode->buildConnection(vRemoteIP, vRemotePort, Protocal == "TCP");
}

//******************************************************************
//FUNCTION:
const hiveConfig::CHiveConfig* hiveNetworkCommon::hiveGetNetworkConfig()
{
	return CPhysicalNode::getInstance()->getConfig();
}

//****************************************************************************
//FUNCTION:
hiveNetworkCommon::CLogicalNode* hiveNetworkCommon::hiveFindLogicalNodeByProtocal(const std::string& vProtocal)
{
	_ASSERTE(!vProtocal.empty());
	return CPhysicalNode::getInstance()->findLogicalNodeByProtocal(vProtocal);
}

//********************************************************************
//FUNCTION:
bool hiveNetworkCommon::hiveIsAdapterAvailable(const std::string& vAdapterName)
{
	IP_ADAPTER_INFO *pIPAdapterInfo = new IP_ADAPTER_INFO;
	unsigned long AdapterSize = sizeof(IP_ADAPTER_INFO);
	int QueryResult = GetAdaptersInfo(pIPAdapterInfo, &AdapterSize);

	if (QueryResult == ERROR_BUFFER_OVERFLOW)
	{ 
		delete pIPAdapterInfo;
		pIPAdapterInfo = reinterpret_cast<IP_ADAPTER_INFO*>(new BYTE[AdapterSize]);
		QueryResult = GetAdaptersInfo(pIPAdapterInfo, &AdapterSize);
	}
	if (QueryResult == ERROR_SUCCESS)
	{
		IP_ADAPTER_INFO *pTemp = pIPAdapterInfo;
		std::string NetworkCardDescription;
		while (pIPAdapterInfo)
		{
			NetworkCardDescription = pIPAdapterInfo->Description;
			NetworkCardDescription = boost::to_upper_copy(NetworkCardDescription);
			if (boost::istarts_with(NetworkCardDescription, boost::to_upper_copy(vAdapterName)))
			{
				delete[] pTemp;
				return true;
			}
			pIPAdapterInfo = pIPAdapterInfo->Next;
		}
	}

	if (pIPAdapterInfo)
	{
		delete[] pIPAdapterInfo;
	}

	return false;
}

//****************************************************************************
//FUNCTION:
bool hiveNetworkCommon::hiveIsConnectionBuilt(const std::string& vIP, const std::string& vProtocal)
{
	CLogicalNode *pNode = hiveFindLogicalNodeByProtocal(vProtocal);
	return pNode ? pNode->isConnectionBuilt(vIP) : false;
}