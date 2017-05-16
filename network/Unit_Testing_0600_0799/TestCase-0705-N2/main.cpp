#include <windows.h>
#include "Common/CommonInterface.h"
#include "Common/HiveCommonMicro.h"
#include "NetworkInterface.h"
#include "IBInterface.h"
#include "TCPIPInterface.h"
#include "MyIBPacket.h"

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

unsigned char* generateRandomBuffer(unsigned int vBufferSize)
{
	unsigned char *pBuffer = new unsigned char[vBufferSize];

	srand(0);
	for (unsigned int i = 0; i < vBufferSize; i++) pBuffer[i] = rand() % 256;
	return pBuffer;
}

int main()
{
	installMemoryLeakDetector();

	try
	{
		hiveInfiniband::hiveInitIB("Network.xml");
		if (!hiveNetworkCommon::hiveBuildConnection("192.168.18.4", 8001, "IB"))
		{
			std::cout << "Fail to build IB connection with 192.168.18.4." << std::endl;
		}
		else
		{
			std::cout << "Waiting respond from 192.168.18.4." << std::endl;
			while (!hiveNetworkCommon::hiveIsConnectionBuilt("192.168.18.4", "IB"))
			{
				Sleep(50);
			}
			std::cout << "Succeed to build IB connection with 192.168.18.4." << std::endl;

			hiveNetworkCommon::SSendOptions SendOption;
			SendOption.TargetIP = "192.168.18.4";
			SendOption.TargetPort = 8001;

			CMyIBPacket *pPacket = new CMyIBPacket("MyIBPacket");
			pPacket->setData(generateRandomBuffer(10000), 10000, true);
			hiveNetworkCommon::hiveSendPacket(pPacket, "IB", SendOption);
			delete pPacket;
		}

#ifdef _DEBUG
		int c = getchar();
#endif

		hiveNetworkCommon::hiveStopNetwork();
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}

	return 0;
}
