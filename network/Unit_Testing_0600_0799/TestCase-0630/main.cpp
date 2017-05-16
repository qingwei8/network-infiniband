#include "NetworkInterface.h"
#include "NetworkConfigParser_TXT.h"
#include "PhysicalNodeConfig.h"
#include "TCPIPCommon.h"
#include "IBCommon.h"
#include "IBNodeConfig.h"
#include "common/CommonInterface.h"
#include <windows.h>

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
	//_crtBreakAlloc = 3017;
#endif
}

/*
* Note: This is a client. It used to send data request and receive data form server node.
*/

int main(int, char**)
{
	installMemoryLeakDetector();

	hiveCommon::hiveLoadTestingDLLs();

	try
	{
		hiveNetworkCommon::CNetworkConfigParser_TXT *pConfigParser = dynamic_cast<hiveNetworkCommon::CNetworkConfigParser_TXT*>(hiveCommon::hiveGetOrCreateProduct("InfinibandNetworkConfigParser"));
		if (pConfigParser)
		{
			hiveNetworkCommon::CPhysicalNodeConfig PhysicalNodeConfig;
			pConfigParser->parseConfig("NetworkEnv.cfg", &PhysicalNodeConfig);

			const hiveTCPIP::CIPBasedNetworkNodeConfig* pUDPNodeConfig    = dynamic_cast<const hiveTCPIP::CIPBasedNetworkNodeConfig*>(PhysicalNodeConfig.getSubConfigAt(0));
			const hiveTCPIP::SSocketConfig UDPSocketConfig                = pUDPNodeConfig->getSocketConfigAt(0);
			const hiveInfiniband::CIBNodeConfig* pIBNodeConfig            = dynamic_cast<const hiveInfiniband::CIBNodeConfig*>(PhysicalNodeConfig.getSubConfigAt(1));
			const hiveInfiniband::SIBConnectionConfig& IBConnectionConfig = pIBNodeConfig->getOutgoingConnConfigAt(0);
		
			const std::string& UDPNodeName = pUDPNodeConfig->getName();
			const std::string& IBNodeName  = pIBNodeConfig->getName();
			const std::string& AuxNodeName = pIBNodeConfig->getAuxNodeName();

			std::string NetworkLocalIP    = PhysicalNodeConfig.getLocalIP();
			std::string SocketName        = UDPSocketConfig.SocketName;
			std::string ConnectionName    = IBConnectionConfig.ConnectionName;
			std::string RemoteIP          = IBConnectionConfig.RemoteIP;
			std::string RemoteIBNodeName  = IBConnectionConfig.RemoteIBNodeName;
			std::string CQHandleThreadSig = IBConnectionConfig.CompletionQueueHandleThreadSig;

			int LocalPort       = UDPSocketConfig.LocalPort;			
			int AuxNodeTargetPort = IBConnectionConfig.AuxNodeTargetPort;
			int AuxNodeSourcePort = IBConnectionConfig.AuxNodeSourcePort;

			unsigned char Usage       = UDPSocketConfig.Usage;
			unsigned int NumQueuePair = IBConnectionConfig.NumQueuePair;
			
			std::cout << "NetworkLocalIP  : " << NetworkLocalIP << std::endl;
			std::cout << "UDPNodeName     : " << UDPNodeName << std::endl;
			std::cout << "SocketName      : " << SocketName <<std::endl;
			std::cout << "LocalPort       : " << LocalPort << std::endl;
			std::cout << "Usage           : " << Usage << std::endl;
			std::cout << "IBNodeName      : " << IBNodeName << std::endl;
			std::cout << "AuxNodeName     : " << AuxNodeName << std::endl;
			std::cout << "ConnectionName  : " << ConnectionName << std::endl;
			std::cout << "RemoteIP        : " << RemoteIP << std::endl;
			std::cout << "AuxNodeTargetPort : " << AuxNodeTargetPort << std::endl;
			std::cout << "AuxNodeSourcePort : " << AuxNodeSourcePort << std::endl;
			std::cout << "RemoteIBNodeName: " << RemoteIBNodeName << std::endl;
			std::cout << "NumQueuePair    : " << NumQueuePair << std::endl;
			std::cout << "CompletionQueueHandleThreadSig: " << CQHandleThreadSig << std::endl;
		}
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected Error");
	}
	return 0;
}