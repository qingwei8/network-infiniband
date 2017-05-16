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
		CTCPDataReceivingThread      *m_pDataReceivingThread;  //NOTE: ������Ҫ�����¼�հ��߳�����Ϊ�����ӷ�����������ж�ʱ��������Ҫͬʱ��ֹ�հ��̣߳��հ��̺߳����Ӿ���һһ��Ӧ��ϵ��������TCP�ڵ��ڵ��̲߳���������ص㣩��
		boost::asio::io_service      *m_pIOService;
		boost::asio::ip::tcp::socket *m_pConnectionSocket;

		virtual void __onConnectedV() override;
		virtual bool __buildV(const std::string& vRemoteIP, unsigned short vRemotePort) override;
		virtual void __closeConnectionV() override;

		void __initAsOutgoingConnection(const std::string& vRemoteIP, unsigned int vRemotePort);
	};
}