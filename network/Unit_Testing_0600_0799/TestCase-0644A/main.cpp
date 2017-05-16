#include <windows.h>
#include "../../common/Packet.h"
#include "NetworkInterface.h"
#include "PacketExtraInfoOnFile.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"

//FUNCTION: detect the memory leak in DEBUG mode
void installMemoryLeakDeteor()
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	//_CRTDBG_LEAK_CHECK_DF: Perform automatic leak checking at program exit through a call to _CrtDumpMemoryLeaks and generate an error 
	//report if the application failed to free all the memory it allocated. OFF: Do not automatically perform leak checking at program exit.
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	//the following statement is used to trigger a breakpoint when memory leak happens
	//comment it out if there is no memory leak report;
	//_crtBreakAlloc = 20001;
#endif 
}

void setPacketData(hiveNetworkCommon::IUserPacket *&voPacket);

int main(int, char**)
{
	installMemoryLeakDeteor();
	hiveCommon::hiveLoadTestingDLLs();

	try
	{
		if (hiveNetworkCommon::initNetwork("NetworkEnvSource.cfg", "InfinibandNetworkConfigParser"))
		{
			std::cout << " establish the connection successfully" << std::endl;
		}

		hiveNetworkCommon::IUserPacket *Packet = new hiveNetworkCommon::IUserPacket();
		setPacketData(Packet);
		hiveNetworkCommon::sendPacket(Packet,"IB");
		
		Sleep(6 * 1000);

		_SAFE_DELETE(Packet);
		hiveNetworkCommon::hiveStopNetwork();
	}
	catch (...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected Error");
	}
	return 0;
}

//**************************************************************************************
//FUCTION:
void setPacketData(hiveNetworkCommon::IUserPacket *&voPacket)
{
	voPacket->setPacketTargetPort(9999);
	voPacket->setPacketSourcePort(8889);
	voPacket->setPacketTargetIP("11.4.12.65");
	voPacket->setPacketSourceIP("11.4.12.65");

	voPacket->setProductCreationSig("Packet");
	voPacket->setContainExtraInfoFlag();

	hiveNetworkCommon::CPacketExtraInfoOnFile *PacketExtraInfoOnFile = new hiveNetworkCommon::CPacketExtraInfoOnFile;
	PacketExtraInfoOnFile->setFilePath("F:\\readme.txt");
	PacketExtraInfoOnFile->setProductCreationSig("CPacketExtraInfoOnFile");
	voPacket->setPacketExtraInfo(PacketExtraInfoOnFile);
}