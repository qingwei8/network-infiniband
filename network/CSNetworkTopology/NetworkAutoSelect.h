#pragma once
#include <map>
#include "TopologyCommon.h"
#include "common\Singleton.h"

namespace hiveNetworkCommon
{
	class IUserPacket;
	class CLogicalNode;
}

namespace hiveNetworkTopology
{
	class CNetworkAutoSelect : public hiveCommon::CSingleton<CNetworkAutoSelect>
	{
	public:
		~CNetworkAutoSelect();

		hiveNetworkCommon::CLogicalNode* selectNetworkNode(hiveNetworkCommon::IUserPacket* vPacket);
		hiveNetworkCommon::CLogicalNode* selectNetworkNode(const std::string& vNetworkNodeName);

	protected:
		CNetworkAutoSelect();

	private:
		std::map<std::string, hiveNetworkCommon::CLogicalNode*> m_AvailableNodeMap;

		void __initAvailableNodeMap();

		friend class hiveCommon::CSingleton<CNetworkAutoSelect>;
	};
}