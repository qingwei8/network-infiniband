#include <windows.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "../../common/NetworkInterface.h"
#include "../../tcpip/TCPIPInterface.h"
#include "TestPacket1.h"

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

#define MAX_PACKET_SIZE 100000

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
	hiveCommon::hiveLoadTestingDLLs();

	try
	{
		srand(0);
		installMemoryLeakDetector();

		hiveTCPIP::hiveInitTCPIP("Network.xml");
		
		hiveNetworkCommon::SSendOptions SendOption;
		SendOption.TargetIP = "192.168.18.132";
		SendOption.TargetPort = 8000;

		unsigned int ScaleFactor = MAX_PACKET_SIZE / RAND_MAX;
		_ASSERT(ScaleFactor > 1);

		std::string Prompt;
		unsigned int BufferSize;

		while (true)
		{
			CTestPacket1 *pPacket = new CTestPacket1(std::string("TESTPACKET1_SIG"));
			BufferSize = rand();
			if (rand() % 3 != 0) BufferSize *= ScaleFactor;
			if (BufferSize > MAX_PACKET_SIZE) BufferSize = MAX_PACKET_SIZE;
			char *pBuffer = generateRandomBuffer(BufferSize);
			pPacket->setMessage(std::string(pBuffer));
			hiveNetworkCommon::hiveSendPacket(pPacket, "UDP", SendOption);
		
			_ASSERT(pPacket->getMessageSize() <= MAX_PACKET_SIZE);
			Prompt = _BOOST_STR1("Sending packet [%1%]...\n", pPacket->getMessageSize());
			std::cout << Prompt;

			delete pBuffer;

			Sleep(50);                     //NOTE: 发包之后挂起一段时间，否则发包速度太快，会导致发包缓冲区内存急剧扩张
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