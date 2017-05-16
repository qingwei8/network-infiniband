#include <boost\format.hpp>
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "common\FileSystem.h"
#include "common\ConfigInterface.h"
#include "../common/PhysicalNode.h"
#include "../common/LogicalNode.h"
#include "../common/NetworkSelector.h"
#include "../common/PacketExtraInfoOnFile.h"
#include "../common/NetworkInterface.h"
#include "../common/Packet.h"
#include "../common/NetworkNodeConfig.h"
#include "ClientPhysicalNode.h"
#include "ServerPhysicalNode.h"
#include "CSNetworkTopologyInterface.h"
#include "MasterInfoConfig.h"
#include "NetworkAutoSelect.h"

//*******************************************************************
//FUNCTION:
bool initClientNode(const hiveNetworkCommon::CNetworkNodeConfig* vConfig, const std::string& vTopologyConfigFile/*= ""*/)
{
	_ASSERT(vConfig);

	if (hiveCommon::CFileSystem::getInstance()->isRegualFile(vTopologyConfigFile) == false)
	{
		hiveCommon::hiveOutputEvent("waring: lack of master info config, can not init network!");
		return false;
	}
	hiveConfig::hiveParseConfig(vTopologyConfigFile, hiveConfig::CONFIG_XML, dynamic_cast<hiveNetworkTopology::CMasterInfoConfig*>(hiveNetworkTopology::CMasterInfoConfig::getInstance()));
	hiveNetworkTopology::CMasterInfoConfig::getInstance()->initMasterInfo();
	hiveCommon::hiveOutputEvent("Master config parsing completed !");

	if (hiveNetworkTopology::CClientPhysicalNode::getInstance()->init(vConfig, true))
	{
		hiveNetworkTopology::CClientPhysicalNode::getInstance()->waitRegister();
		return true;
	}
	else 
		return false;
}

//*******************************************************************
//FUNCTION:
bool setPacketTargetPort(hiveNetworkCommon::CLogicalNode* vNetworkNode, hiveNetworkCommon::IUserPacket *vPacket, hiveNetworkTopology::ESendToType vSendToType)
{
	_ASSERT(vPacket);
	_ASSERT(vNetworkNode);
	
	if (vSendToType == hiveNetworkTopology::TO_CLIENT)
	{
		if (vNetworkNode->getName() == "TCP")
		{
			vPacket->setPacketTargetPort(hiveNetworkTopology::CServerPhysicalNode::getInstance()->getClientTCPPort());
			vPacket->setPacketSourcePort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalTCPListeningPort());
		}
		else if (vNetworkNode->getName() == "UDP" || vNetworkNode->getName() == "IB")
		{
			vPacket->setPacketTargetPort(hiveNetworkTopology::CServerPhysicalNode::getInstance()->getClientUDPPort());
			vPacket->setPacketSourcePort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalUDPListeningPort());
		}
	}
	else if (vSendToType == hiveNetworkTopology::TO_MASTER)
	{
		if (vNetworkNode->getName() == "TCP")
		{
			vPacket->setPacketTargetPort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getMasterTCPListeningPort());
			vPacket->setPacketSourcePort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalTCPListeningPort());
		}
		else if (vNetworkNode->getName() == "UDP" || vNetworkNode->getName() == "IB")
		{
			vPacket->setPacketTargetPort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getMasterUDPListeningPort());
			vPacket->setPacketSourcePort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalUDPListeningPort());
		}
	}
	else
	{
		hiveCommon::hiveOutputEvent("Fail to send packet, because of error ESendToType!");
		return false;
	}
	return true;
}

//*********************************************************************************
//FUNCTION:
bool hiveNetworkTopology::hiveInitNetwork(const hiveNetworkCommon::CNetworkNodeConfig* vConfig, const std::string& vNetWorkType, const std::string& vTopologyConfigFile/*= ""*/)
{
	_ASSERT(vConfig);

	hiveNetworkTopology::CMasterInfoConfig::getInstance()->getInfoFromPhysicalNodeConfig(vConfig);

	std::string NetworkType = boost::algorithm::to_upper_copy(vNetWorkType);
	if (NetworkType == "SERVER")
	{
		_HIVE_SIMPLE_IF_ELSE(CServerPhysicalNode::getInstance()->init(vConfig, true), return true, return false)
	}
	else if (NetworkType == "CLIENT")
	{
		return initClientNode(vConfig, vTopologyConfigFile);
	}
	else
	{
		hiveCommon::hiveOutputEvent("Fail to initialize the physical network node, because of error NetWorkType!");
		return false;
	}
}

//*********************************************************************************
//FUNCTION:
bool hiveNetworkTopology::hiveInitNetwork(const std::string& vNetWorkConfigFile, const std::string& vNetWorkType, const std::string& vTopologyConfigFile /*= ""*/)
{
	hiveNetworkCommon::CNetworkNodeConfig* pNodeConfig = new hiveNetworkCommon::CNetworkNodeConfig();

	_HIVE_SIMPLE_IF(!hiveConfig::hiveParseConfig(vNetWorkConfigFile, hiveConfig::CONFIG_XML, pNodeConfig), return false);
	
	return hiveInitNetwork(pNodeConfig, vNetWorkType, vTopologyConfigFile);
}

