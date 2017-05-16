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
//NOTE: ���������û�е��ã�������ͨ��һ��send_to()����������vIPPacketSet�е��������ݣ������ݺܳ����Ĳ��ԣ��ƺ�ͨ��һ��send_to()�����ᵼ���հ�Ч���½���
//      ������Ҫ����ϸ�Ĳ��ԣ����Ҹо��շ���Ч�ʻ���UDP_DEFAULT_RECEIVE_BUFFER_SIZE��UDP_DEFAULT_MAX_PACKET_SIZE�Լ��������С�й�
unsigned int CUDPDataSendingThread::__try2SendInOneCall(const std::vector<SRawDataBuffer>& vIPPacketSet, const boost::asio::ip::udp::endpoint& vEndPoint, boost::asio::ip::udp::socket *vSocket)
{
	std::vector<boost::asio::const_buffer> BufferSet;
	for (unsigned int i=0; i<vIPPacketSet.size(); i++) BufferSet.push_back(boost::asio::buffer(vIPPacketSet[i].fetchBufferAddress(), vIPPacketSet[i].getBufferSize()));
	int NumBytesSend = vSocket->send_to(BufferSet, vEndPoint, 0, m_ErrorCode);

	if (m_ErrorCode)
	{//NOTE: ��vIPPacketSet���������ݵĴ�С��һ��̫��ʱ����Ҫȷ��̫��ľ���ֵ����unsigned shrot���ֵ������ͨ�����������ݷ���BufferSet��ͨ��һ��send_to()������з���ʱ�����ͻ�ʧ�ܣ�
	 //      ��ʱͨ���������vIPPacketSet�еİ��ķ�ʽ������������
		NumBytesSend = 0;
		for (unsigned int i=0; i<vIPPacketSet.size(); i++) 
		{
			NumBytesSend += vSocket->send_to(boost::asio::buffer(vIPPacketSet[i].fetchBufferAddress(), vIPPacketSet[i].getBufferSize()), vEndPoint, 0, m_ErrorCode);
			if (m_ErrorCode) break;
		}
	}

	return NumBytesSend;
}