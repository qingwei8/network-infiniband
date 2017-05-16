#pragma once
#include "IPBasedNode.h"
#include "TCPIPExport.h"

namespace hiveNetworkCommon
{
	class IUserPacket;
}

namespace hiveTCPIP
{
	class TCPIP_DLL_EXPORT CUDPNode : public CIPBasedNode
	{
	public:
		CUDPNode();
		virtual ~CUDPNode();

	private:
		boost::asio::io_service *m_pIOService;  

		void __createThreads(const std::string& vLocalIP, int vLocalPort, unsigned char vUsage);
		void __configSocket(const SSocketInfo& vSocketInfo, boost::asio::ip::udp::socket* vioSocket);

		boost::asio::ip::udp::socket* __createAndOpenUDPSocket(const std::string& vSocketName, ESocketUsage vSocketUsage, const std::string& vIP, unsigned short vPort);

		virtual bool __extraInitV() override;
		virtual void __createNetworkThreadsV(std::vector<hiveNetworkCommon::INetworkThread*>& voOutput) override;
		virtual void __closeAndDestroyAllSocketsV() override;

	friend class CUDPDataSendingThread;
	};
}