//**************************************************************************************
//FUCTION:
bool hiveNetworkTopology::sendPacket(hiveNetworkCommon::IUserPacket* vPacket, ESendToType vSendToType, bool vDeletePacketAfterSentHint)
{
	hiveNetworkCommon::CLogicalNode* pBaseNetworkNode = hiveNetworkTopology::CNetworkAutoSelect::getInstance()->selectNetworkNode(vPacket);

	if (pBaseNetworkNode)
	{
		if (setPacketTargetPort(pBaseNetworkNode, vPacket, vSendToType))
		{
			pBaseNetworkNode->sendPacket(vPacket);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

//**************************************************************************************
//FUCTION:
bool hiveNetworkTopology::sendPacket(hiveNetworkCommon::IUserPacket *vPacket, ESendToType vSendToType, const std::string& vNetworkNodeName, bool vDeletePacketAfterSentHint)
{
	return hiveNetworkTopology::sendPacket(vPacket, vSendToType, vNetworkNodeName.c_str(), vDeletePacketAfterSentHint);
}

//**************************************************************************************
//FUCTION:
bool hiveNetworkTopology::sendPacket(hiveNetworkCommon::IUserPacket *vPacket, ESendToType vSendToType, const char* vNetworkNodeName, bool vDeletePacketAfterSentHint)
{
	if (vNetworkNodeName == "TCP" || vNetworkNodeName == "IB")
		hiveNetworkCommon::buildConnection(vPacket->getPacketTargetIP(), vPacket->getPacketTargetPort(), vNetworkNodeName);

	hiveNetworkCommon::CLogicalNode* pBaseNetworkNode = hiveNetworkTopology::CNetworkAutoSelect::getInstance()->selectNetworkNode(vNetworkNodeName);
	
	if (pBaseNetworkNode)
	{
		if (setPacketTargetPort(pBaseNetworkNode, vPacket, vSendToType))
		{
			pBaseNetworkNode->sendPacket(vPacket);
			return true;
		}
		else
			return false;
	}
	else
		return false;
}

//*********************************************************************************
//FUNCTION:
bool hiveNetworkTopology::sendFile(const std::string& vFilePath, ESendToType vSendToType, const std::string& vTargetIP, unsigned int vTargetPort, const std::string& vSendType /*= "TCP"*/, hiveNetworkCommon::ESendMode vSendMode /*= hiveNetworkCommon::UNICAST*/)
{
	if (!hiveCommon::CFileSystem::getInstance()->isRegualFile(vFilePath))
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("File [%1%] not exits!", vFilePath));
		return false;
	}

	hiveNetworkCommon::IUserPacket* pSendPacket = new hiveNetworkCommon::IUserPacket;
	hiveNetworkCommon::CPacketExtraInfoOnFile* pFileInfo = new hiveNetworkCommon::CPacketExtraInfoOnFile;
	pFileInfo->setFilePath(vFilePath);

	pSendPacket->setPacketTargetIP(vTargetIP);
	pSendPacket->setPacketTargetPort(vTargetPort);
	pSendPacket->setPacketType(hiveNetworkCommon::EPacketType::FILE);
	pSendPacket->setSendMode(vSendMode);
	pSendPacket->setContainExtraInfoFlag();
	pSendPacket->setPacketExtraInfo(pFileInfo);

	return hiveNetworkTopology::sendPacket(pSendPacket, vSendToType, vSendType.c_str());
}

//**************************************************************************************
//FUCTION:
void hiveNetworkTopology::stopNetwork()
{
	hiveNetworkTopology::CClientPhysicalNode::getInstance()->stopPhysicalNode();

	hiveNetworkCommon::CPhysicalNode::getInstance()->stop();

	boost::this_thread::sleep(boost::posix_time::milliseconds(1000));

	hiveNetworkTopology::CServerPhysicalNode::getInstance()->stopPhysicalNode();
	
}

//******************************************************************
//FUNCTION:
void hiveNetworkTopology::queryConnectionUpdate(CConnectionUpdateInfo& voUpdatedConnectionInfo)
{
	if (hiveNetworkTopology::CServerPhysicalNode::getInstance()->isNodeInit())
	{
		hiveNetworkTopology::CServerPhysicalNode::getInstance()->queryConnectionUpdate(voUpdatedConnectionInfo);
	}
	else
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Client can not query connection update information.");
	}
}

//******************************************************************
//FUNCTION:
void hiveNetworkTopology::queryMasterInfo(SMasterInfo& voMasterInfo)
{
	voMasterInfo.MasterIP = CMasterInfoConfig::getInstance()->getMasterIP();
	voMasterInfo.MasterTCPListeningPort = CMasterInfoConfig::getInstance()->getMasterTCPListeningPort();
	voMasterInfo.MasterUDPListeningPort = CMasterInfoConfig::getInstance()->getMasterUDPListeningPort();
}