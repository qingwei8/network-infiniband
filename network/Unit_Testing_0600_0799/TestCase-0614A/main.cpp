#include <windows.h>
#include "NetworkInterface.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "TestPacket.h"

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
	//_crtBreakAlloc = 2123;
#endif
}


void sendPackets(int vBufferSize, int vPacketNum)
{
	for (int i=0; i<vPacketNum; ++i)
	{
		hiveNetworkCommon::IUserPacket* pPacket = dynamic_cast<hiveNetworkCommon::IUserPacket*>(hiveCommon::hiveCreateProduct("TESTPACKET_SIG"));
		pPacket->setPacketTargetIP("127.0.0.1");
		pPacket->setPacketTargetPort(2000);
		pPacket->setSendMode(hiveNetworkCommon::ESendMode::UNICAST);
		dynamic_cast<CTestPacket*>(pPacket)->creatBuffer(vBufferSize);
		dynamic_cast<CTestPacket*>(pPacket)->setAllPacketsNum(vPacketNum);

		hiveNetworkCommon::sendPacket(pPacket);
	}
}

int main()
{
	hiveCommon::hiveLoadTestingDLLs();
	installMemoryLeakDetector();
	try
	{
		hiveNetworkCommon::initNetwork("NetworkEnv.cfg");

		// CTestPackt本身的序列化后大小为148	
		// 分片大小为102400
		hiveNetworkCommon::buildConnection("127.0.0.1", 2000);
		sendPackets(148-148, 10);
 		sendPackets(1024-148, 10);
		sendPackets(102400-148, 10);
		sendPackets(102400-148 + 1, 10);
		sendPackets(102400*3 - 148, 10);
 		sendPackets(10000000, 10);

		Sleep(10000);
		hiveNetworkCommon::hiveStopNetwork();
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
		hiveNetworkCommon::hiveStopNetwork();
	}
	return 0;
}