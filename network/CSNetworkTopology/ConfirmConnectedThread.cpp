#include "ConfirmConnectedThread.h"
#include "TopologyCommon.h"
#include "ClientPhysicalNode.h"
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"

hiveNetworkTopology::CConfirmConnectedThread::CConfirmConnectedThread()
{
	_setThreadName("ConfirmConnectedThread");
}

//**************************************************************************************
//FUCTION:
void hiveNetworkTopology::CConfirmConnectedThread::__runV()
{
	while (isRunning())
	{
		boost::this_thread::interruption_point();

		boost::this_thread::sleep(boost::posix_time::milliseconds(CONFIRMCONNECTED_INTERVAL_TIME));

		time_t TimeDiff = time(0) - CClientPhysicalNode::getInstance()->getLastConfirmConnectionTimeStamp();

		if (TimeDiff > LASTCONNECTED_CURRENT_THRESHOLD_INTERVAL_TIME) 
		{
			hiveCommon::hiveOutputEvent("Connection lost, trying to re-register");
			
#ifdef _DEBUG
		std::cout << "¶ÏÏßÖØÐÂ×¢²á\n";
#endif
		CClientPhysicalNode::getInstance()->__stopKeepAliveThread();
		CClientPhysicalNode::getInstance()->__startRegisterThread();

		interruptThread();
		break;
		}
	}
}
