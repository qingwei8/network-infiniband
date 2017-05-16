#include <windows.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "../../common/NetworkInterface.h"
#include "../../tcpip/TCPIPInterface.h"
#include "../../infiniband/IBInterface.h"
#include "TestPacket.h"
#include <numeric>

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
	//_crtBreakAlloc = 22976;
#endif
}

#define STEP_PACKET_SIZE 10000
#define INITIAL_PACKET_SIZE 100000000

//****************************************************************************
//FUNCTION:
char* generateRandomBuffer(unsigned int vBufferSize)
{
	char *pBuffer = new char[vBufferSize];

	for (unsigned int i=0; i<vBufferSize; i++) pBuffer[i] = rand() % 26 + 'a';
	pBuffer[vBufferSize-1] = 0;
	return pBuffer;
}

//****************************************************************************
//FUNCTION:
int main()
{
	try
	{
		srand(0);
		installMemoryLeakDetector();

		hiveInfiniband::hiveInitIB("Network.xml");
		if (hiveNetworkCommon::hiveBuildConnection("11.4.12.64", 8000, "IB"))
		{
			Sleep(1000);
			std::cout << "build Connection Succeed.." << std::endl;
		}

		hiveNetworkCommon::SSendOptions SendOption;
		SendOption.TargetIP = "11.4.12.64";
		SendOption.TargetPort = 8000;

		std::string Prompt;
		unsigned int BufferSize = INITIAL_PACKET_SIZE;
		unsigned int i = 0, PacketNum = 0;
		long double TotalPacketOfPast = 0.0;
		clock_t StartTime;
				
		while (true)
		{
			CTestPacket *pPacket = dynamic_cast<CTestPacket*>(hiveCommon::hiveCreateProduct("TESTPACKET_SIG")); //new CTestPacket(std::string("TESTPACKET_SIG"));
			char *pBuffer = generateRandomBuffer(BufferSize);
			pPacket->setMessage(std::string(pBuffer));
			//TotalPacketOfPast += pPacket->getMessageSize();
			pPacket->setTotalPacketOfPast(BufferSize);

			PacketNum = ++i;
			pPacket->setPacketNum(PacketNum);
			StartTime = clock();
			pPacket->setStartTime(StartTime);
			hiveNetworkCommon::hiveSendPacket(pPacket, "IB", SendOption);
// 			Prompt = _BOOST_STR1("Sending packet [%1%]...\n", pPacket->getMessageSize());
// 			std::cout << Prompt<< std::endl;
// 			std::cout << PacketNum << " packets" << std::endl;

			delete pBuffer;

// 			BufferSize += STEP_PACKET_SIZE;
// 			if(BufferSize >= INT_MAX) BufferSize = INT_MAX;

			Sleep(100);                  //NOTE: ����֮�����һ��ʱ�䣬���򷢰��ٶ�̫�죬�ᵼ�·����������ڴ漱������
		}

		Sleep(50000000);
		hiveNetworkCommon::hiveStopNetwork();
	}

	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}