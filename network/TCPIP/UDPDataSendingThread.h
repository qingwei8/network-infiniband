#pragma once
#include "IPBasedDataSendingThread.h"
#include <boost/asio.hpp>

namespace hiveNetworkCommon
{
	class IUserPacket;
	class CLogicalNode;
}

namespace hiveTCPIP
{
	class CUDPDataSendingThread : public IIPBasedDataSendingThread
	{
	public:
		CUDPDataSendingThread();
		CUDPDataSendingThread(hiveNetworkCommon::CLogicalNode *vHostNode);
		virtual ~CUDPDataSendingThread();

	protected:
		virtual void _initThreadV() override;

	private:
		virtual boost::asio::socket_base* __chooseSocketV(const hiveNetworkCommon::SSendOptions& vSendOption) override;

		virtual unsigned int __sendIPPacketSetV(const std::vector<SRawDataBuffer>& vIPPacketSet, const hiveNetworkCommon::IUserPacket *vUserPacket, const hiveNetworkCommon::SSendOptions& vSendOption, boost::asio::socket_base *vSocket) override;

		unsigned int __try2SendInOneCall(const std::vector<SRawDataBuffer>& vIPPacketSet, const boost::asio::ip::udp::endpoint& vEndPoint, boost::asio::ip::udp::socket *vSocket);
	};
}