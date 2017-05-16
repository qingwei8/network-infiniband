#pragma once
#include "LogicalNode.h"
#include <boost/asio.hpp>
#include "TCPIPCommon.h"

namespace hiveNetworkCommon
{
	class IUserPacket;
}

namespace hiveTCPIP
{
	class CIPBasedNode : public hiveNetworkCommon::CLogicalNode
	{
	public:
		CIPBasedNode();
		virtual ~CIPBasedNode();
		
		bool registerSocket(const SSocketInfo& vSocketInfo, boost::asio::socket_base* vSocket);

	protected:
		virtual void _startNodeV() override;
		virtual void _stopNodeV() override;

		void _onUserPacketReceived(hiveNetworkCommon::IUserPacket *vUserPacket);
		
		boost::asio::socket_base* _findBroadcastSocket() const;
		boost::asio::socket_base* _findMulticastSocket(const std::string& vRemoteIP) const;
		boost::asio::socket_base* _findUnicastSocket() const;
		boost::asio::socket_base* _fetchSocketAt(unsigned int vIndex) const {_ASSERTE(vIndex < m_SocketSet.size()); return m_SocketSet[vIndex].second;}
		
		unsigned int _getNumSockets() const {return m_SocketSet.size();}

	private:
		std::vector<std::pair<SSocketInfo, boost::asio::socket_base*>> m_SocketSet;  //NOTE: socket��ά����CIPBasedNode����ɣ����ǿ��ǵ�����TCP����UDP�����Ͱ��������������ͨ��socket�����

		virtual void __sendPacketV(const hiveNetworkCommon::IUserPacket *vPacket, const hiveNetworkCommon::SSendOptions& vSendOption) override;
		virtual void __closeAndDestroyAllSocketsV() = 0;

	friend class IIPBasedDataReceivingThread;
	friend class CIPBasedPacketHandlingThread;
	friend class IIPBasedDataSendingThread;
	};
}