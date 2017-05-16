#include "IBConfig.h"
#include <boost/format.hpp>
#include "NetworkCommon.h"
#include "IBCommon.h"
#include "Common/HiveCommonMicro.h"
#include "Common/CommonInterface.h"

using namespace hiveInfiniband;

CIBConfig::CIBConfig()
{
	defineAttribute(CONFIG_KEYWORD::IB_AUX_NODE,			   hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(CONFIG_KEYWORD::IB_CQ_HANDLE_THREAD_SIG,   hiveConfig::ATTRIBUTE_STRING);
	defineAttribute(CONFIG_KEYWORD::NUM_TRY_BUILDING_CONN,     hiveConfig::ATTRIBUTE_INT);
	defineAttribute(CONFIG_KEYWORD::IB_MEMORY_BLOCK_SIZE,      hiveConfig::ATTRIBUTE_INT);
	defineAttribute(CONFIG_KEYWORD::IB_NUM_MEMORY_BLOCK,	   hiveConfig::ATTRIBUTE_INT);
	defineAttribute(CONFIG_KEYWORD::MAX_NUM_RECEIVE_REQUEST,   hiveConfig::ATTRIBUTE_INT);
	defineAttribute(CONFIG_KEYWORD::MAX_NUM_SEND_REQUEST,      hiveConfig::ATTRIBUTE_INT);
	defineAttribute(CONFIG_KEYWORD::IB_NUM_QUEUE_PAIR,		   hiveConfig::ATTRIBUTE_INT);
	defineAttribute(CONFIG_KEYWORD::IB_PERFORMANCE_TEST_MODE,  hiveConfig::ATTRIBUTE_BOOL);
	defineAttribute(CONFIG_KEYWORD::IB_PERFORMANCE_TEST_NUM_REQUEST, hiveConfig::ATTRIBUTE_INT);
}

CIBConfig::~CIBConfig()
{
}

//********************************************************************
//FUNCTION:
bool CIBConfig::__otherVerifyOnNodeV(const hiveConfig::CHiveConfig *vNodeConfig) const 
{
	_ASSERT(vNodeConfig);

	if (vNodeConfig->getName() == "IB")
	{
		_HIVE_EARLY_RETURN(!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_AUX_NODE), "The config must specifies an auxiliary node!", false);
		std::string AuxNodeName = vNodeConfig->getAttribute<std::string>(CONFIG_KEYWORD::IB_AUX_NODE);
		_HIVE_EARLY_RETURN(AuxNodeName.empty(), "The auxiliary node cannot be empty!", false);

		if (!__findAuxNodeConfigByName(AuxNodeName))
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("Fail to find the configuration of auxiliary node [%1%].", AuxNodeName));
			return false;
		}
	}

	return true;
}

//********************************************************************
//FUNCTION:
void CIBConfig::__extraOnLoadedV(hiveConfig::CHiveConfig* vNodeConfig)
{
	_ASSERT(vNodeConfig);

	if (vNodeConfig->getAttribute<std::string>(hiveNetworkCommon::CONFIG_KEYWORD::PROTOCAL) == "IB")
	{
		vNodeConfig->setAttribute(hiveNetworkCommon::CONFIG_KEYWORD::OBJECT_CREATION_SIG, DEFAULT_IB_NODE_SIG);
	    if (!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_CQ_HANDLE_THREAD_SIG))  	   vNodeConfig->setAttribute(CONFIG_KEYWORD::IB_CQ_HANDLE_THREAD_SIG, DEFAULT_IB_CQ_HANDLE_THREAD_SIG);
		if (!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::NUM_TRY_BUILDING_CONN))		   vNodeConfig->setAttribute(CONFIG_KEYWORD::NUM_TRY_BUILDING_CONN, (int)DEFAULT_NUM_TRY_BUILDING_CONN);
		if (!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_NUM_QUEUE_PAIR))			   vNodeConfig->setAttribute(CONFIG_KEYWORD::IB_NUM_QUEUE_PAIR, DEFAULT_IB_NUM_QUEUE_PAIR);
		if (!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::MAX_NUM_RECEIVE_REQUEST))		   vNodeConfig->setAttribute(CONFIG_KEYWORD::MAX_NUM_RECEIVE_REQUEST, DEFAULT_IB_QP_RQ_DEPTH);
		if (!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::MAX_NUM_SEND_REQUEST))		       vNodeConfig->setAttribute(CONFIG_KEYWORD::MAX_NUM_SEND_REQUEST, DEFAULT_IB_QP_SQ_DEPTH);
		if (!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_PERFORMANCE_TEST_MODE))		   vNodeConfig->setAttribute(CONFIG_KEYWORD::IB_PERFORMANCE_TEST_MODE, false);
		if (!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_PERFORMANCE_TEST_NUM_REQUEST)) vNodeConfig->setAttribute(CONFIG_KEYWORD::IB_PERFORMANCE_TEST_NUM_REQUEST, DEFAULT_PERFORMANCE_TEST_NUM_REQUEST);
		if (!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_NUM_MEMORY_BLOCK))			   vNodeConfig->setAttribute(CONFIG_KEYWORD::IB_NUM_MEMORY_BLOCK, DEFAULT_IB_NUM_MEMORY_BLOCK);
		if (!vNodeConfig->isAttributeExisted(CONFIG_KEYWORD::IB_MEMORY_BLOCK_SIZE))		       vNodeConfig->setAttribute(CONFIG_KEYWORD::IB_MEMORY_BLOCK_SIZE, DEFAULT_IB_MEMORY_BLOCK_SIZE);
	}
}

//********************************************************************
//FUNCTION:
bool CIBConfig::__findAuxNodeConfigByName(const std::string& vAuxNodeName) const
{
	for (unsigned int i=0; i<getNumSubConfig(); i++)
	{
		const hiveConfig::CHiveConfig* pNodeConfig = getSubConfigAt(i);
		if (pNodeConfig->getName()== vAuxNodeName) return true;
	}
	return false;
}