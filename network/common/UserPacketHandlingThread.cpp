#include "UserPacketHandlingThread.h"
#include "Packet.h"
#include "LogicalNode.h"

using namespace hiveNetworkCommon;

CUserPacketHandlingThread::CUserPacketHandlingThread()
{
	_setThreadName("CUserPacketHandlingThread");
}

CUserPacketHandlingThread::CUserPacketHandlingThread(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	_ASSERTE(vHostNode);
	_setHostLogicalNode(vHostNode);
	_setThreadName("CUserPacketHandlingThread");
}

CUserPacketHandlingThread::~CUserPacketHandlingThread()
{
}

//*********************************************************************************
//FUNCTION:
void CUserPacketHandlingThread::__runV()
{
	hiveNetworkCommon::IUserPacket *pUserPacket = nullptr;
	CLogicalNode *pHostNode = fetchHostLogicalNode();
	_ASSERTE(pHostNode);

	while (isRunning() && !isInterruptionRequested())
	{
		pUserPacket = pHostNode->_waitAndPopIncommingUserPacket();
		if (!pUserPacket) continue;
		pUserPacket->processPacketV(pHostNode);
		delete pUserPacket;
	}
}