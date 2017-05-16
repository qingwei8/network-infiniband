#pragma once
#include <vector>
#include <boost/thread/mutex.hpp>
#include "common\Singleton.h"
#include "CSNetworkTopologyExport.h"

namespace hiveNetworkCommon
{
	class CNetworkNodeConfig;
}

namespace hiveNetworkTopology
{
	struct SClientInfo;
	class CBaseManageTCPConnection;
	class CCSNTMessageHandleThread;
	class CManageClientConnectionThread;
	class CConnectionUpdateInfo;

	class NETWORK_TOPOLOGY_DLL_EXPORT CServerPhysicalNode : public hiveCommon::CSingleton<CServerPhysicalNode>
	{
	public:
		virtual ~CServerPhysicalNode();

		bool isNodeInit() {return m_IsNodeInit;}
		bool isNeedRegisterClientBuildTCPConnection();
		bool isClientExit(const std::string& vIP, unsigned int vClientUDPListeningPort);
		bool init(const hiveNetworkCommon::CNetworkNodeConfig *vConfig, bool vInitLogicalNode);

		unsigned int getClientUDPPort() {return m_ClientUDPPort;}
		unsigned int getClientTCPPort() {return m_ClientTCPPort;}

		void stopPhysicalNode();
		void monitorClientConnection();
		void registerClient(SClientInfo* vClientInfo);
		void setManageTCPConnection(CBaseManageTCPConnection* vManageTCPConnection);
		void updateClientInfoByIP(const std::string& vIP, unsigned int vClientUDPListeningPort);
		const std::vector<SClientInfo*>& getRegisterClientSet() const {return m_RegisterClientSet;}
		void queryConnectionUpdate(CConnectionUpdateInfo& voUpdatedConnectionInfo);
		void setClientUDPPort(unsigned int vUDPPort) {m_ClientUDPPort = vUDPPort;}
		void setClientTCPPort(unsigned int vTCPPort) {m_ClientTCPPort = vTCPPort;}
		
	private:
		bool						   m_IsNodeInit;
		unsigned int                   m_ClientUDPPort;
		unsigned int                   m_ClientTCPPort;
		boost::mutex				   m_RegisterClientSetMutex;
		CBaseManageTCPConnection*      m_pManageTCPConnection;
		CCSNTMessageHandleThread*      m_pCSNTMessageHandleThread;
		CManageClientConnectionThread* m_pManageClientConnectionThread;
		std::vector<SClientInfo*>	   m_RegisterClientSet;

		CConnectionUpdateInfo*		   m_pConnectionUpdateInfo;

		CServerPhysicalNode();

		void __clearMessageQueue();
		void __clearClientInfoSet();
		void __deleteClientConnection(int vIndex);

		friend class hiveCommon::CSingleton<CServerPhysicalNode>;
	};
}
