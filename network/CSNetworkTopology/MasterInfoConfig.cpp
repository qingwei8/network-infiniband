#include "MasterInfoConfig.h"
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "NetworkNodeConfig.h"
#include "ClientPhysicalNode.h"

using namespace hiveNetworkTopology;

CMasterInfoConfig::CMasterInfoConfig()
{
	defineAttribute("MASTER_IP",				 hiveConfig::ATTRIBUTE_STRING);
	defineAttribute("MASTER_TCP_LISTENING_PORT", hiveConfig::ATTRIBUTE_INT);
	defineAttribute("MASTER_UDP_LISTENING_PORT", hiveConfig::ATTRIBUTE_INT);
	defineAttribute("SEND_KEEP_ALIVE_OPEN",      hiveConfig::ATTRIBUTE_STRING);
	defineAttribute("SEND_PACKET_SIG_AND_TIME",  hiveConfig::ATTRIBUTE_SUBCONFIG);
	defineAttribute("SIG",                       hiveConfig::ATTRIBUTE_SUBCONFIG);
	defineAttribute("TIME",						 hiveConfig::ATTRIBUTE_INT);
}

//******************************************************************
//FUNCTION:
void CMasterInfoConfig::getInfoFromPhysicalNodeConfig(const hiveNetworkCommon::CNetworkNodeConfig* vConfig)
{
	_ASSERT(vConfig);
	for (int i = 0; i<vConfig->getNumSubConfig(); i++)
	{
		const hiveNetworkCommon::CNetworkNodeConfig* pNetworkNodeConfig = (const hiveNetworkCommon::CNetworkNodeConfig*)vConfig->getSubConfigAt(i);

		if (pNetworkNodeConfig->getName() == "UDP")
		{
			const hiveNetworkCommon::CNetworkNodeConfig* UDPConfig = (const hiveNetworkCommon::CNetworkNodeConfig*)pNetworkNodeConfig->getSubConfigAt(0);
			_ASSERT(UDPConfig);
			m_LocalUDPListeningPort = UDPConfig->getAttribute<int>("LOCAL_PORT");
		}
		else if (pNetworkNodeConfig->getName() == "TCP")
		{
		    const hiveNetworkCommon::CNetworkNodeConfig* TCPConfig = (const hiveNetworkCommon::CNetworkNodeConfig*)pNetworkNodeConfig->getSubConfigAt(0);
			_ASSERT(TCPConfig);
			m_LocalTCPListeningPort = TCPConfig->getAttribute<int>("LOCAL_PORT");
		}
	}
}

//******************************************************************
//FUNCTION:
void CMasterInfoConfig::initMasterInfo()
{
	m_MasterIP               = getAttribute<std::string>("MASTER_IP");
	m_MasterTCPListeningPort = getAttribute<int>("MASTER_TCP_LISTENING_PORT");
    m_MasterUDPListeningPort = getAttribute<int>("MASTER_UDP_LISTENING_PORT");

	if ("FALSE" == getAttribute<std::string>("SEND_KEEP_ALIVE_OPEN"))
	{
		CClientPhysicalNode::getInstance()->setKeepAliveOpenFlag(false);
	}

	if (getNumSubConfig() > 0)
	{
		const hiveConfig::CHiveConfig* pConfig = getSubConfigAt(0);
		for (int i = 0; i<pConfig->getNumSubConfig(); i++)
		{
			const hiveConfig::CHiveConfig* pSigConfig = pConfig->getSubConfigAt(i);
			CClientPhysicalNode::getInstance()->insertSendPacketSig(pSigConfig->getAttribute<int>("TIME"), pSigConfig->getName());
		}
	}
}