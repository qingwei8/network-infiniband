#include <windows.h>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "CSNetworkTopologyInterface.h"
#include "TopologyCommon.h"
#include "common/FileSystem.h"

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
	_crtBreakAlloc = 6001;
#endif
}

void printFileMD5Value(char* vFileName)
{
	std::cout << vFileName << " (MD5) : " << hiveCommon::hiveComputeMD5(std::string(vFileName)) << std::endl;
}

int main()
{
	using namespace hiveCSNetworkTopology;
	hiveCommon::hiveLoadTestingDLLs();
	installMemoryLeakDetector();

	try
	{
		initNetwork("NetworkEnv.xml", "SERVER");
		Sleep(10000);
		char pReceivedFile[4][30] = {"test1.log", "test2.log", "test3.log", "RoughRefr_I3D_Final.pdf"};
		while (1)
		{
			std::string FilePath = hiveCommon::CFileSystem::getInstance()->getCurrentPath();
			std::vector<std::string> LogFileName = hiveCommon::CFileSystem::getInstance()->getFileInDirectory(FilePath, ".log");
			std::vector<std::string> PDFFileName = hiveCommon::CFileSystem::getInstance()->getFileInDirectory(FilePath, ".pdf");
			if (LogFileName.empty() || PDFFileName.empty()) continue;
			for (int i=0; i<4; ++i)
			{
				printFileMD5Value(pReceivedFile[i]);
			}
			break;
		}

		while(1);
		stopNetwork();
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}

	return 0;
}