#pragma once
#include <boost/asio.hpp>
#include "IPBasedDataReceivingThread.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

namespace hiveTCPIP
{
	class CTCPConnection;

	class CTCPDataReceivingThread : public IIPBasedDataReceivingThread
	{
	public:
		CTCPDataReceivingThread();
		CTCPDataReceivingThread(hiveNetworkCommon::CLogicalNode *vHostNode, boost::asio::ip::tcp::socket *vSocket);
		virtual ~CTCPDataReceivingThread();

	protected:
		virtual void _initThreadV() override;

	private:
		virtual int  __receivingDataV(boost::asio::socket_base *vSocket, SRawDataBuffer& voReceiveBuffer) override;
		virtual bool __isWholeIPPacketReceivedV() override {return false;}

		virtual EOpOnNetworkFailure __onReceivingDataFailV(const boost::system::error_code& vErrorCode, boost::asio::socket_base *vSocket) override;
	};
}