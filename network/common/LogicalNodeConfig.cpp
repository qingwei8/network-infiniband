#include "LogicalNodeConfig.h"
#include "common\ProductFactory.h"
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "NetworkCommon.h"

using namespace hiveNetworkCommon;

hiveCommon::CProductFactory<CLogicalNodeConfig> theCreator("CLogicalNodeConfig");

CLogicalNodeConfig::CLogicalNodeConfig(void) : m_NodeStartupType(ON_SEND_FIRST_PACKET)
{
}

CLogicalNodeConfig::CLogicalNodeConfig(const CLogicalNodeConfig& vConfig) : hiveConfig::CHiveConfig(vConfig)
{
	m_LocalIP         = vConfig.m_LocalIP;
	m_NodeStartupType = vConfig.m_NodeStartupType;
	m_NodeCreationSig = vConfig.m_NodeCreationSig;
}

CLogicalNodeConfig::~CLogicalNodeConfig(void)
{
}

//*********************************************************************************
//FUNCTION:
hiveConfig::CHiveConfig* CLogicalNodeConfig::cloneConfigV() const
{
	return new CLogicalNodeConfig(*this);
}

//*********************************************************************************
//FUNCTION:
bool CLogicalNodeConfig::_verifyConfigV() const
{
	if (m_NodeCreationSig.empty())
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The creation of logical node might fail without node signature.");
		return false;
	}
	return true;
}