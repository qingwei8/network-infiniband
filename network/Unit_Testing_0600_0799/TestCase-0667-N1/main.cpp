#include <windows.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include <GL/glut.h>
#include <SOIL/SOIL.h>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "../../common/NetworkInterface.h"
#include "../../tcpip/TCPIPInterface.h"
#include "TestPacket.h"

//FUNCTION: detect the memory leak in DEBUG mode
void installMemoryLeakDetector()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	//_CRTDBG_LEAK_CHECK_DF: Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks and generate an error 
	//report if the application failed to free all the memory it allocated. OFF: Do not automatically perform leak checking at program exit.
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	//the following statement is used to trigger a breakpoint when memory leak happens
	//comment it out if there is no memory leak report;
	//_crtBreakAlloc = 32473;
#endif
}

//***********************************************************
//FUNCTION:
void initImageDataSet(std::vector<std::pair<unsigned char*, unsigned int> >& voImageDataSet)
{
	for (unsigned int i = 1; i < 250; i++)
	{
		std::string FileName = "./Test2/Test";
		FileName = FileName + std::to_string(i) + ".jpg";
		int Width = 0, Height = 0;

		unsigned char* pImageData = SOIL_load_image(FileName.c_str(), &Width, &Height, 0, SOIL_LOAD_RGB);
		_ASSERTE(pImageData);
		voImageDataSet.push_back(std::make_pair(pImageData, Width*Height*3));
	}
}

//****************************************************************************
//FUNCTION:
int main()
{
#ifdef _DEBUG
	hiveCommon::hiveSetDisplayAllEventsHint();
#endif

	try
	{
		installMemoryLeakDetector();

		hiveTCPIP::hiveInitTCPIP("Network.xml");
		if (hiveNetworkCommon::hiveBuildConnection("192.168.18.66", 8000)) std::cout << "Succeed to build connection." << std::endl;

		std::vector<std::pair<unsigned char*, unsigned int>> ImageDataSet;
		initImageDataSet(ImageDataSet);

		hiveNetworkCommon::SSendOptions SendOption;
		SendOption.TargetIP = "192.168.18.66";
		SendOption.TargetPort = 8000;

		for (unsigned int i = 0; i < ImageDataSet.size(); i++)
		{
			CTestPacket *pPacket = new CTestPacket("TESTPACKET_SIG");
			pPacket->setImageData(ImageDataSet[i]);
			hiveNetworkCommon::hiveSendPacket(pPacket, "TCP", SendOption);
		}

		Sleep(100000);
		hiveNetworkCommon::hiveStopNetwork();

		system("pause");
	}

	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}