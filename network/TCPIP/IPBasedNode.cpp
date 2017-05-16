#include "IPBasedNode.h"
#include "common/HiveConfig.h"
#include "Packet.h"

using namespace hiveTCPIP;

CIPBasedNode::CIPBasedNode() 
{
}

CIPBasedNode::~CIPBasedNode()
{
}

//******************************************************************
//FUNCTION:
void CIPBasedNode::_startNodeV()
{
	_startAllThreads();
}

//***********************************************************
//FUNCTION:
bool CIPBasedNode::registerSocket(const SSocketInfo& vSocketInfo, boost::asio::socket_base* vSocket)
{
	_ASSERTE(vSocket);
	for (unsigned int i=0; i<m_SocketSet.size(); i++)
	{
		if (m_SocketSet[i].first == vSocketInfo) return false;
	}
	m_SocketSet.push_back(std::make_pair(vSocketInfo, vSocket));
	return true;
}

//**************************************************************************************
//FUNCTION:
void CIPBasedNode::__sendPacketV(const hiveNetworkCommon::IUserPacket *vPacket, const hiveNetworkCommon::SSendOptions& vSendOption)
{
//NOTE: ע����������Ҫ��vPacket������ָ��stack�ڴ��ָ�룬��ʱ��Ϊ�˺�����δ������ִ�з�����������ֻ�ǽ���ѹ��m_OutgoingPacketQueue��
//      ʵ�ʵķ�������ʵ������һ���߳�����ɡ��������vPacketΪָ��stack�ڴ��ָ�룬�û������д�������ƴ��룺
//      IUserPacket Packet;
//      while (true)
//      {
//           //��Packet���������
//           __sendPacketV(&Packet...
//      }
//      ����������������ݺ�ʵ�ʵķ����������ڲ�ͬ�߳�����ɣ�����������ʵ��ÿ�ζ���д�뵽ͬһ���ڴ��У����ܵ��·���ȥ�İ�������
//      ���ִ������磬���߳�1���һ�������߳�2�������˰����ͣ����Ͱ���������ȷ��������߳�1��������������������߳�2�ٰ�������
//      �����ͳ�ȥ����ʵ�����߳�2���͵���������������һ����
//      ����һ�����������������vPacketָֻ��stack�ڴ��ָ�룬�����IUserPacket::clonePacket()���������ݿ���������һ�����У����
//      ͻ���������ơ������ǵ�__sendPacketV()���ܻᱻƵ�����ã��Ұ��Ĵ�С���ܽϴ�Ƶ���İ��������ܽ��ͳ�������Ч�ʣ�������ǲ���
//      ��Ŀǰ������������
	_ASSERTE(isRunning() && vPacket && !hiveCommon::hiveIsMemoryOnStack((void*)vPacket));  
	m_OutgoingPacketQueue.push(std::make_pair(vPacket, vSendOption));
}

//******************************************************************
//FUNCTION:
void CIPBasedNode::_onUserPacketReceived(hiveNetworkCommon::IUserPacket *vUserPacket)
{
	_ASSERTE(vUserPacket);
	m_IncomingPacketQueue.push(vUserPacket);
}

//***********************************************************
//FUNCTION:
void CIPBasedNode::_stopNodeV()
{
	__closeAndDestroyAllSocketsV();  //NOTE: ������ֹͣ�ڵ�ʱҪɾ�����е�socket��������������Ϊ��CIPBasedNode�б������ָ��boost::asio::socket_base��ָ�룬boost::asio::socket_base����������
	                                 //      �Ǳ�����Ա���޷����ⲿ��������CIPBasedNode����������֪��ʵ�ʵ�socket����������ｻ������麯������������ȥ�������socket�����������
	                                 //      ������������������ǣ���һ���ڵ㱻����_stopNodeV()����������µ���_startNodeV()��������socket������뱻���´���
}

//***********************************************************
//FUNCTION:
boost::asio::socket_base* CIPBasedNode::_findBroadcastSocket() const
{
	for (unsigned int i=0; i<m_SocketSet.size(); ++i) 
	{
		if (m_SocketSet[i].first.Usage & SOCKET_BROADCAST) return m_SocketSet[i].second;
	}
	return nullptr;
}

//***********************************************************
//FUNCTION:
boost::asio::socket_base* CIPBasedNode::_findMulticastSocket(const std::string& vRemoteIP) const
{
	for (unsigned int i=0; i<m_SocketSet.size(); ++i) 
	{
		if ((m_SocketSet[i].first.Usage & SOCKET_MULTICAST) && (m_SocketSet[i].first.IP == vRemoteIP)) return m_SocketSet[i].second;
	}
	return nullptr;
}

//***********************************************************
//FUNCTION:
boost::asio::socket_base* CIPBasedNode::_findUnicastSocket() const
{
	for (unsigned int i=0; i<m_SocketSet.size(); ++i) 
	{
		if (m_SocketSet[i].first.Usage & SOCKET_UNICAST) return m_SocketSet[i].second;
	}
	return nullptr;
}