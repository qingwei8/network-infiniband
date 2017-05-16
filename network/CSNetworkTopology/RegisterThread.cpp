#include "RegisterThread.h"
#include "RegisterMessage.h"
#include "TopologyCommon.h"
#include "common\CommonInterface.h"
#include "CSNetworkTopologyInterface.h"
#include "MasterInfoConfig.h"
#include "ClientPhysicalNode.h"

hiveNetworkTopology::CRegisterThread::CRegisterThread()
{
	_setThreadName("CRegisterThread");
}

//*********************************************************************************
//FUNCTION:
void hiveNetworkTopology::CRegisterThread::__runV()
{
	while (isRunning())
	{
		boost::this_thread::interruption_point();

		boost::this_thread::sleep(boost::posix_time::milliseconds(REGISTER_INTERVAL_TIME));

		hiveCommon::hiveOutputEvent("Sending the Register message");
#ifdef _DEBUG
		std::cout << "Sending the register message\n";
#endif

		hiveNetworkTopology::CRegisterMessage* pPacket = dynamic_cast<hiveNetworkTopology::CRegisterMessage*>(hiveCommon::hiveCreateProduct("MESSAGE_REGISTER"));

		pPacket->setUDPListeningPort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalUDPListeningPort());
		pPacket->setTCPListeningPort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalTCPListeningPort());

		pPacket->setPacketSourcePort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalUDPListeningPort());
		pPacket->setPacketTargetIP(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getMasterIP());
		pPacket->setPacketTargetPort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getMasterUDPListeningPort());

		if (CClientPhysicalNode::getInstance()->isKeepAliveOpen() == false)
			pPacket->setKeepAliveOpenFlag(false);

		hiveNetworkTopology::sendPacket(pPacket, TO_MASTER, "UDP");
	}
}
