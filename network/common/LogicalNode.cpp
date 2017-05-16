#include "LogicalNode.h"
#include <algorithm>
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "common/HiveConfig.h"
#include "NetworkThread.h"
#include "Packet.h"
#include "PhysicalNode.h"
#include "NetworkConnection.h"
#include "NetworkInterface.h"

using namespace hiveNetworkCommon;

CLogicalNode::CLogicalNode() : m_IsRunning(false), m_pHostPhysicalNode(nullptr), m_IsInitialized(false)
{
}

CLogicalNode::~CLogicalNode()
{
	stop();

	for (auto itr=m_NetworkThreadSet.begin(); itr!=m_NetworkThreadSet.end(); ++itr)
	{
		if (itr->second)
		{
			hiveCommon::hiveOutputEvent(_BOOST_STR2("Network thread [%2%] has been released by node [%1%].", getName(), itr->first->getThreadName()));
			delete itr->first;
		}
	}

	_closeAllConnections();
}
 
//**********************************************************************************
//FUNCTION:
//NOTE: �������ʹ��ConnectionSet�������������ӣ�����Ϊ��INetworkConnection::closeConnection()�У��Ὣ�����m_ConnectionSet��ɾ����
//      �����ֱ����m_ConnectionSet�Ͻ��ж���ɾ�������ƻ�m_ConnectionSet����
void CLogicalNode::_closeAllConnections()
{
	std::vector<INetworkConnection*> ConnectionSet = m_ConnectionSet;
	for (unsigned int i = 0; i < ConnectionSet.size(); ++i)
	{
		ConnectionSet[i]->closeConnection();
		delete ConnectionSet[i];
	}
	_ASSERTE(m_ConnectionSet.empty());
}

//**********************************************************************************
//FUNCTION:
void CLogicalNode::__unregisterConnection(INetworkConnection* vConnection)
{
	_ASSERT(vConnection);
	boost::mutex::scoped_lock Lock(m_Mutex4Connection);
	m_ConnectionSet.erase(std::remove_if(m_ConnectionSet.begin(), m_ConnectionSet.end(), 
		[vConnection](INetworkConnection* v) {return (v == vConnection);}), m_ConnectionSet.end());
}

//**********************************************************************************
//FUNCTION:
//NOTE: ����������������ڸ�����ṩһ��������ڵ㱻��ʼ��֮ǰ�޸Ļ����������õĽӿڡ�һ�����͵�Ӧ�ó������ڲ��л���ϵͳ�ĳ�ʼ��
//      �����У�ĳЩ����ڵ�Ĳ���������Ϣ������Զ�ˡ�������������CONFIG_KEYWORD::START_NODE_AFTER_CREATION���ʹ�á�
void CLogicalNode::updateNodeConfig(const std::string& vAttributePath, boost::any vAttributeValue)
{
	_ASSERTE(!m_IsInitialized && !m_IsRunning);
	std::vector<std::string> AttrPath;

	hiveConfig::CHiveConfig *pSubConfig = &m_Config;
	hiveCommon::hiveSplitLine(vAttributePath, "|", true, -1, AttrPath);
	for (unsigned int i=0; i<AttrPath.size()-1; i++)
	{
		pSubConfig = pSubConfig->findSubConfigByName(AttrPath[i]);
		_HIVE_EARLY_EXIT(!pSubConfig, _BOOST_STR1("[%1%] is not a valid sub-configuration name.", AttrPath[i]));
	}
	pSubConfig->setAttribute(AttrPath[AttrPath.size()-1], vAttributeValue);
}

//**************************************************************************************
//FUCTION:
void CLogicalNode::registerConnection(INetworkConnection* vConnection)
{
	_ASSERTE(vConnection);
	boost::mutex::scoped_lock Lock(m_Mutex4Connection);
#ifdef _DEBUG
	for (unsigned int i=0; i<m_ConnectionSet.size(); i++)
	{
		if (m_ConnectionSet[i] == vConnection)
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("Fail to register connection to node [%1%] because it has been registered.", getName()));
			return;
		}
	}
#endif
	m_ConnectionSet.push_back(vConnection);
}

