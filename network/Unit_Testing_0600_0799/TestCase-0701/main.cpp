#include "Common/HiveCommonMicro.h"
#include "Common/CommonInterface.h"
#include "IBInterface.h"
#include "PhysicalNode.h"

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
		hiveInfiniband::hiveInitIB("Network.xml");
		hiveNetworkCommon::CPhysicalNode* pPhysicalNode = hiveNetworkCommon::CPhysicalNode::getInstance();
		if (pPhysicalNode->findLogicalNodeByProtocal("IB") && pPhysicalNode->findLogicalNodeByProtocal("TCP"))
			std::cout << "Succeed to create IB and its auxiliary node (TCP).\n";
		else
			std::cout << "Fail to create IB and its auxiliary node (TCP).\n";

		system("Pause");
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}

	return 0;
}