#pragma once
#include "../common/NetworkThread.h"

namespace hiveNetworkTopology
{
	class CTimingSendPacketThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		CTimingSendPacketThread();
		virtual ~CTimingSendPacketThread() {};

	private:
		void __sendKeepAliveMessage();
		void __sendConfigPacket(const std::string& vSig);
		void __sendAllPacketFirstTime(const std::multimap<int, std::string>& vSendPacketSets);

		virtual void __runV() override;
	};
}