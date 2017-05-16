#pragma once
#include "../common/NetworkThread.h"

namespace hiveNetworkTopology
{
	class IMessage;

	class CManageClientConnectionThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		CManageClientConnectionThread();
		virtual ~CManageClientConnectionThread() {};

	private:
		virtual void __runV() override;
	};
}