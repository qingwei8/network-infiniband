#include <algorithm>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/HiveCommon.h"
#include "common/HiveCommonMicro.h"
#include "common/CommonInterface.h"
#include "common/ProductFactory.h"
#include "common/HiveConfig.h"
#include "UserPacketHandlingThread.h"
#include "TCPIPCommon.h"
#include "TCPNode.h"
#include "TCPConnectionListenThread.h"
#include "TCPConnectionMonitorThread.h"
#include "TCPDataSendingThread.h"
#include "TCPDataReceivingThread.h"
#include "TCPConnection.h"
#include "TCPCommon.h"
#include "PhysicalNode.h"

using namespace hiveTCPIP;

hiveCommon::CProductFactory<CTCPNode> theCreator(DEFAULT_TCP_NODE_SIG);

CTCPNode::CTCPNode()
{
}

CTCPNode::~CTCPNode()
{
}

//*********************************************************************************
//FUNCTION:
void CTCPNode::__createNetworkThreadsV(std::vector<hiveNetworkCommon::INetworkThread*>& voOutput)
{
	_ASSERTE(voOutput.empty());

//NOTE: 一个TCP节点不论和其他节点有多少TCP连接，都只包含一个发包线程，在发包时，根据包的接收方来选取合适的socket进行发送。这种情况和每个TCP连接都对应一个
//      收包线程不同。在同步方式下，如果所有TCP连接对应同一个收包线程，由于同步方式收包决定了没有收到数据时线程阻塞，会导致一个TCP连接上没有数据到达，阻塞
//      收包线程而使得其他连接无法进行收包
	voOutput.push_back(new CTCPDataSendingThread(this));
	voOutput.push_back(new CTCPConnectionMonitorThread(this));
	voOutput.push_back(new hiveNetworkCommon::CUserPacketHandlingThread(this));
}

//*********************************************************************************
//FUNCTION:
void CTCPNode::__closeAndDestroyAllSocketsV()
{
//NOTE: 在这个虚函数中不实现任何功能，这时因为在TCP中，socket和connection是成对出现，因此将socket管理的功能放入connection对象中，避免出现connection和socket出现不一致（例如connection被删除了，但对应的socket还在）
}

//*********************************************************************************
//FUNCTION:
bool CTCPNode::__extraInitV()
{
	const hiveConfig::CHiveConfig* pNodeConfig = getConfig();
	_ASSERTE(pNodeConfig);
	
	for (unsigned int i=0; i<pNodeConfig->getNumSubConfig(); ++i)
	{
		const hiveConfig::CHiveConfig *pSocketConfig = pNodeConfig->getSubConfigAt(i);
		_ASSERTE(pSocketConfig && pSocketConfig->isAttributeExisted(CONFIG_KEYWORD::PORT));

		CTCPConnectionListenThread *pListeningThread = new CTCPConnectionListenThread(this, pSocketConfig->getAttribute<int>(CONFIG_KEYWORD::PORT));
	}
	return true;
}

//*********************************************************************************
//FUNCTION:
boost::asio::ip::tcp::socket* CTCPNode::findSocket(const std::string& vRemoteIP, unsigned int vRemotePort)
{
	boost::mutex::scoped_lock Lock(m_Mutex4Connection);
	for (unsigned int i=0; i<m_ConnectionSet.size(); ++i)
	{
		if (m_ConnectionSet[i]->isConnectedTo(vRemoteIP, vRemotePort)) 
		{
			CTCPConnection *pConn = dynamic_cast<CTCPConnection*>(m_ConnectionSet[i]);
			_ASSERT(pConn);
			return pConn->fetchSocket();
		}
	}
	return nullptr;
}

//**************************************************************************************
//FUCTION:
void CTCPNode::monitorConnection()
{
	std::vector<std::pair<std::string, unsigned short>> BrokenOutgoingConnectionSet;
	std::vector<CTCPConnection*> BrokenConnectionSet;

	{//NOTE: 这对大括号的目的是保证m_ConnectionSet的线程安全（boost::mutex::scoped_lock的加锁范围）
		boost::mutex::scoped_lock Lock(m_Mutex4Connection);

		auto itr = m_ConnectionSet.begin();
		for (auto itr=m_ConnectionSet.begin(); itr!=m_ConnectionSet.end(); itr++)
		{
			CTCPConnection *pConn = dynamic_cast<CTCPConnection*>(*itr);
			_ASSERT(pConn);
			if (pConn->isBroken()) 
			{
				if (pConn->isOutgoingConnection()) BrokenOutgoingConnectionSet.push_back(std::make_pair(pConn->getRemoteIP(), pConn->getRemotePort()));
				BrokenConnectionSet.push_back(pConn);
			}
		}

		for (auto itr=BrokenConnectionSet.begin(); itr!=BrokenConnectionSet.end(); itr++) 
		{
			(*itr)->closeConnection();
			delete *itr;
		}
	}

	if (BrokenOutgoingConnectionSet.empty()) return;

	for (unsigned int i=0; i<BrokenOutgoingConnectionSet.size(); i++)
	{
		buildConnection(BrokenOutgoingConnectionSet[i].first, BrokenOutgoingConnectionSet[i].second, true);
	}
}

//**********************************************************************************
//FUNCTION:
unsigned int CTCPNode::getNumIncommingConnection()
{
	unsigned int NumIncommingConnection = 0;

	boost::mutex::scoped_lock Lock(m_Mutex4Connection);
	for (auto itr = m_ConnectionSet.begin(); itr!=m_ConnectionSet.end(); itr++)
	{
		CTCPConnection *pConn = dynamic_cast<CTCPConnection*>(*itr);
		_ASSERT(pConn);
		if (!pConn->isOutgoingConnection()) NumIncommingConnection++;
	}
	return NumIncommingConnection;
}

void hiveTCPIP::CTCPNode::getPhyical()
{
	const hiveNetworkCommon::CPhysicalNode *pPhysicalNode = getHostPhysicalNode();
	CTCPNode* pIBNode = dynamic_cast<CTCPNode*>(pPhysicalNode->findLogicalNodeByName("TCP"));
}