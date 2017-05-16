#include "../common/PhysicalNode.h"
#include "ServerPhysicalNode.h"
#include "ManageClientConnectionThread.h"
#include "TopologyCommon.h"

using namespace hiveNetworkTopology;

CManageClientConnectionThread::CManageClientConnectionThread()
{
	_setThreadName("CManageClientConnectionThread");
}

//**************************************************************************
//FUNCTION:
void CManageClientConnectionThread::__runV()
{
	while (isRunning())
	{
		boost::this_thread::interruption_point();

		boost::this_thread::sleep(boost::posix_time::milliseconds(MANAGECLIENT_INTERVAL_TIME));

		CServerPhysicalNode::getInstance()->monitorClientConnection();
	}
}