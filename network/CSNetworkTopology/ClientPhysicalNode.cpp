#include "ClientPhysicalNode.h"
#include "../common/PhysicalNode.h"
#include "RegisterThread.h"
#include "TimingSendPacketThread.h"
#include "TopologyCommon.h"
#include "ConfirmConnectedThread.h"
#include "common\HiveCommonMicro.h"
#include "common\CommonInterface.h"
#include "CSNTMessageHandleThread.h"
#include "CSNetworkTopologyMessagePool.h"

using namespace hiveNetworkTopology;

CClientPhysicalNode::CClientPhysicalNode() 
	: m_IsNodeInit(false), m_IsKeepAliveOpen(true), m_ConfirmConnectionTimeStamp(0), m_pRegisterThread(nullptr), m_pKeepAliveThread(nullptr), m_pCSNTMessageHandleThread(nullptr), m_pConfirmConnetedThread(nullptr)
{

}

CClientPhysicalNode::~CClientPhysicalNode()
{
	_SAFE_DELETE(m_pRegisterThread);
	_SAFE_DELETE(m_pKeepAliveThread);
	_SAFE_DELETE(m_pConfirmConnetedThread);
	_SAFE_DELETE(m_pCSNTMessageHandleThread);
}

//*********************************************************************************************************************************************************
//FUNCTION:
bool CClientPhysicalNode::init(const hiveNetworkCommon::CNetworkNodeConfig *vConfig, bool vInitLogicalNode)
{
	_ASSERT(vConfig);

	_HIVE_SIMPLE_IF(m_IsNodeInit, return true);

	try
	{
		_HIVE_SIMPLE_IF(!m_pRegisterThread, m_pRegisterThread = new CRegisterThread);
		_HIVE_SIMPLE_IF(!m_pKeepAliveThread, m_pKeepAliveThread = new CTimingSendPacketThread);
		_HIVE_SIMPLE_IF(!m_pConfirmConnetedThread, m_pConfirmConnetedThread = new CConfirmConnectedThread);
		_HIVE_SIMPLE_IF(!m_pCSNTMessageHandleThread, m_pCSNTMessageHandleThread = new CCSNTMessageHandleThread);
		_HIVE_SIMPLE_IF(m_pCSNTMessageHandleThread, m_pCSNTMessageHandleThread->startThread());
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Fail to initialize the physical network node due to unexpected error.");
		return false;		
	}

	if (hiveNetworkCommon::CPhysicalNode::getInstance()->init(vConfig, vInitLogicalNode) == true)
	{
		if (m_IsKeepAliveOpen == true)
			insertSendPacketSig(KEEPALIVE_INTERVAL_TIME/1000, "MESSAGE_KEEPALIVE");
		m_IsNodeInit = true;
		m_pRegisterThread->startThread();
		hiveCommon::hiveOutputEvent("client has been init success, now wait for register to server!");
		return true;
	}
	else
		return false;
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::stopPhysicalNode()
{
 	_HIVE_SIMPLE_IF(m_pRegisterThread, m_pRegisterThread->interruptThread());
	_HIVE_SIMPLE_IF(m_pKeepAliveThread, m_pKeepAliveThread->interruptThread());
	_HIVE_SIMPLE_IF(m_pConfirmConnetedThread, m_pConfirmConnetedThread->interruptThread());
	_HIVE_SIMPLE_IF(m_pCSNTMessageHandleThread, m_pCSNTMessageHandleThread->interruptThread());

 	CCSNTMessagePool::getInstance()->notifyAll();

	_HIVE_SIMPLE_IF(m_pRegisterThread, m_pRegisterThread->join());
	_HIVE_SIMPLE_IF(m_pKeepAliveThread, m_pKeepAliveThread->join());
	_HIVE_SIMPLE_IF(m_pConfirmConnetedThread, m_pConfirmConnetedThread->join());
	_HIVE_SIMPLE_IF(m_pCSNTMessageHandleThread, m_pCSNTMessageHandleThread->join());
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::__startRegisterThread()
{
	_HIVE_SIMPLE_IF(m_pRegisterThread == nullptr, m_pRegisterThread = new CRegisterThread);
	_HIVE_SIMPLE_IF(m_pRegisterThread, m_pRegisterThread->startThread());
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::__stopRegisterThread()
{
	_HIVE_SIMPLE_IF(m_pRegisterThread, m_pRegisterThread->interruptThread());
	_HIVE_SIMPLE_IF(m_pRegisterThread, m_pRegisterThread->join());
	_SAFE_DELETE(m_pRegisterThread);
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::__startKeepAliveThread()
{
	_HIVE_SIMPLE_IF(m_pKeepAliveThread == nullptr, m_pKeepAliveThread = new CTimingSendPacketThread);
	_HIVE_SIMPLE_IF(m_pKeepAliveThread, m_pKeepAliveThread->startThread());
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::__stopKeepAliveThread()
{
	_HIVE_SIMPLE_IF(m_pKeepAliveThread, m_pKeepAliveThread->interruptThread());
	_HIVE_SIMPLE_IF(m_pKeepAliveThread, m_pKeepAliveThread->join());
	_SAFE_DELETE(m_pKeepAliveThread);
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::__startConfirmConnectionThread()
{
	_HIVE_SIMPLE_IF(m_pConfirmConnetedThread == nullptr, m_pConfirmConnetedThread = new CConfirmConnectedThread);
	_HIVE_SIMPLE_IF(m_pConfirmConnetedThread, m_pConfirmConnetedThread->startThread());
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::__stopConfirmConnectionThread()
{
	_HIVE_SIMPLE_IF(m_pConfirmConnetedThread, m_pConfirmConnetedThread->interruptThread());
	_HIVE_SIMPLE_IF(m_pConfirmConnetedThread, m_pConfirmConnetedThread->join());
	_SAFE_DELETE(m_pConfirmConnetedThread);
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::__updataConfirmConnectionTimeStamp()
{
	m_ConfirmConnectionTimeStamp = time(0);
}

//*********************************************************************************************************************************************************
//FUNCTION:
const time_t hiveNetworkTopology::CClientPhysicalNode::getLastConfirmConnectionTimeStamp() const
{
	return m_ConfirmConnectionTimeStamp;
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::waitRegister() 
{
	boost::mutex RegisterMutex;
	boost::mutex::scoped_lock lock(RegisterMutex);
	m_RegisterSucceedSignal.wait(lock);
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::notifyRegister() 
{
	m_RegisterSucceedSignal.notify_one();
}

//*********************************************************************************************************************************************************
//FUNCTION:
void hiveNetworkTopology::CClientPhysicalNode::insertSendPacketSig(int vTime, std::string vSig)
{
	_ASSERT(vSig.size() != 0);

	m_SendPacketSigSets.insert(std::make_pair(vTime, vSig));
}