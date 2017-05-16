#include "IPBasedPacketHandlingThread.h"
#include "Packet.h"
#include "IPBasedNode.h"

using namespace hiveTCPIP;

CIPBasedPacketHandlingThread::CIPBasedPacketHandlingThread()
{
	_setThreadName("CIPBasedPacketHandlingThread");
}

CIPBasedPacketHandlingThread::CIPBasedPacketHandlingThread(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	_ASSERTE(vHostNode);
	_setHostLogicalNode(vHostNode);
	_setThreadName("CIPBasedPacketHandlingThread");
}

CIPBasedPacketHandlingThread::~CIPBasedPacketHandlingThread()
{
}

//*********************************************************************************
//FUNCTION:
void CIPBasedPacketHandlingThread::__runV()
{
	hiveNetworkCommon::IUserPacket *pUserPacket = nullptr;
	CIPBasedNode *pHostNode = dynamic_cast<CIPBasedNode*>(fetchHostLogicalNode());
	_ASSERTE(pHostNode);

	while (isRunning() && !isInterruptionRequested())
	{
		pUserPacket = pHostNode->_waitAndPopIncommingUserPacket();
		if (!pUserPacket) continue;
		pUserPacket->processPacketV(pHostNode);
		delete pUserPacket;
	}
}