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

void printFileMD5Value(char* vFileName)
{
	std::cout << vFileName << " (MD5) : " << hiveCommon::hiveComputeMD5(std::string(vFileName)) << std::endl;
}

int main()
{
	hiveCommon::hiveLoadTestingDLLs();
	installMemoryLeakDetector();
	try
	{
		hiveNetworkCommon::initNetwork("NetworkEnv.cfg");

		hiveNetworkCommon::buildConnection("127.0.0.1", 2000);

		hiveNetworkCommon::IUserPacket* pPacket = dynamic_cast<hiveNetworkCommon::IUserPacket*>(hiveCommon::hiveCreateProduct("TESTPACKET_SIG"));
		pPacket->setPacketTargetIP("127.0.0.1");
		pPacket->setPacketTargetPort(2000);
		dynamic_cast<CTestPacket*>(pPacket)->setMessage("This is test packet!");
		hiveNetworkCommon::sendPacket(pPacket);
		Sleep(1000);
		
		char pSendFile[4][30] = {"test1.log", "test2.log", "test3.log", "RoughRefr_I3D_Final.pdf"};
		//test1.log刚好分成一个片
		//test2.log 大于一个片
		//test3.log 小于一个片
		//RoughRefr_I3D_Final.pdf 大文件
		
		for (int i=0; i<4; ++i)
		{
			hiveNetworkCommon::sendFile(pSendFile[i], "127.0.0.1", 2000);
			printFileMD5Value(pSendFile[i]);
			Sleep(1000);
		}

		Sleep(1000);

		hiveNetworkCommon::hiveStopNetwork();
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}