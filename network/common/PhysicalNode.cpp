#include "PhysicalNode.h"
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "common/HiveConfig.h"
#include "NetworkCommon.h"
#include "LogicalNode.h"

using namespace hiveNetworkCommon;

CPhysicalNode::CPhysicalNode() : m_pConfig(nullptr)
{
}

CPhysicalNode::~CPhysicalNode()
{
	stop();
	_ASSERTE(m_LogicalNodeSet.empty()); 
}

//****************************************************************************
//FUNCTION:
void CPhysicalNode::__destroyAllLogicalNodes()
{
	for (auto itr=m_LogicalNodeSet.begin(); itr!=m_LogicalNodeSet.end(); itr++)
	{
		delete itr->second;
	}
	m_LogicalNodeSet.clear();
	_SAFE_DELETE(m_pConfig);
}

//*********************************************************************************
//FUNCTION:
void CPhysicalNode::__registerLogicalNode(CLogicalNode *vNode)
{
	_ASSERTE(vNode && !vNode->getName().empty() && (m_LogicalNodeSet.count(vNode->getName()) == 0));

	m_LogicalNodeSet[vNode->getName()] = vNode;
	vNode->__setHostPhysicalNode(this);
}

//****************************************************************************
//FUNCTION:
void CPhysicalNode::updateLogicalNodeConfig(const std::string& vNodeName, const std::string& vAttributePath, boost::any vAttributeValue)
{
	_ASSERTE(!m_LogicalNodeSet.empty());
	auto itr = m_LogicalNodeSet.find(boost::to_upper_copy(vNodeName));
	_HIVE_EARLY_EXIT((itr == m_LogicalNodeSet.end()), _BOOST_STR2("Fail to update attribute [%2%] of node [%1%] because it does not exist.", vNodeName, vAttributePath));

	itr->second->updateNodeConfig(vAttributePath, vAttributeValue);
}

//****************************************************************************
//FUNCTION:
void CPhysicalNode::startLogicalNode(const std::string& vNodeName)
{
	_ASSERTE(!m_LogicalNodeSet.empty());
	auto itr = m_LogicalNodeSet.find(boost::to_upper_copy(vNodeName));
	_HIVE_EARLY_EXIT((itr == m_LogicalNodeSet.end()), _BOOST_STR1("Fail to start node [%1%] because it does not exist.", vNodeName));

	CLogicalNode *pNode = itr->second;
	_ASSERTE(!pNode->isRunning() && !pNode->isInitialized());
	pNode->init();
	pNode->start();
}

//*********************************************************************************
//FUNCTION:
bool CPhysicalNode::init(const hiveConfig::CHiveConfig *vConfig)
{
	_ASSERTE(vConfig && !m_pConfig);

	try
	{
 		m_pConfig = dynamic_cast<hiveConfig::CHiveConfig*>(vConfig->cloneConfigV());
		_ASSERTE(m_pConfig);
		__createLogicalNodes(m_pConfig);

		for (auto itr=m_LogicalNodeSet.begin(); itr!=m_LogicalNodeSet.end(); itr++)
		{
			const hiveConfig::CHiveConfig *pConfig = itr->second->getConfig();
			if (pConfig->getAttribute<bool>(CONFIG_KEYWORD::START_NODE_AFTER_CREATION))
			{
 				_ASSERTE(itr->second);
				itr->second->init();
				itr->second->start();
			}
		}
		 
		hiveCommon::hiveOutputEvent("Succeed to initialize physical node.");
		return true;
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Fail to initialize the physical network node due to unexpected error.");
		return false;		
	}
}

//***********************************************************
//FUNCTION:
std::string CPhysicalNode::getIP() const
{//NOTE: 我们强行要求用户在配置文件中给出本机IP地址，这是因为系统中可能存在多个网卡（LAN，WIFI...）
	_ASSERTE(m_pConfig && m_pConfig->isAttributeExisted(CONFIG_KEYWORD::IP));
	return m_pConfig->getAttribute<std::string>(CONFIG_KEYWORD::IP);
}

//*********************************************************************************
//FUNCTION:
void CPhysicalNode::__createLogicalNodes(const hiveConfig::CHiveConfig *vNetworkConfig)
{
	_ASSERTE(vNetworkConfig);
	for (unsigned int i=0; i<vNetworkConfig->getNumSubConfig(); i++)
	{
		const hiveConfig::CHiveConfig *pLogicalNodeConfig = dynamic_cast<const hiveConfig::CHiveConfig*>(vNetworkConfig->getSubConfigAt(i));
		_ASSERTE(pLogicalNodeConfig && pLogicalNodeConfig->isAttributeExisted(CONFIG_KEYWORD::OBJECT_CREATION_SIG));

		CLogicalNode *pNode = dynamic_cast<CLogicalNode*>(hiveCommon::hiveCreateProduct(pLogicalNodeConfig->getAttribute<std::string>(CONFIG_KEYWORD::OBJECT_CREATION_SIG)));	
		if (!pNode) 
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("Fail to create logical node [%1%]", pLogicalNodeConfig->getName()));
			continue;
		}

		pNode->__setConfig(pLogicalNodeConfig);
		__registerLogicalNode(pNode);

		hiveCommon::hiveOutputEvent(_BOOST_STR1("Succeed to create logical node [%1%].", pNode->getName()));
	}
}

//*********************************************************************************
//FUNCTION:
CLogicalNode* CPhysicalNode::findLogicalNodeByName(const std::string& vName) const
{
   std::map<std::string, CLogicalNode*>::const_iterator itr = m_LogicalNodeSet.find(boost::to_upper_copy(vName));
	return (itr != m_LogicalNodeSet.end()) ? itr->second : nullptr;
}

//*********************************************************************************
//FUNCTION: 
void CPhysicalNode::stop()
{
	for (auto itr=m_LogicalNodeSet.begin(); itr!=m_LogicalNodeSet.end(); itr++)
	{
 		itr->second->stop();
	}
	__destroyAllLogicalNodes();
}

//****************************************************************************
//FUNCTION:
//ASSUMPTION：1.一个CPhysicalNode中，针对某种通讯协议（TCP，UDP，Infiniband...），只包含一个CLogicalNode对象；
//            2. CLogicalNode对应的配置中，必须有PROTOCAL这个属性
CLogicalNode* CPhysicalNode::findLogicalNodeByProtocal(const std::string& vProtocal) const
{
	for (auto itr=m_LogicalNodeSet.begin(); itr!=m_LogicalNodeSet.end(); itr++)
	{
		const hiveConfig::CHiveConfig *pNodeConfig = itr->second->getConfig();
		_ASSERTE(pNodeConfig && pNodeConfig->isAttributeExisted(CONFIG_KEYWORD::PROTOCAL));
		if (pNodeConfig->getAttribute<std::string>(CONFIG_KEYWORD::PROTOCAL) == boost::to_upper_copy(vProtocal)) return itr->second;
	}
	return nullptr;
}