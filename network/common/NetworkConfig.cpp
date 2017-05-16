//#include "NetworkConfig.h"
//#include <boost/format.hpp>
//#include "NetworkCommon.h"
//
//using namespace hiveNetworkCommon;
//
//CNetworkConfig::CNetworkConfig()
//{
//	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::IP,                  hiveConfig::ATTRIBUTE_STRING);
//	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::NODE,	            hiveConfig::ATTRIBUTE_SUBCONFIG);
//	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::PROTOCAL,            hiveConfig::ATTRIBUTE_STRING);
//	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::OBJECT_CREATION_SIG, hiveConfig::ATTRIBUTE_STRING);
//	defineAttribute(hiveNetworkCommon::CONFIG_KEYWORD::START_NODE_AFTER_CREATION, hiveConfig::ATTRIBUTE_BOOL);
//}
//
//CNetworkConfig::~CNetworkConfig()
//{
//}
//
////********************************************************************
////FUNCTION:
//void CNetworkConfig::_onLoadedV()
//{
//	for (unsigned int i=0; i<getNumSubConfig(); i++)
//	{
//		hiveConfig::CHiveConfig* pNodeConfig = fetchSubConfigAt(i);
//		if (pNodeConfig->getType() == hiveNetworkCommon::CONFIG_KEYWORD::NODE && !pNodeConfig->isAttributeExisted(hiveNetworkCommon::CONFIG_KEYWORD::START_NODE_AFTER_CREATION))
//		{
//			pNodeConfig->setAttribute(hiveNetworkCommon::CONFIG_KEYWORD::START_NODE_AFTER_CREATION, true);
//		}
//	}
//	_extralOnLoadedV();
//}
//
////********************************************************************
////FUNCTION:
//bool CNetworkConfig::_verifyConfigV() const
//{
//	bool IsValid = true;
//
//	if (!isAttributeExisted(hiveNetworkCommon::CONFIG_KEYWORD::IP))
//	{//NOTE: 需要配置本机IP，在发包时，本机IP将作为种子来生成随机数作为包的ID
//		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The configuration of local IP is missed.");
//		IsValid = false;
//	}
//
//	for (unsigned int i=0; i<getNumSubConfig(); i++)
//	{
//		const hiveConfig::CHiveConfig *pSubConfig = getSubConfigAt(i);
//		_ASSERTE(pSubConfig);
//		if (((pSubConfig->getType() == hiveNetworkCommon::CONFIG_KEYWORD::NODE)) && !__isValidNodeConfiguration(pSubConfig)) IsValid = false;
//	}
//	return IsValid;
//}
//
////********************************************************************
////FUNCTION:
//bool hiveNetworkCommon::CNetworkConfig::__isValidNodeConfiguration(const hiveConfig::CHiveConfig *vNodeConfig) const
//{
//	bool IsValid = true;
//
//	_ASSERTE(vNodeConfig);
//	if (!vNodeConfig->isAttributeExisted(hiveNetworkCommon::CONFIG_KEYWORD::PROTOCAL))
//	{
//		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("Invalid node configuration [%1%] due to missing definition of protocal.", vNodeConfig->getName()));
//		IsValid = false;
//	}
//
//	if (!_isValidNodeConfigurationV(vNodeConfig))
//	{
//		IsValid = false;
//	}
//
//	return IsValid;
//}