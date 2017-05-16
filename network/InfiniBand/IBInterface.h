#pragma once
#include <string>
#include "InfinibandExport.h"

namespace hiveInfiniband
{
	INFINIBAND_DLL_EXPORT bool hiveInitIB(const std::string& vNetworkConfigFile);
}