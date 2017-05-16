#pragma once
#include "../common/NetworkThread.h"

namespace hiveNetworkTopology
{
	class CRegisterThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		CRegisterThread();
		virtual ~CRegisterThread() {};

	private:
		virtual void __runV() override;
	};
}