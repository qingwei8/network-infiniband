#include <windows.h>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "../../common/NetworkInterface.h"
#include "../../tcpip/TCPIPInterface.h"
#include "TestPacket.h"
#include <sstream>

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
	//_crtBreakAlloc = 22976;
#endif
}

int main()
{
	int MAX_PACKET_NUMBER = 0;

	try
	{
		installMemoryLeakDetector();

		hiveTCPIP::hiveInitTCPIP("Network.xml");

		printf("Test multicast sender:\n");

		hiveNetworkCommon::SSendOptions SendOption;
		SendOption.TargetIP = "239.255.0.1";
		SendOption.TargetPort = 9000;
		SendOption.SendMode = hiveNetworkCommon::MULTICAST;

		while (true && MAX_PACKET_NUMBER <= 100)
		{
			CTestPacket *pPacket = new CTestPacket(std::string("TESTPACKET_SIG"));
			std::cout << MAX_PACKET_NUMBER << std::endl;
			std::stringstream SS;
			SS << MAX_PACKET_NUMBER++;
			pPacket->setMessage(SS.str() + "This is a test packet.....");
			hiveNetworkCommon::hiveSendPacket(pPacket, "UDP", SendOption);
			Sleep(10);                     //NOTE: 发包之后挂起一段时间，否则发包速度太快，会导致发包缓冲区内存急剧扩张
		}

		Sleep(90000000);
		hiveNetworkCommon::hiveStopNetwork();
	}

	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}