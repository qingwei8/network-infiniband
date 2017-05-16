#include "ServerPhysicalNode.h"
#include "common\HiveCommonMicro.h"
#include "common\CommonInterface.h"
#include "../common/PhysicalNode.h"
#include "TopologyCommon.h"
#include "BaseManageTCPConnection.h"
#include "CSNTMessageHandleThread.h"
#include "ManageClientConnectionThread.h"
#include "CSNetworkTopologyMessagePool.h"
#include "ConnectionUpdateInfo.h"

using namespace hiveNetworkTopology;

CServerPhysicalNode::~CServerPhysicalNode()
{
	_SAFE_DELETE(m_pManageTCPConnection);
	_SAFE_DELETE(m_pConnectionUpdateInfo);
	_SAFE_DELETE(m_pCSNTMessageHandleThread);
	_SAFE_DELETE(m_pManageClientConnectionThread);
	
	__clearClientInfoSet();
}

CServerPhysicalNode::CServerPhysicalNode() : m_IsNodeInit(false), m_pCSNTMessageHandleThread(nullptr), m_pManageClientConnectionThread(nullptr), m_pManageTCPConnection(nullptr), m_pConnectionUpdateInfo(nullptr),
	m_ClientUDPPort(CLIENT_UDPLISTENING_PORT), m_ClientTCPPort(CLIENT_TCPLISTENING_PORT)
{

}

