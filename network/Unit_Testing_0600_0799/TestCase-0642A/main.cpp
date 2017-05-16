#include <windows.h>
#include "NetworkInterface.h"
#include "IBDataRequestPacket.h"
#include "PacketExtraInfoOnMessage.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "MessageTestPacket.h"

//FUNCTION: detect the memory leak in DEBUG mode
void installMemoryLeakDeteor()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	//_CRTDBG_LEAK_CHECK_DF: Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks and generate an error 
	//report if the application failed to free all the memory it allocated. OFF: Do not automatically perform leak checking at program exit.
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//the following statement is used to trigger a breakpoint when memory leak happens
	//comment it out if there is no memory leak report;
	//_crtBreakAlloc = 20001;
#endif 
}

void setIBDataRequestPacketData(hiveInfiniband::CIBDataRequestPacket *&voIBDataRequestPacket);

int main(int, char**)
{
	installMemoryLeakDeteor();
	hiveCommon::hiveLoadTestingDLLs();

	try
	{
		if (hiveNetworkCommon::initNetwork("NetworkEnvSource.cfg", "InfinibandNetworkConfigParser"))
		{
			std::cout << " establish the connection successfully" << std::endl;
		}

		hiveInfiniband::CIBDataRequestPacket *IBDataRequestPacket = new hiveInfiniband::CIBDataRequestPacket();
		setIBDataRequestPacketData(IBDataRequestPacket);
		IBDataRequestPacket->postIBRecvRequest("IB");
		hiveNetworkCommon::sendPacket(IBDataRequestPacket, "UDP");
		

		Sleep(5 * 1000);

		hiveNetworkCommon::hiveStopNetwork();
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected Error");
	}
	return 0;
}

//**************************************************************************************
//FUCTION:
void setIBDataRequestPacketData(hiveInfiniband::CIBDataRequestPacket *&voIBDataRequestPacket)
{
	hiveNetworkCommon::CPacketExtraInfoOnMessage *PacketExtraInfoOnMessage = new hiveNetworkCommon::CPacketExtraInfoOnMessage;
	PacketExtraInfoOnMessage->setMessagePacketSig("MessageTestPacket");

	voIBDataRequestPacket->setPacketTargetPort(9999);
	voIBDataRequestPacket->setPacketSourcePort(8889);
	voIBDataRequestPacket->setPacketTargetIP("11.4.12.62");
	voIBDataRequestPacket->setPacketSourceIP("11.4.12.62");

	voIBDataRequestPacket->setSenderNodeName("IB");
	voIBDataRequestPacket->setReceiverNodeName("IB");
	voIBDataRequestPacket->setRecvBufferSize(1024*512);
	voIBDataRequestPacket->setReceiverQueuePairID(0);
	voIBDataRequestPacket->setProductCreationSig("IBDataRequestPacket");
	voIBDataRequestPacket->setContainExtraInfoFlag();
	voIBDataRequestPacket->setPacketExtraInfo(PacketExtraInfoOnMessage);
}