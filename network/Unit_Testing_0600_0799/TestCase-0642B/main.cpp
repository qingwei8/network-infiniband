#include <windows.h>
#include "NetworkNode.h"
#include "NetworkInterface.h"
#include "NetworkController.h"
#include "IBDataRequestPacket.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"

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
	//_crtBreakAlloc = 20658;
#endif 
}

int main(int, char**)
{
	installMemoryLeakDeteor();

	hiveCommon::hiveLoadTestingDLLs();

	try
	{
		if (hiveNetworkCommon::initNetwork("NetworkEnvTarget.cfg"))
		{
			std::cout << "IB Nodes Initialize successfully" << std::endl;
		}

		Sleep(10 * 1000);

		hiveNetworkCommon::hiveStopNetwork();
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected Error");
	}
	return 0;
}