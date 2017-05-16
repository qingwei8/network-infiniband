#include <windows.h>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "../../common/NetworkInterface.h"
#include "../../tcpip/TCPIPInterface.h"
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
	//_crtBreakAlloc = 8272;
#endif
}

int main()
{
	installMemoryLeakDetector();

	try
	{
		hiveCommon::hiveLoadTestingDLLs();

		hiveTCPIP::hiveInitTCPIP("Network.xml");
		if (hiveNetworkCommon::hiveBuildConnection("192.168.18.4", 8000)) std::cout << "Succeed to build connection." << std::endl;

		hiveNetworkCommon::SSendOptions SendOption;
		SendOption.TargetIP = "192.168.18.4";
		SendOption.TargetPort = 8000;

		while (true)
		{
			CTestPacket *pPacket = new CTestPacket(std::string("TESTPACKET_SIG"));
			pPacket->setMessage("This is a test packet.0660N2");
			hiveNetworkCommon::hiveSendPacket(pPacket, "TCP", SendOption);
			Sleep(10);                     //NOTE: ����֮�����һ��ʱ�䣬���򷢰��ٶ�̫�죬�ᵼ�·����������ڴ漱������
		}

		Sleep(1000000);
		hiveNetworkCommon::hiveStopNetwork();

		system("pause");

	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}