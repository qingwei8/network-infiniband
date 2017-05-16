#pragma once
#include "common\Singleton.h"
#include "common\ConcurrentQueue.h"
#include "CSNetworkTopologyExport.h"

namespace hiveNetworkCommon
{
	class IUserPacket;
}

namespace hiveNetworkTopology
{
	class NETWORK_TOPOLOGY_DLL_EXPORT CCSNTMessagePool : public hiveCommon::CSingleton<CCSNTMessagePool>
	{
	public:
		int getSize();

		hiveNetworkCommon::IUserPacket* waitAndPop();

		void notifyAll() {m_MessageQuque.notifyAll();}
		bool tryPop(hiveNetworkCommon::IUserPacket* voPacket);
		void pushMessage(hiveNetworkCommon::IUserPacket* vMessage);

	protected:
		CCSNTMessagePool() {}
		CCSNTMessagePool& operator = (const CCSNTMessagePool&); 

	private:
		hiveCommon::CConcurrentQueue<hiveNetworkCommon::IUserPacket*> m_MessageQuque;

		friend class hiveCommon::CSingleton<CCSNTMessagePool>;
	};
}