#pragma once
#include <boost/asio.hpp>
#include "IPBasedDataSendingThread.h"

namespace hiveNetworkCommon
{
	class IUserPacket;
	class CLogicalNode;
}

namespace hiveTCPIP
{
	class CTCPDataSendingThread : public IIPBasedDataSendingThread
	{
	public:
		CTCPDataSendingThread();
		CTCPDataSendingThread(hiveNetworkCommon::CLogicalNode *vHostNode);
		virtual ~CTCPDataSendingThread();

	protected:
		virtual void _initThreadV() override;

	private:
		virtual boost::asio::socket_base* __chooseSocketV(const hiveNetworkCommon::SSendOptions& vSendOption) override;

		virtual unsigned int __sendIPPacketSetV(const std::vector<SRawDataBuffer>& vIPPacketSet, const hiveNetworkCommon::IUserPacket *vUserPacket, const hiveNetworkCommon::SSendOptions& vSendOption, boost::asio::socket_base *vSocket) override;

		virtual EOpOnNetworkFailure __onSendingDataFailV(const boost::system::error_code& vErrorCode, boost::asio::socket_base *vSocket) override;
	};
}