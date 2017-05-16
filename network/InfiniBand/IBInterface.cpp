#include "IBInterface.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/HiveCommonMicro.h"
#include "Common/ConfigInterface.h"
#include "IBConfig.h"
#include "NetworkCommon.h"
#include "NetworkInterface.h"

using namespace hiveInfiniband;

//********************************************************************
//FUNCTION:
INFINIBAND_DLL_EXPORT bool hiveInfiniband::hiveInitIB(const std::string& vNetworkConfigFile)
{
	_HIVE_EARLY_RETURN(!hiveNetworkCommon::hiveIsAdapterAvailable("mellanox"), "Fail to intialize IB because this computer does not have an IB adapter.", false);
	
	CIBConfig NetworkConfig;
	_HIVE_EARLY_RETURN(!hiveConfig::hiveParseConfig(vNetworkConfigFile, hiveConfig::CONFIG_XML, &NetworkConfig),
		_BOOST_STR1("Fail to parser network configuration file [%1%]!", vNetworkConfigFile), false);

	return hiveNetworkCommon::hiveInitNetwork(&NetworkConfig);
}