//*********************************************************************************************************************************************************
//FUNCTION:
bool CServerPhysicalNode::init(const hiveNetworkCommon::CNetworkNodeConfig *vConfig, bool vInitLogicalNode)
{
	_ASSERTE(vConfig);

	_HIVE_SIMPLE_IF(m_IsNodeInit, return true);

	try
	{
		_HIVE_SIMPLE_IF(!m_pManageTCPConnection, m_pManageTCPConnection = new CBaseManageTCPConnection());
		_HIVE_SIMPLE_IF(!m_pCSNTMessageHandleThread, m_pCSNTMessageHandleThread = new CCSNTMessageHandleThread());
		_HIVE_SIMPLE_IF(m_pCSNTMessageHandleThread, m_pCSNTMessageHandleThread->startThread());
		_HIVE_SIMPLE_IF(!m_pManageClientConnectionThread, m_pManageClientConnectionThread = new CManageClientConnectionThread());
		_HIVE_SIMPLE_IF(m_pManageClientConnectionThread, m_pManageClientConnectionThread->startThread());
		_HIVE_SIMPLE_IF(!m_pConnectionUpdateInfo, m_pConnectionUpdateInfo = new CConnectionUpdateInfo());
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Fail to initialize the physical network node due to unexpected error.");
		return false;		
	}

	if (hiveNetworkCommon::CPhysicalNode::getInstance()->init(vConfig, vInitLogicalNode) == true)
	{
		m_IsNodeInit = true;
		hiveCommon::hiveOutputEvent("server has been init success!");
		return true;
	}
	else
		return false;
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CServerPhysicalNode::registerClient(SClientInfo* vClientInfo)
{
	m_RegisterClientSetMutex.lock();
	m_RegisterClientSet.push_back(vClientInfo);
	time_t CurrentTime;
	time(&CurrentTime);
	SClientInfo ClientInfo = *vClientInfo;
	m_pConnectionUpdateInfo->_addConnetion(&SConnectionInfo(ClientInfo.ClientIP, ClientInfo.ClientTCPListeningPort, ClientInfo.ClientUDPListeningPort, CurrentTime));
	m_RegisterClientSetMutex.unlock();
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CServerPhysicalNode::updateClientInfoByIP(const std::string& vIP, unsigned int vClientUDPListeningPort)
{
	_ASSERT(vIP.size() != 0);

	m_RegisterClientSetMutex.lock();
	for (auto itr = m_RegisterClientSet.begin(); itr!=m_RegisterClientSet.end(); itr++)
	{
		if ((*itr)->ClientIP.c_str() == vIP && (*itr)->ClientUDPListeningPort == vClientUDPListeningPort)
			(*itr)->LastConnectTime = time(0);
	}
	m_RegisterClientSetMutex.unlock();
}

//*********************************************************************************************************************************************************
//FUNCTION:
bool CServerPhysicalNode::isClientExit(const std::string& vIP, unsigned int vClientUDPListeningPort)
{
	for (auto itr = m_RegisterClientSet.begin(); itr!=m_RegisterClientSet.end(); itr++)
	{
		if ((*itr)->ClientIP.c_str() == vIP && (*itr)->ClientUDPListeningPort == vClientUDPListeningPort)
			return true;
	}
	return false;
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CServerPhysicalNode::stopPhysicalNode()
{
	_HIVE_SIMPLE_IF(m_pCSNTMessageHandleThread, m_pCSNTMessageHandleThread->interruptThread());
	_HIVE_SIMPLE_IF(m_pManageClientConnectionThread, m_pManageClientConnectionThread->interruptThread());

	CCSNTMessagePool::getInstance()->notifyAll();

	_HIVE_SIMPLE_IF(m_pCSNTMessageHandleThread, m_pCSNTMessageHandleThread->join());
	_HIVE_SIMPLE_IF(m_pManageClientConnectionThread,m_pManageClientConnectionThread->join());
	__clearMessageQueue();
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CServerPhysicalNode::__clearClientInfoSet()
{
	for (unsigned int i=0; i<m_RegisterClientSet.size(); ++i)
	{
		_SAFE_DELETE(m_RegisterClientSet[i]);
	}

	m_RegisterClientSet.clear();
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CServerPhysicalNode::monitorClientConnection()
{
	for (unsigned int i=0; i<m_RegisterClientSet.size(); ++i)
	{
		time_t CurrentTime;
		time(&CurrentTime); 
		time_t ConnectTime = m_RegisterClientSet.at(i)->LastConnectTime;
		double DifferenceOfRegisterTimeAndCurrentTime = difftime(CurrentTime, ConnectTime);

		if (DifferenceOfRegisterTimeAndCurrentTime > LASTCONNECTED_CURRENT_THRESHOLD_INTERVAL_TIME && m_RegisterClientSet.at(i)->KeepAliveOpenFlag == true)
		{
			__deleteClientConnection(i);
		}
	}
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CServerPhysicalNode::__deleteClientConnection(int vIndex)
{
	auto itr = m_RegisterClientSet.begin();

	hiveCommon::hiveOutputEvent("Delete a lost Client");

	m_RegisterClientSetMutex.lock();
	time_t CurrentTime;
	time(&CurrentTime);
	SClientInfo ClientInfo = *(m_RegisterClientSet.at(vIndex));
	m_pConnectionUpdateInfo->_deleteConnection(&SConnectionInfo(ClientInfo.ClientIP, ClientInfo.ClientTCPListeningPort, ClientInfo.ClientUDPListeningPort, CurrentTime));
	m_RegisterClientSet.erase(itr+vIndex);
	m_RegisterClientSetMutex.unlock();
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CServerPhysicalNode::setManageTCPConnection(CBaseManageTCPConnection* vManageTCPConnection)
{
	_SAFE_DELETE(m_pManageTCPConnection);
	m_pManageTCPConnection = vManageTCPConnection;
}

//*********************************************************************************************************************************************************
//FUNCTION:
bool CServerPhysicalNode::isNeedRegisterClientBuildTCPConnection()
{
	if (m_pManageTCPConnection == nullptr)
		return true;
	else
		return m_pManageTCPConnection->isNeedTCPConnection();
}

//******************************************************************
//FUNCTION:
void CServerPhysicalNode::queryConnectionUpdate(CConnectionUpdateInfo& voUpdatedConnectionInfo)
{
	voUpdatedConnectionInfo = *m_pConnectionUpdateInfo;
	m_pConnectionUpdateInfo->_clear();
}

//**************************************************************************************
//FUCTION:
void CServerPhysicalNode::__clearMessageQueue()
{
	unsigned int MessagePacketNum = CCSNTMessagePool::getInstance()->getSize();
	for (unsigned int i=0; i<MessagePacketNum; ++i)
	{
		hiveNetworkCommon::IUserPacket* pPacket = nullptr;
		CCSNTMessagePool::getInstance()->tryPop(pPacket);
		_SAFE_DELETE(pPacket);
	}
}