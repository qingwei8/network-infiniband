#pragma once
#include "../common/NetworkThread.h"

namespace hiveNetworkTopology
{
	class CConfirmConnectedThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		CConfirmConnectedThread();
		virtual ~CConfirmConnectedThread() {};

	private:
		virtual void __runV() override;
	};
}