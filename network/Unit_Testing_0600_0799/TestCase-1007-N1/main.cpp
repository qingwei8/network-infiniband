#include <windows.h>
#include <vector>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "NetworkInterface.h"
#include "TestPacket.h"
#include "TCPIPInterface.h"

void installMemoryLeakDetector()
{

#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

int main()
{
	installMemoryLeakDetector();

	try
	{
		hiveTCPIP::hiveInitTCPIP("Network.xml");
		if(hiveNetworkCommon::hiveBuildConnection("192.168.18.4", 8000)) std::cout << "Succeed to build connection." << std::endl;
		hiveNetworkCommon::SSendOptions SendOption;
		SendOption.TargetIP = "192.168.18.4";

		std::vector<ib_net32_t> ReqQueuePairIDSet;

		while (true){
			CTestPacket *pPacket = new CTestPacket(std::string("TESTPACKET_SIG"));

			ReqQueuePairIDSet.clear();
			ReqQueuePairIDSet.push_back(1);
			pPacket->setReqQueuePairIDSet(ReqQueuePairIDSet);
			
			hiveNetworkCommon::hiveSendPacket(pPacket, "TCP", SendOption);
			Sleep(100);
		}

		Sleep(5000);
		hiveNetworkCommon::hiveStopNetwork();

		system("pause");
	}

	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexcepted error");
	}
	return 0;
}