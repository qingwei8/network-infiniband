#include "UDPDataSendingThread.h"
#include <time.h>
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "Packet.h"
#include "NetworkInterface.h"
#include "UDPNode.h"

using namespace hiveTCPIP;

CUDPDataSendingThread::CUDPDataSendingThread()
{
	_setThreadName("CUDPDataSendingThread");
}

CUDPDataSendingThread::CUDPDataSendingThread(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	_ASSERTE(vHostNode);
	_setHostLogicalNode(vHostNode);
	_setThreadName("CUDPDataSendingThread");
}

CUDPDataSendingThread::~CUDPDataSendingThread()
{
}

//**************************************************************************
//FUNCTION:
void CUDPDataSendingThread::_initThreadV()
{
	const hiveConfig::CHiveConfig *pNetworkConfig = hiveNetworkCommon::hiveGetNetworkConfig();
	_ASSERTE(pNetworkConfig && pNetworkConfig->isAttributeExisted(CONFIG_KEYWORD::UDP_MAX_PACKET_SIZE));
	setMaxIPPacketSize(pNetworkConfig->getAttribute<int>(CONFIG_KEYWORD::UDP_MAX_PACKET_SIZE));

	_ASSERTE(getHostLogicalNode());
	hiveCommon::hiveOutputEvent(_BOOST_STR2("The maximum size of IP packet of node [%1%] is set to [%2%].", getHostLogicalNode()->getName(), pNetworkConfig->getAttribute<int>(CONFIG_KEYWORD::UDP_MAX_PACKET_SIZE)));
}

//**************************************************************************
//FUNCTION:
boost::asio::socket_base* CUDPDataSendingThread::__chooseSocketV(const hiveNetworkCommon::SSendOptions& vSendOption)
{
	const CUDPNode *pHostNode = dynamic_cast<const CUDPNode*>(getHostLogicalNode());
	_ASSERTE(pHostNode);

	if (vSendOption.SendMode == hiveNetworkCommon::BROADCAST) return pHostNode->_findBroadcastSocket();
	if (vSendOption.SendMode == hiveNetworkCommon::MULTICAST) return pHostNode->_findMulticastSocket(vSendOption.TargetIP);
	_ASSERTE(vSendOption.SendMode == hiveNetworkCommon::UNICAST);
	return pHostNode->_findUnicastSocket();
}

//**************************************************************************************
//FUCTION:
unsigned int CUDPDataSendingThread::__sendIPPacketSetV(const std::vector<SRawDataBuffer>& vIPPacketSet, const hiveNetworkCommon::IUserPacket *vUserPacket, const hiveNetworkCommon::SSendOptions& vSendOption, boost::asio::socket_base *vSocket)
{
	_ASSERTE(!vIPPacketSet.empty() && vUserPacket && vSocket && !vSendOption.TargetIP.empty() && (vSendOption.TargetPort != hiveNetworkCommon::DUMMY_PORT));

	boost::asio::ip::udp::endpoint EndPoint(boost::asio::ip::address::from_string(vSendOption.TargetIP), vSendOption.TargetPort);
	boost::asio::ip::udp::socket *pSocket = (boost::asio::ip::udp::socket*)(vSocket);
	_ASSERTE(pSocket && pSocket->is_open());

	int NumBytesSend = 0;
	for (unsigned int i=0; i<vIPPacketSet.size(); i++) 
	{
		NumBytesSend += pSocket->send_to(boost::asio::buffer(vIPPacketSet[i].fetchBufferAddress(), vIPPacketSet[i].getBufferSize()), EndPoint, 0, m_ErrorCode);
		if (m_ErrorCode) break;
	}
	return NumBytesSend;
}

//****************************************************************************
//FUNCTION:
//NOTE: 这个函数并没有调用，它尝试通过一条send_to()命令来发送vIPPacketSet中的所有内容，但根据很初步的测试，似乎通过一条send_to()发包会导致收包效率下降。
//      这里需要更详细的测试，并且感觉收发包效率还和UDP_DEFAULT_RECEIVE_BUFFER_SIZE、UDP_DEFAULT_MAX_PACKET_SIZE以及包本身大小有关
unsigned int CUDPDataSendingThread::__try2SendInOneCall(const std::vector<SRawDataBuffer>& vIPPacketSet, const boost::asio::ip::udp::endpoint& vEndPoint, boost::asio::ip::udp::socket *vSocket)
{
	std::vector<boost::asio::const_buffer> BufferSet;
	for (unsigned int i=0; i<vIPPacketSet.size(); i++) BufferSet.push_back(boost::asio::buffer(vIPPacketSet[i].fetchBufferAddress(), vIPPacketSet[i].getBufferSize()));
	int NumBytesSend = vSocket->send_to(BufferSet, vEndPoint, 0, m_ErrorCode);

	if (m_ErrorCode)
	{//NOTE: 当vIPPacketSet中所有数据的大小加一起太大时（需要确定太大的具体值，是unsigned shrot最大值？），通过将所有数据放入BufferSet并通过一条send_to()命令进行发送时，发送会失败，
	 //      此时通过逐个发送vIPPacketSet中的包的方式来解决这个问题
		NumBytesSend = 0;
		for (unsigned int i=0; i<vIPPacketSet.size(); i++) 
		{
			NumBytesSend += vSocket->send_to(boost::asio::buffer(vIPPacketSet[i].fetchBufferAddress(), vIPPacketSet[i].getBufferSize()), vEndPoint, 0, m_ErrorCode);
			if (m_ErrorCode) break;
		}
	}

	return NumBytesSend;
}