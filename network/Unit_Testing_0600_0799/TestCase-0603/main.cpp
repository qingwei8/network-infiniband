#include <windows.h>
#include "NetworkInterface.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "TestPacket.h"
#include <fstream>
#include <string>

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
	//_crtBreakAlloc = 6473;
#endif
}

char* ReadFile(char* vFileName)
{
	FILE *pFile;
	char *pContent = NULL;
	int Count = 0;
	if (vFileName != NULL)
	{
		errno_t ErrorT;
		ErrorT = fopen_s(&pFile, vFileName, "rt");
		if (pFile != NULL)
		{
			fseek(pFile, 0, SEEK_END);
			Count = ftell(pFile);
			rewind(pFile);
			if (Count > 0)
			{
				pContent = (char *)malloc(sizeof(char)*(Count + 1));
				Count = fread(pContent, sizeof(char), Count, pFile);
				pContent[Count] = '\0';
			}
			fclose(pFile);
		}
	}
	return pContent;
}

int main()
{
	hiveCommon::hiveLoadTestingDLLs();
	installMemoryLeakDetector();
	try
	{
		hiveNetworkCommon::initNetwork("NetworkEnv.cfg");
		char *pFileContent0 = ReadFile("test0.txt");
		char *pFileContent1 = ReadFile("test1.txt");
		std::string TestPacketSet[2];
 		TestPacketSet[0] = std::string(pFileContent0);
		TestPacketSet[1] = std::string(pFileContent1);
		delete pFileContent0; pFileContent0 = NULL;
		delete pFileContent1; pFileContent1 = NULL;

		for (int i=0; i<2; ++i)
		{
			hiveNetworkCommon::IUserPacket* pPacket = dynamic_cast<hiveNetworkCommon::IUserPacket*>(hiveCommon::hiveCreateProduct("TESTPACKET_SIG"));
			pPacket->setPacketTargetIP("127.0.0.1");
			pPacket->setPacketTargetPort(2000);
			dynamic_cast<CTestPacket*>(pPacket)->setMessage(TestPacketSet[i]);
			hiveNetworkCommon::buildConnection("127.0.0.1", 2000);
			hiveNetworkCommon::sendPacket(pPacket);
			Sleep(1000);
		}
		Sleep(5000);
		hiveNetworkCommon::hiveStopNetwork();
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}