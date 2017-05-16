#include "TimingSendPacketThread.h"
#include "KeepAliveMessage.h"
#include "TopologyCommon.h"
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "CSNetworkTopologyInterface.h"
#include "MasterInfoConfig.h"
#include "ClientPhysicalNode.h"
#include <boost\format.hpp>

hiveNetworkTopology::CTimingSendPacketThread::CTimingSendPacketThread()
{
	_setThreadName("CTimingSendPacketThread");
}

//*********************************************************************************
//FUNCTION:
void hiveNetworkTopology::CTimingSendPacketThread::__runV()
{
	static int Count = 0;
	std::multimap<int, std::string> SigSets = CClientPhysicalNode::getInstance()->getSendPacketSigSets();

	if (SigSets.size() == 0)
		return;

	int MaxTime = SigSets.rbegin()->first;
	__sendAllPacketFirstTime(SigSets);

	while (isRunning())
	{
		boost::this_thread::interruption_point();

		boost::this_thread::sleep(boost::posix_time::milliseconds(1000));
		Count++;

		for (auto itr = SigSets.begin(); itr!=SigSets.end(); itr++)
		{
			if (Count / (itr->first) > 0 && Count%itr->first == 0)
			{
				if (itr->second == "MESSAGE_KEEPALIVE")
					__sendKeepAliveMessage();
				else
					__sendConfigPacket(itr->second);
			}
		}
		if (MaxTime == Count)
			Count = 0;
	}
}

//*********************************************************************************
//FUNCTION:
void hiveNetworkTopology::CTimingSendPacketThread::__sendKeepAliveMessage()
{
	/*
#ifdef _DEBUG
		std::cout << "Sending the KeepAlive message"<<std::endl;
#endif
	*/
		hiveNetworkTopology::CKeepAliveMessage* pPacket = dynamic_cast<hiveNetworkTopology::CKeepAliveMessage*>(hiveCommon::hiveCreateProduct("MESSAGE_KEEPALIVE"));

		pPacket->setTCPListeningPort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalTCPListeningPort());
		pPacket->setUDPListeningPort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalUDPListeningPort());
		pPacket->setPacketSourcePort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalUDPListeningPort());
		pPacket->setPacketTargetIP(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getMasterIP());
		pPacket->setPacketTargetPort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getMasterUDPListeningPort());

		hiveNetworkTopology::sendPacket(pPacket, TO_MASTER, "UDP");
}

//*********************************************************************************
//FUNCTION:
void hiveNetworkTopology::CTimingSendPacketThread::__sendAllPacketFirstTime(const std::multimap<int, std::string>& vSendPacketSets)
{
	for (auto itr = vSendPacketSets.begin(); itr!=vSendPacketSets.end(); itr++)
	{
		if (itr->second == "MESSAGE_KEEPALIVE")
			__sendKeepAliveMessage();
		else
			__sendConfigPacket(itr->second);
	}
}

//*********************************************************************************
//FUNCTION:
void hiveNetworkTopology::CTimingSendPacketThread::__sendConfigPacket(const std::string& vSig)
{
	hiveNetworkCommon::IUserPacket* pPacket = dynamic_cast<hiveNetworkCommon::IUserPacket*>(hiveCommon::hiveCreateProduct(vSig));
	if (pPacket)
	{
		pPacket->setPacketSourcePort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getLocalUDPListeningPort());
		pPacket->setPacketTargetIP(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getMasterIP());
		pPacket->setPacketTargetPort(hiveNetworkTopology::CMasterInfoConfig::getInstance()->getMasterUDPListeningPort());

		hiveNetworkTopology::sendPacket(pPacket, TO_MASTER, "UDP");
		/*
#ifdef _DEBUG
		std::cout<<"send packet sig: "<<itr->second;
#endif
		*/
	}
	else
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("fail to create packet sig [%1%] by Product!", vSig));
}


