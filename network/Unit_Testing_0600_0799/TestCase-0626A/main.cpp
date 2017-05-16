#include <windows.h>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "CSNetworkTopologyInterface.h"
#include "TestPacket.h"
#include "TopologyCommon.h"

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
	//_crtBreakAlloc = 6001;
#endif
}

int main()
{
	using namespace hiveCSNetworkTopology;
	hiveCommon::hiveLoadTestingDLLs();
	installMemoryLeakDetector();
	try
	{
		initNetwork("NetworkEnv.xml", "CLIENT", "MasterInfo.xml");
		Sleep(10000);
		CTestPacket* pPacket = dynamic_cast<CTestPacket*>(hiveCommon::hiveCreateProduct("TESTPACKET_SIG"));
		pPacket->setPacketTargetIP("127.0.0.1");
		dynamic_cast<CTestPacket*>(pPacket)->setMessage("This is test TCP packet!");
		hiveCSNetworkTopology::sendPacket(pPacket, hiveCSNetworkTopology::TO_MASTER);
		while (1);
		stopNetwork();
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}

	return 0;
}