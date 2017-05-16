#pragma once
#include "CSNetworkTopologyExport.h"

namespace hiveNetworkTopology
{
	class NETWORK_TOPOLOGY_DLL_EXPORT CBaseManageTCPConnection
	{
	public:
		CBaseManageTCPConnection() {}
		virtual ~CBaseManageTCPConnection() {}

		bool isNeedTCPConnection() {return _isNeedTCPConnectionV();};

	protected:
		virtual bool _isNeedTCPConnectionV() {return true;}
	};
}
