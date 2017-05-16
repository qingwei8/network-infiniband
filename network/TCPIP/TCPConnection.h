#pragma once
#include <boost/asio.hpp>
#include "NetworkConnection.h"
#include "TCPCommon.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

namespace hiveTCPIP
{
	class CTCPDataReceivingThread;

	class CTCPConnection : public hiveNetworkCommon::INetworkConnection
	{
	public:
		CTCPConnection();
		CTCPConnection(hiveNetworkCommon::CLogicalNode* vHostNode);
		virtual ~CTCPConnection();

		void initAsIncommingConnection(boost::asio::ip::tcp::socket *vSocket);
		bool isOutgoingConnection() const {_ASSERTE(m_ConnectionType != UNDEFINED); return (m_ConnectionType == OUTGOING);}

		boost::asio::ip::tcp::socket* fetchSocket() const {return m_pConnectionSocket;}
	
	private:
		EConnectionType               m_ConnectionType;
		CTCPDataReceivingThread      *m_pDataReceivingThread;  //NOTE: 这里需要额外记录收包线程是因为在连接发生问题而被中断时，我们需要同时终止收包线程（收包线程和连接具有一一对应关系，而其他TCP节点内的线程不具有这个特点）。
		boost::asio::io_service      *m_pIOService;
		boost::asio::ip::tcp::socket *m_pConnectionSocket;

		virtual void __onConnectedV() override;
		virtual bool __buildV(const std::string& vRemoteIP, unsigned short vRemotePort) override;
		virtual void __closeConnectionV() override;

		void __initAsOutgoingConnection(const std::string& vRemoteIP, unsigned int vRemotePort);
	};
}