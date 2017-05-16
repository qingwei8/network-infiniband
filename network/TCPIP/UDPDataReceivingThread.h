#pragma once
#include <boost/asio.hpp>
#include "IPBasedDataReceivingThread.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

namespace hiveTCPIP
{
	class CUDPDataReceivingThread : public IIPBasedDataReceivingThread
	{
	public:
		CUDPDataReceivingThread();
		CUDPDataReceivingThread(hiveNetworkCommon::CLogicalNode *vHostNode);
		virtual ~CUDPDataReceivingThread();

	protected:
		virtual void _initThreadV() override;

	private:
		virtual int  __receivingDataV(boost::asio::socket_base *vSocket, SRawDataBuffer& voReceiveBuffer) override;
		virtual bool __isWholeIPPacketReceivedV() override {return true;}
	};
}