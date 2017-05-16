#include <windows.h>
#include "Common/CommonInterface.h"
#include "Common/HiveCommonMicro.h"
#include "NetworkInterface.h"
#include "IBInterface.h"
#include "IBPacket.h"

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
	//_crtBreakAlloc = 23230;
#endif
}

unsigned char* generateRandomBuffer(unsigned int vBufferSize)
{
	unsigned char *pBuffer = new unsigned char[vBufferSize];

	for (unsigned int i = 0; i < vBufferSize; i++) pBuffer[i] = rand() % 256;
	return pBuffer;
}

int main()
{
#ifdef _DEBUG
	hiveCommon::hiveSetDisplayAllEventsHint();
#endif
	installMemoryLeakDetector();
	srand(0);

	try
	{
		hiveInfiniband::hiveInitIB("Network.xml");
		if (!hiveNetworkCommon::hiveBuildConnection("192.168.18.4", 8001, "IB"))
		{
			std::cout << "Fail to build IB connection with 192.168.18.4." << std::endl;
		}
		else
		{
			std::cout << "Waiting respond from 192.168.18.4." << std::endl;
			while (!hiveNetworkCommon::hiveIsConnectionBuilt("192.168.18.4", "IB"))
			{
				Sleep(50);
			}
			std::cout << "Succeed to build IB connection with 192.168.18.4." << std::endl;

			hiveNetworkCommon::SSendOptions SendOption;
			SendOption.TargetIP = "192.168.18.4";
			SendOption.TargetPort = 8001;

			unsigned int Counter = 0;
			while (Counter < 210)
			{
				hiveInfiniband::CIBPacket *pPacket = new hiveInfiniband::CIBPacket;
				pPacket->setData(generateRandomBuffer(10000), 10000, true);
				hiveNetworkCommon::hiveSendPacket(pPacket, "IB", SendOption);
				delete pPacket;
				Counter++;
			}
		}

		int c = getchar();  //NOTE: ���к���Ҫ������������whileѭ����ʵ��ֻ�ǽ����еķ��������ύ��IB��������ִ�е�����ʱ��ʵ�ʺܿ���IB��δ������а��ķ��ͣ���������൱�����ڵȴ�IB������з��ͣ����Ը�����Ļ��ʾ���ж�IB�Ƿ���ɷ��ͣ�

		hiveNetworkCommon::hiveStopNetwork();
	}
	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}

	return 0;
}