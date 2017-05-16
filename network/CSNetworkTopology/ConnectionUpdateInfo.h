#pragma once
#include <vector>
#include "TopologyCommon.h"
#include "CSNetworkTopologyExport.h"

namespace hiveNetworkTopology
{
	class NETWORK_TOPOLOGY_DLL_EXPORT CConnectionUpdateInfo
	{
	public:
		CConnectionUpdateInfo();
		virtual ~CConnectionUpdateInfo();

		bool isUpdated() const {return m_State;}

		const std::vector<SConnectionInfo*>& getAddedConnectionInfo() const;
		const std::vector<SConnectionInfo*>& getDeletedConnectionInfo() const;

		CConnectionUpdateInfo& operator=(CConnectionUpdateInfo& vConnectUpdateInfo);

	protected:
		void _clear();
		void _addConnetion(const SConnectionInfo* vClientInfo);
		void _deleteConnection(const SConnectionInfo* vClientInfo);
		
	private:
		bool m_State;
		std::vector<SConnectionInfo*> m_AddedSet;
		std::vector<SConnectionInfo*> m_DeletedSet;

		void __clearVector(std::vector<SConnectionInfo*>& vVector);

		friend class CServerPhysicalNode;
	};
}