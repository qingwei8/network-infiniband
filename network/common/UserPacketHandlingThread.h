#pragma once
#include "NetworkThread.h"
#include "NetworkExport.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;

	class NETWORK_DLL_EXPORT CUserPacketHandlingThread : public INetworkThread
	{
	public:
		CUserPacketHandlingThread();
		CUserPacketHandlingThread(hiveNetworkCommon::CLogicalNode *vHostNode);
		virtual ~CUserPacketHandlingThread();

	private:
		virtual void __runV() override;
	};
}