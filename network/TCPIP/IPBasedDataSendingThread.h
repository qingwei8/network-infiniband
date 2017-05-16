#pragma once
#include "NetworkThread.h"
#include <boost/asio/socket_base.hpp>
#include "NetworkCommon.h"
#include "UserPacketSplitter.h"

namespace hiveNetworkCommon
{
	class IUserPacket;
}

namespace hiveTCPIP
{
	class IIPBasedDataSendingThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		IIPBasedDataSendingThread();
		virtual ~IIPBasedDataSendingThread();

		void setMaxIPPacketSize(unsigned int vMaxSize) {m_UserPacketSplitter.setMaxIPPacketSize(vMaxSize);}

	protected:
		boost::system::error_code m_ErrorCode;

	private:
		CUserPacketSplitter m_UserPacketSplitter;

		virtual boost::asio::socket_base* __chooseSocketV(const hiveNetworkCommon::SSendOptions& vSendOption) = 0;
		virtual unsigned int __sendIPPacketSetV(const std::vector<SRawDataBuffer>& vIPPacketSet, const hiveNetworkCommon::IUserPacket *vUserPacket, const hiveNetworkCommon::SSendOptions& vSendOption, boost::asio::socket_base *vSocket) = 0;

		virtual EOpOnNetworkFailure __onSendingDataFailV(const boost::system::error_code& vErrorCode, boost::asio::socket_base *vSocket) {return CONTINUE_THREAD;}

		virtual void __runV() override;
	};
}