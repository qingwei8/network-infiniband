#include <windows.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "../../common/NetworkInterface.h"
#include "../../tcpip/TCPIPInterface.h"

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
	//_crtBreakAlloc = 32473;
#endif
}

//****************************************************************************
//FUNCTION:
int main()
{
#ifdef _DEBUG
	hiveCommon::hiveSetDisplayAllEventsHint();
#endif

	try
	{
		installMemoryLeakDetector();

		hiveTCPIP::hiveInitTCPIP("Network.xml");		
		Sleep(3000000);

		hiveNetworkCommon::hiveStopNetwork();
	}

	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}