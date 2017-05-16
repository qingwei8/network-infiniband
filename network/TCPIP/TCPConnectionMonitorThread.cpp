#include "TCPConnectionMonitorThread.h"
#include "NetworkInterface.h"
#include "TCPNode.h"

using namespace hiveTCPIP;

CTCPConnectionMonitorThread::CTCPConnectionMonitorThread()
{
	_setThreadName("CTCPConnectionMonitorThread");
}

CTCPConnectionMonitorThread::CTCPConnectionMonitorThread(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	_ASSERTE(vHostNode);
	_setThreadName("CTCPConnectionMonitorThread");
	_setHostLogicalNode(vHostNode);
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CTCPConnectionMonitorThread::__runV()
{
	CTCPNode *pTCPNode = dynamic_cast<CTCPNode*>(fetchHostLogicalNode());
	_ASSERTE(pTCPNode);
	const hiveConfig::CHiveConfig *pNetworkConfig = hiveNetworkCommon::hiveGetNetworkConfig();
	_ASSERTE(pNetworkConfig && pNetworkConfig->isAttributeExisted(CONFIG_KEYWORD::TCP_CONNECTION_MONITOR_INTERVAL_TIME));
	int IntervalTime = pNetworkConfig->getAttribute<int>(CONFIG_KEYWORD::TCP_CONNECTION_MONITOR_INTERVAL_TIME);

	while (isRunning() && !isInterruptionRequested())
	{
		boost::this_thread::sleep(boost::posix_time::milliseconds(IntervalTime));

		pTCPNode->monitorConnection();
	}
}