#include <boost\format.hpp>
#include "CSNTMessageHandleThread.h"
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "../common/NetworkSelector.h"
#include "../common/NetworkInterface.h"
#include "MessageBase.h"
#include "CSNetworkTopologyMessagePool.h"
#include "ServerPhysicalNode.h"
#include "ClientPhysicalNode.h"
#include "TopologyCommon.h"
#include "RegisterMessage.h"
#include "MasterInfoConfig.h"
#include "RegisterReplyMessage.h"
#include "KeepAliveReplyMessage.h"
#include "CSNetworkTopologyInterface.h"

using namespace hiveNetworkTopology;

CCSNTMessageHandleThread::CCSNTMessageHandleThread()
{
	_setThreadName("CCSNTMessageHandleThread");
}

//*********************************************************************************
//FUNCTION:
void CCSNTMessageHandleThread::__runV()
{	
	while (isRunning())
	{
		boost::this_thread::interruption_point();

		IMessage* pTempMessage = nullptr;
		pTempMessage = dynamic_cast<IMessage*>(CCSNTMessagePool::getInstance()->waitAndPop());
		__parseMessage(pTempMessage);
		_SAFE_DELETE(pTempMessage);
	}
}

//*********************************************************************************
//FUNCTION:
void CCSNTMessageHandleThread::__parseMessage(const IMessage* vReciveMessage)
{
	_HIVE_SIMPLE_IF(vReciveMessage == nullptr, return);
	_HIVE_SIMPLE_IF(vReciveMessage->getPacketSourceIP().size() == 0, return);

	switch (vReciveMessage->getMessageType())
	{
	case IMessage::REGISTER_MESSAGE:
		{
			_HIVE_SIMPLE_IF(CServerPhysicalNode::getInstance()->isClientExit(vReciveMessage->getPacketSourceIP(), vReciveMessage->getUDPListeningPort()), break);
			__parseRegisterMessage(vReciveMessage);
			break;
		}
	case IMessage::KEEP_ALIVE_MESSAGE:
		{
			_HIVE_SIMPLE_IF(!CServerPhysicalNode::getInstance()->isClientExit(vReciveMessage->getPacketSourceIP(), vReciveMessage->getUDPListeningPort()), break);
			__parseKeepAliveMessage(vReciveMessage);
			break;
		}
	case IMessage::REGISTER_REPLY_MESSAGE:
		{
			__parseRegisterReplyMessage(vReciveMessage);
			break;
		}
	case IMessage::KEEP_ALIVE_REPLY_MESSAGE: 
		{
			CClientPhysicalNode::getInstance()->__updataConfirmConnectionTimeStamp();
			break;
		}
	default:
		break;
	}
}

//*********************************************************************************
//FUNCTION:
bool CCSNTMessageHandleThread::__sendReplyMessage(const std::string& vReplyMessageSig, const IMessage* vReciveMessage, IMessage* vReplyMessage)
{
	vReplyMessage->setPacketSourceIP(vReciveMessage->getPacketTargetIP());
	vReplyMessage->setPacketSourcePort(vReciveMessage->getPacketTargetPort());
	
	vReplyMessage->setPacketTargetIP(vReciveMessage->getPacketSourceIP());
	vReplyMessage->setPacketTargetPort(vReciveMessage->getUDPListeningPort());

	return hiveNetworkTopology::sendPacket(vReplyMessage, TO_CLIENT ,"UDP");
}

//*********************************************************************************
//FUNCTION:
void CCSNTMessageHandleThread::__registerClientByRegisterMessage(const IMessage* vMessage)
{
	_ASSERT(vMessage);

	SClientInfo* ClientInfo            = new SClientInfo;
	ClientInfo->ClientIP               = vMessage->getPacketSourceIP();
	ClientInfo->ClientUDPListeningPort = vMessage->getUDPListeningPort();
	ClientInfo->ClientTCPListeningPort = vMessage->getTCPListeningPort();
	ClientInfo->UDPConnectionFlag      = true;
	ClientInfo->LastConnectTime        = time(0);

	if (dynamic_cast<hiveNetworkTopology::CRegisterMessage*>((IMessage*)vMessage)->getKeepAliveOpenFlag() == false)
		ClientInfo->KeepAliveOpenFlag = false;
	CServerPhysicalNode::getInstance()->registerClient(ClientInfo);

#ifdef _DEBUG
			std::cout<<"注册了一个客户机"<<std::endl;
			hiveCommon::hiveOutputEvent(_BOOST_STR2("客户IP:[%1%] UDP监听端口:[%2%]\n", vMessage->getPacketSourceIP(), vMessage->getUDPListeningPort()));
#endif
}