//***********************************************************
//FUNCTION:
void CLogicalNode::__setConfig(const hiveConfig::CHiveConfig *vConfig)
{
	_ASSERTE(vConfig); 
	m_Config = *vConfig;
}

//***********************************************************
//FUNCTION:
std::string CLogicalNode::getIP() const
{
	_ASSERTE(m_pHostPhysicalNode);
	return m_pHostPhysicalNode->getIP();
}

//*********************************************************************************
//FUNCTION:
void CLogicalNode::start()
{
	if (m_IsRunning) return;

	if (!m_IsInitialized) init();
	_ASSERTE(m_IsInitialized);

	_startNodeV();

	hiveCommon::hiveOutputEvent(_BOOST_STR1("Succeed to start node [%1%].", getName()));
	m_IsRunning = true;
}

//*********************************************************************************
//FUNCTION:
void CLogicalNode::stop()
{
	if (!m_IsRunning) return;

	__interruptAllThreads();
	__notifyAllWaitingConcurrentQueue();
	_stopNodeV();
	__joinAllThreads();
	__clearAllConcurrentQueue();

	hiveCommon::hiveOutputEvent(_BOOST_STR1("Succeed to stop node [%1%].", getName()));
	m_IsRunning = false;
}

//*********************************************************************************
//FUNCTION:
void CLogicalNode::init()
{
	if (!m_IsInitialized)
	{
		std::vector<INetworkThread*> ThreadSet;
		__createNetworkThreadsV(ThreadSet);  //NOTE: �����������̶߳������ﴴ����������UDP�µĽ����߳̾�����CUDPNode::__extraInitV()�д���
		_ASSERTE(m_NetworkThreadSet.size() == ThreadSet.size());

		_HIVE_EARLY_EXIT(!__extraInitV(), _BOOST_STR1("Fail to initialize the logical network node [%1%].", getName()));

		m_IsInitialized = true;
		hiveCommon::hiveOutputEvent(_BOOST_STR1("Succeed to initialize logical node [%1%].", getName()));
	}
}

//**************************************************************************************
//FUNCTION:
void CLogicalNode::__registerNetworkThread(INetworkThread* vNetworkThread)
{
	_ASSERTE(vNetworkThread);

#ifdef _DEBUG
	for (unsigned int i=0; i<m_NetworkThreadSet.size(); i++)
	{
		if (m_NetworkThreadSet[i].first == vNetworkThread)
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR2("The request of registering thread [%1%] to node [%2%] is ignored because it has been registered.", vNetworkThread->getThreadName(), getName()));
			return; 
		}
	}
#endif

	m_NetworkThreadSet.push_back(std::make_pair(vNetworkThread, vNetworkThread->isNodeResponsible4DestroyThread()));
}

//**************************************************************************************
//FUNCTION:
void CLogicalNode::__interruptAllThreads()
{
	std::for_each(m_NetworkThreadSet.begin(), m_NetworkThreadSet.end(), 
		[](std::pair<INetworkThread*, bool>& v){v.first->interruptThread();});
}

//**************************************************************************************
//FUNCTION:
void CLogicalNode::__joinAllThreads()
{
	std::for_each(m_NetworkThreadSet.begin(), m_NetworkThreadSet.end(), 
		[](std::pair<INetworkThread*, bool>& v){v.first->joinUntilTimeout(boost::posix_time::milliseconds(100), true);});
}

//**************************************************************************************
//FUNCTION:
void CLogicalNode::_startAllThreads()
{ 
	std::for_each(m_NetworkThreadSet.begin(), m_NetworkThreadSet.end(), 
   		[](std::pair<INetworkThread*, bool>& v){v.first->startThread();});
}

//**************************************************************************************
//FUNCTION:
void CLogicalNode::unregisterNetworkThread(const INetworkThread* vNetworkThread)
{
	m_NetworkThreadSet.erase(std::remove_if(m_NetworkThreadSet.begin(), m_NetworkThreadSet.end(), 
		[vNetworkThread](std::pair<INetworkThread*, bool>& v) {return (v.first == vNetworkThread);}), m_NetworkThreadSet.end());
}

