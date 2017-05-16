#pragma once
#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>
#include "IPBasedNode.h"
#include "TCPIPExport.h"

namespace hiveNetworkCommon
{
	class INetworkThread;
}

namespace hiveTCPIP
{	
	class TCPIP_DLL_EXPORT CTCPNode : public CIPBasedNode
	{
	public:
		CTCPNode();
		virtual ~CTCPNode();

		boost::asio::ip::tcp::socket* findSocket(const std::string& vRemoteIP, unsigned int vRemotePort);

		void monitorConnection();

		unsigned int getNumIncommingConnection();

		void getPhyical();

	private:
		virtual bool __extraInitV() override;
		virtual void __createNetworkThreadsV(std::vector<hiveNetworkCommon::INetworkThread*>& voOutput) override;
		virtual void __closeAndDestroyAllSocketsV() override;
	};
}