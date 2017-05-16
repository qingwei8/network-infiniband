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

//NOTE: һ��TCP�ڵ㲻�ۺ������ڵ��ж���TCP���ӣ���ֻ����һ�������̣߳��ڷ���ʱ�����ݰ��Ľ��շ���ѡȡ���ʵ�socket���з��͡����������ÿ��TCP���Ӷ���Ӧһ��
//      �հ��̲߳�ͬ����ͬ����ʽ�£��������TCP���Ӷ�Ӧͬһ���հ��̣߳�����ͬ����ʽ�հ�������û���յ�����ʱ�߳��������ᵼ��һ��TCP������û�����ݵ������
//      �հ��̶߳�ʹ�����������޷������հ�
	voOutput.push_back(new CTCPDataSendingThread(this));
	voOutput.push_back(new CTCPConnectionMonitorThread(this));
	voOutput.push_back(new hiveNetworkCommon::CUserPacketHandlingThread(this));
}

//*********************************************************************************
//FUNCTION:
void CTCPNode::__closeAndDestroyAllSocketsV()
{
//NOTE: ������麯���в�ʵ���κι��ܣ���ʱ��Ϊ��TCP�У�socket��connection�ǳɶԳ��֣���˽�socket����Ĺ��ܷ���connection�����У��������connection��socket���ֲ�һ�£�����connection��ɾ���ˣ�����Ӧ��socket���ڣ�
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

	{//NOTE: ��Դ����ŵ�Ŀ���Ǳ�֤m_ConnectionSet���̰߳�ȫ��boost::mutex::scoped_lock�ļ�����Χ��
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