//*********************************************************************************
//FUNCTION:
void CLogicalNode::sendPacket(const IUserPacket *vUserPacket, const SSendOptions& vSendOption)
{
	IUserPacket *pPacket = const_cast<IUserPacket*>(vUserPacket);
	pPacket->_setPacketSourceIP(hiveNetworkCommon::hiveGetLocalIP());
	_ASSERTE(vUserPacket && m_IsInitialized && vUserPacket->isReady2SendV() && vSendOption.isValid());
	__sendPacketV(vUserPacket, vSendOption);
}

//**************************************************************************************
//FUNCTION:
bool CLogicalNode::buildConnection(const std::string& vRemoteIP, unsigned short vRemotePort, bool vTagAsConnectedImmediately)
{
	_ASSERTE(m_IsInitialized && getConfig());

	std::string ConnectionObjSig = getConfig()->getAttribute<std::string>(CONFIG_KEYWORD::PROTOCAL) + std::string("_CONNECTION");
	INetworkConnection *pConnection = dynamic_cast<INetworkConnection*>(hiveCommon::hiveCreateProduct(ConnectionObjSig));
	_HIVE_EARLY_RETURN(!pConnection, _BOOST_STR1("Fail to create connection object due to invalid object signature [%1%].", ConnectionObjSig), false);
	pConnection->_setHostNode(this);
	if (!pConnection->buildConnection(vRemoteIP, vRemotePort, vTagAsConnectedImmediately))
	{
		delete pConnection;
		return false;
	}
	return true;
}

//****************************************************************************
//FUNCTION:
void CLogicalNode::notifyConnectionIsBroken(const std::string& vRemoteIP, unsigned short vRemotePort)
{
	boost::mutex::scoped_lock Lock(m_Mutex4Connection);
	for (unsigned int i=0; i<m_ConnectionSet.size(); i++)
	{
		if (m_ConnectionSet[i]->isConnectedTo(vRemoteIP, vRemotePort)) 
		{
			m_ConnectionSet[i]->notifyConnectionIsBroken();
			return;
		}
	}
#ifdef _DEBUG
	hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR2("The TCP connection to [%1%:%2%] does not exist.", vRemoteIP, vRemotePort));
#endif
}

//**************************************************************************************
//FUCTION:
bool CLogicalNode::isConnectionBuilt(const std::string& vRemoteIP, unsigned short vRemotePort)
{
	boost::mutex::scoped_lock Lock(m_Mutex4Connection);
	for (auto itr = m_ConnectionSet.begin(); itr!=m_ConnectionSet.end(); itr++)
	{
		if ((*itr)->isConnectedTo(vRemoteIP, vRemotePort)) return true;
	}
	return false;
}

//**************************************************************************************
//FUCTION:
std::pair<const IUserPacket*, SSendOptions> CLogicalNode::_waitAndPopOutgoingUserPacket()
{
	std::pair<const IUserPacket*, SSendOptions> t;
	m_OutgoingPacketQueue.waitAndPop(t);
	return t;
}

//**************************************************************************************
//FUCTION:
void CLogicalNode::__clearAllConcurrentQueue()
{
	for (unsigned int i=0; i<m_OutgoingPacketQueue.size(); ++i)
	{
		std::pair<const IUserPacket*, SSendOptions> t;
		m_OutgoingPacketQueue.tryPop(t);
		delete t.first;
	}

	for (unsigned int i=0; i<m_IncomingPacketQueue.size(); ++i)
	{
		IUserPacket *pPacket = nullptr;
		m_IncomingPacketQueue.tryPop(pPacket);
		delete pPacket;
	}
}

//**************************************************************************************
//FUCTION:
void CLogicalNode::__notifyAllWaitingConcurrentQueue()
{
	m_OutgoingPacketQueue.notifyAll();
	m_IncomingPacketQueue.notifyAll();
}

//**************************************************************************************
//FUNCTION:
IUserPacket* CLogicalNode::_waitAndPopIncommingUserPacket()
{
	IUserPacket *pPacket = nullptr;
	m_IncomingPacketQueue.waitAndPop(pPacket);
	return pPacket;
}
