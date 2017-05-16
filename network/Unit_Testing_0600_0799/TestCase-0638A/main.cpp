#include <windows.h>
#include "NetworkInterface.h"
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
	//_crtBreakAlloc = 46672;
#endif
}

int main()
{
	installMemoryLeakDetector();

	hiveCommon::hiveLoadTestingDLLs();

	try
	{
		if (hiveNetworkCommon::initNetwork("NetworkEnv.cfg","InfinibandNetworkConfigParser"))
		{
			hiveCommon::hiveOutputEvent("Success to init network!", hiveCommon::WARNING_LEVEL);
		}
		else
		{
			hiveCommon::hiveOutputEvent("Failed to init network!", hiveCommon::WARNING_LEVEL);
		}

		hiveNetworkCommon::hiveStopNetwork();
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected Error");
	}

	return 0;
}