#pragma once
#include "../common/NetworkThread.h"

namespace hiveNetworkTopology
{
	class IMessage;

	class CCSNTMessageHandleThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		CCSNTMessageHandleThread();
		virtual ~CCSNTMessageHandleThread() {};

	private:
		void __parseMessage(const IMessage* vReciveMessage);
		void __parseKeepAliveMessage(const IMessage* vReciveMessage);
		void __parseRegisterMessage(const IMessage* vReciveMessage);
		void __parseRegisterReplyMessage(const IMessage* vReciveMessage);
		void __registerClientByRegisterMessage(const IMessage* vReciveMessage);
		bool __sendReplyMessage(const std::string& vReplyMessageSig, const IMessage* vReciveMessage, IMessage* vReplyMessage);

		virtual void __runV() override;
	};
}