#include <windows.h>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "CSNetworkTopologyInterface.h"
#include "TopologyCommon.h"
#include "ConnectionUpdateInfo.h"

using namespace hiveCSNetworkTopology;

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

void printQueryConnectionUpdateInfo(CConnectionUpdateInfo& vInfo)
{
	std::cout<<std::endl;
	if (!vInfo.isUpdated())
	{
		std::cout<<"Connection has not been updated since the last query"<<std::endl;
		return;
	}

	std::cout<<"Connection has been updated since the last query"<<std::endl;

	std::vector<SConnectionInfo*> AddConnectionInfoOutPut = vInfo.getAddedConnectionInfo();
	if (AddConnectionInfoOutPut.size()>0)
	{
		std::cout<<"[Added Connection] : " <<std::endl;
		for (unsigned int i=0; i<AddConnectionInfoOutPut.size(); i++)
		{
			std::cout<<"	[Client Info] "<<std::endl;
			SConnectionInfo* Info = AddConnectionInfoOutPut[i];
			std::cout<<"	Client IP:      "<<Info->ConnectionIP<<std::endl;
			std::cout<<"	Client TCPPort: "<<Info->ConnectionTCPPort<<std::endl;
			std::cout<<"	Client UDPPort: "<<Info->ConnectionUDPPort<<std::endl;
			std::cout<<"	Update Time:    "<<Info->ConnectionUpdateTime<<std::endl;
		}
	}

	std::vector<SConnectionInfo*> DeleteConnectionInfoOutPut = vInfo.getDeletedConnectionInfo();
	if (DeleteConnectionInfoOutPut.size()>0)
	{
		std::cout<<"[Deleted Connection] : " <<std::endl;
		for (unsigned int i=0; i<DeleteConnectionInfoOutPut.size(); i++)
		{
			std::cout<<"	[Client Info] "<<std::endl;
			SConnectionInfo* Info = DeleteConnectionInfoOutPut[i];
			std::cout<<"	Client IP:      "<<Info->ConnectionIP<<std::endl;
			std::cout<<"	Client TCPPort: "<<Info->ConnectionTCPPort<<std::endl;
			std::cout<<"	Client UDPPort: "<<Info->ConnectionUDPPort<<std::endl;
			std::cout<<"	Update Time:    "<<Info->ConnectionUpdateTime<<std::endl;
		}
	}
}

int main()
{
	hiveCommon::hiveLoadTestingDLLs();
	installMemoryLeakDetector();

	try
	{
		initNetwork("Network.xml", "SERVER");
		Sleep(15000);
		CConnectionUpdateInfo ConnectUpdateInfo[3];
		queryConnectionUpdate(ConnectUpdateInfo[0]);	
		Sleep(40000);
		queryConnectionUpdate(ConnectUpdateInfo[1]);
		Sleep(5000);
		queryConnectionUpdate(ConnectUpdateInfo[2]);
		for (int i=0; i<3; i++)
		{
			printQueryConnectionUpdateInfo(ConnectUpdateInfo[i]);
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