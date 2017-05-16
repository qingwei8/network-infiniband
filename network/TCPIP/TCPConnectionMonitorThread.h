#pragma once
#include "NetworkThread.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

namespace hiveTCPIP
{
	class CTCPConnectionMonitorThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		CTCPConnectionMonitorThread();
		CTCPConnectionMonitorThread(hiveNetworkCommon::CLogicalNode *vHostNode);
		virtual ~CTCPConnectionMonitorThread() {}

	private:
		virtual void __runV() override;
	};
}