//*********************************************************************************
//FUNCTION:
void CCSNTMessageHandleThread::__parseKeepAliveMessage(const IMessage* vReciveMessage)
{
	_ASSERT(vReciveMessage);

	hiveNetworkTopology::CKeepAliveReplyMessage* pCKeepAliveReplyMessage = dynamic_cast<hiveNetworkTopology::CKeepAliveReplyMessage*>(hiveCommon::hiveCreateProduct("MESSAGE_KEEPALIVE_REPLY"));

	CServerPhysicalNode::getInstance()->updateClientInfoByIP(vReciveMessage->getPacketSourceIP(), vReciveMessage->getUDPListeningPort());
	__sendReplyMessage("MESSAGE_KEEPALIVE_REPLY", vReciveMessage, pCKeepAliveReplyMessage);
}

//*********************************************************************************
//FUNCTION:
void CCSNTMessageHandleThread::__parseRegisterMessage(const IMessage* vReciveMessage)
{
	_ASSERT(vReciveMessage);

	CRegisterMessage* pTempMessage = dynamic_cast<CRegisterMessage*>((IMessage*)vReciveMessage);
	if (hiveNetworkTopology::CServerPhysicalNode::getInstance()->getClientUDPPort() != vReciveMessage->getUDPListeningPort())
		hiveNetworkTopology::CServerPhysicalNode::getInstance()->setClientUDPPort(vReciveMessage->getUDPListeningPort());
	if (hiveNetworkTopology::CServerPhysicalNode::getInstance()->getClientTCPPort() != vReciveMessage->getTCPListeningPort())
		hiveNetworkTopology::CServerPhysicalNode::getInstance()->setClientTCPPort(vReciveMessage->getTCPListeningPort());

	hiveNetworkTopology::CRegisterReplyMessage* pCRegisterReplyMessage = dynamic_cast<hiveNetworkTopology::CRegisterReplyMessage*>(hiveCommon::hiveCreateProduct("MESSAGE_REGISTER_REPLY"));

	if (hiveNetworkTopology::CServerPhysicalNode::getInstance()->isNeedRegisterClientBuildTCPConnection() == true)
		pCRegisterReplyMessage->setBuildTCPConnectionFlag();
	__registerClientByRegisterMessage(vReciveMessage);
	__sendReplyMessage("MESSAGE_REGISTER_REPLY", vReciveMessage, pCRegisterReplyMessage);
}

//*********************************************************************************
//FUNCTION:
void CCSNTMessageHandleThread::__parseRegisterReplyMessage(const IMessage* vReciveMessage)
{
	_ASSERT(vReciveMessage);

	CRegisterReplyMessage* pTempMessage = dynamic_cast<CRegisterReplyMessage*>((IMessage*)vReciveMessage);

	CClientPhysicalNode::getInstance()->notifyRegister();
	CClientPhysicalNode::getInstance()->__stopRegisterThread();
	CClientPhysicalNode::getInstance()->__startKeepAliveThread();

	if (CClientPhysicalNode::getInstance()->isKeepAliveOpen() == true)
	{
		CClientPhysicalNode::getInstance()->__updataConfirmConnectionTimeStamp();
		CClientPhysicalNode::getInstance()->__startConfirmConnectionThread();
	}

	if (pTempMessage->isNeedBuildTCPConnection() == true)
		hiveNetworkCommon::buildConnection(CMasterInfoConfig::getInstance()->getMasterIP(), CMasterInfoConfig::getInstance()->getMasterTCPListeningPort());
}
