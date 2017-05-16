#pragma once
#include <boost/asio.hpp>
#include "NetworkThread.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

namespace hiveTCPIP
{
	class CTCPNode;

	class CTCPConnectionListenThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		CTCPConnectionListenThread();
		CTCPConnectionListenThread(hiveNetworkCommon::CLogicalNode *vHostNode, int vListeningPort);
		virtual ~CTCPConnectionListenThread();

	protected:
		virtual void _initThreadV() override;

	private:
		int m_ListingPort;
		boost::asio::io_service*        m_pIOService;
		boost::asio::ip::tcp::acceptor* m_pAcceptor;

		void __stopListening();
		void __onIncomingConnectionAccepted(CTCPNode *vHostNode, boost::asio::ip::tcp::socket *vIncommingSocket);

		virtual void __runV() override;
	};
}