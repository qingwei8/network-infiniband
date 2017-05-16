#include <windows.h>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "NetworkInterface.h"
#include "TCPIPInterface.h"

void installMemoryLeakDetector()
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
}

int main()
{
#ifdef _DEBUG
	hiveCommon::hiveSetDisplayAllEventsHint();
#endif

	try
	{
		installMemoryLeakDetector();

		hiveTCPIP::hiveInitTCPIP("Network.xml");		
		Sleep(3000);

		hiveNetworkCommon::hiveStopNetwork();
	}

	catch(...)
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The program is terminated due to unexpected error");
	}
	return 0;
}