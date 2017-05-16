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
//NOTE: 注意这里我们要求vPacket不能是指向stack内存的指针，这时因为此函数并未真正的执行发包操作，而只是将包压入m_OutgoingPacketQueue。
//      实际的发包操作实在另外一个线程中完成。如果允许vPacket为指向stack内存的指针，用户如果编写如下类似代码：
//      IUserPacket Packet;
//      while (true)
//      {
//           //往Packet中填充内容
//           __sendPacketV(&Packet...
//      }
//      由于往包里填充内容和实际的发包操作是在不同线程中完成，而填充包内容实际每次都是写入到同一块内存中，可能导致发出去的包的内容
//      出现错误。例如，若线程1填充一个包，线程2立即将此包发送，则发送包的内容正确。但如果线程1连续填充了两个包，而线程2再把这两个
//      包发送出去，则实际上线程2发送的两个包的内容是一样。
//      另外一个解决方案是若发现vPacket只指向stack内存的指针，则调用IUserPacket::clonePacket()来将其内容拷贝到另外一个包中，则可
//      突破如上限制。但考虑到__sendPacketV()可能会被频繁调用，且包的大小可能较大，频繁的包拷贝可能降低程序运行效率，因此我们采用
//      了目前的限制条件。
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
	__closeAndDestroyAllSocketsV();  //NOTE: 这里在停止节点时要删除所有的socket对象，这样做是因为在CIPBasedNode中保存的是指向boost::asio::socket_base的指针，boost::asio::socket_base的析构函数
	                                 //      是保护成员，无法在外部析构。而CIPBasedNode的派生类是知道实际的socket对象，因此这里交给这个虚函数，由派生类去完成所有socket对象的析构。
	                                 //      这个方案带来的问题是，当一个节点被调用_stopNodeV()后，如果想重新调用_startNodeV()，则所有socket对象必须被重新创建
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