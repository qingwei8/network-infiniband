#pragma once
#include "NetworkThread.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

namespace hiveTCPIP
{
	class CIPBasedPacketHandlingThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		CIPBasedPacketHandlingThread();
		CIPBasedPacketHandlingThread(hiveNetworkCommon::CLogicalNode *vHostNode);
		virtual ~CIPBasedPacketHandlingThread();

	private:
		virtual void __runV() override;
	};
}