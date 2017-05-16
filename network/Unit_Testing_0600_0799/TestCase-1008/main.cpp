#include <windows.h>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "common/ConfigInterface.h"
#include "NetworkInterface.h"
#include "IBCommon.h"
#include "TCPIPCommon.h"
#include "IBConfig.h"

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
		hiveInfiniband::CIBConfig NetworkConfig;
		hiveConfig::hiveParseConfig("Network.xml", hiveConfig::CONFIG_XML, &NetworkConfig);
		
		hiveConfig::CHiveConfig *pIBNodeConfig = NetworkConfig.fetchSubConfigAt(0);
		std::cout << "IP: " << NetworkConfig.getAttribute<std::string>(hiveNetworkCommon::CONFIG_KEYWORD::IP) << std::endl;
		std::cout << "IBNodeName: " << pIBNodeConfig->getName() << std::endl;
		std::cout << "Protocol: " << pIBNodeConfig->getAttribute<std::string>(hiveNetworkCommon::CONFIG_KEYWORD::PROTOCAL) << std::endl;
		std::cout << "IB_AUX_NODE: " << pIBNodeConfig->getAttribute<std::string>(hiveInfiniband::CONFIG_KEYWORD::IB_AUX_NODE) << std::endl;
		
		hiveConfig::CHiveConfig *pAuxNodeConfig = NetworkConfig.fetchSubConfigAt(1);		
		std::cout << "AuxNodeName: " << pAuxNodeConfig->getName() << std::endl;
		std::cout << "AuxNodeProtocol: " << pAuxNodeConfig->getAttribute<std::string>(hiveNetworkCommon::CONFIG_KEYWORD::PROTOCAL) << std::endl;
		
		hiveConfig::CHiveConfig *pSocketConfig = pAuxNodeConfig->fetchSubConfigAt(0);
		std::cout << "SocketName: " << pSocketConfig->getName() << std::endl;
		std::cout << "SocketPort: " << pSocketConfig->getAttribute<int>(hiveTCPIP::CONFIG_KEYWORD::PORT) << std::endl;

		system("pause");
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}

	return 0;
}