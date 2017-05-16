#include <windows.h>
#include "PacketExtraInfo.h"
#include "PacketExtraInfoOnFile.h"
#include "NetworkInterface.h"
#include "IBDataRequestPacket.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"

//FUNCTION: detect the memory leak in DEBUG mode
void installMemoryLeakDetector()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	//_CRTDBG_LEAK_CHECK_DF: Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks and generate an error 
	//report if the application failed to free all the memory it allocated. OFF: Do not automatically perform leak checking at program exit.
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//the following statement is used to trigger a breakpoint when memory leak happens
	//comment it out if there is no memory leak report;
	//_crtBreakAlloc = 65297;
#endif
}

void setIBDataRequestPacketData(hiveInfiniband::CIBDataRequestPacket *&voIBDataRequestPacket);

int main()
{
	installMemoryLeakDetector();

	hiveCommon::hiveLoadTestingDLLs();

	try
	{
		if(hiveNetworkCommon::initNetwork("NetworkEnv.cfg","InfinibandNetworkConfigParser"))
		{
			hiveCommon::hiveOutputEvent("Success to init network!", hiveCommon::WARNING_LEVEL);
		}
		else
		{
			hiveCommon::hiveOutputEvent("Failed to init network!", hiveCommon::WARNING_LEVEL);
		}

		hiveInfiniband::CIBDataRequestPacket *pIBDataRequestPacket = new hiveInfiniband::CIBDataRequestPacket();
		setIBDataRequestPacketData(pIBDataRequestPacket);
		pIBDataRequestPacket->postIBRecvRequest("IB");

		hiveNetworkCommon::sendPacket(pIBDataRequestPacket, "UDP");
		
		Sleep(5*1000);

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
	voIBDataRequestPacket->setPacketTargetPort(9999);
	voIBDataRequestPacket->setPacketSourcePort(8889);
	voIBDataRequestPacket->setPacketTargetIP("11.4.12.65");
	voIBDataRequestPacket->setPacketSourceIP("11.4.12.65");

	voIBDataRequestPacket->setSenderNodeName("IB");
	voIBDataRequestPacket->setReceiverNodeName("IB");
	voIBDataRequestPacket->setRecvBufferSize(1024*512);
	voIBDataRequestPacket->setReceiverQueuePairID(0);
	voIBDataRequestPacket->setProductCreationSig("IBDataRequestPacket");

	hiveNetworkCommon::CPacketExtraInfoOnFile *pPacketExtraInfoOnFile = new hiveNetworkCommon::CPacketExtraInfoOnFile();
	pPacketExtraInfoOnFile->setFilePath("F:\\readme.txt");
	pPacketExtraInfoOnFile->setProductCreationSig("CPacketExtraInfoOnFile");
	
	voIBDataRequestPacket->setContainExtraInfoFlag();
	voIBDataRequestPacket->setPacketExtraInfo(pPacketExtraInfoOnFile);
}