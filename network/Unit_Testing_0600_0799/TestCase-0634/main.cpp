#include "IBCommon.h"
#include "IBChannelAdapter.h"
#include "IBMemoryBlockPool.h"
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
	//_crtBreakAlloc = 1392;
#endif
}

int main()
{
	installMemoryLeakDetector();

	hiveCommon::hiveLoadTestingDLLs();

	try
	{
		hiveInfiniband::CIBChannelAdapter  *IBChannelAdapter  = new hiveInfiniband::CIBChannelAdapter();
		hiveInfiniband::CIBMemoryBlockPool *IBMemoryBlockPool = new hiveInfiniband::CIBMemoryBlockPool();

		IBChannelAdapter->open();

		IBMemoryBlockPool->init(IBChannelAdapter->getProtectionDomain(), 1, hiveInfiniband::DEFAULT_MEMORY_BLOCK_SIZE, IB_AC_LOCAL_WRITE);
		IBMemoryBlockPool->requestMemoryBlock(hiveInfiniband::DEFAULT_MEMORY_BLOCK_SIZE*2, IB_AC_LOCAL_WRITE, 0, hiveInfiniband::WorkRequestType::SEND_TYPE);

		IBMemoryBlockPool->closeIBMemoryBlockPool();
		IBChannelAdapter->close();
		_SAFE_DELETE(IBMemoryBlockPool);
		_SAFE_DELETE(IBChannelAdapter);
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected Error");
	}

	return 0;
}