#include "UDPNode.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/HiveCommonMicro.h"
#include "common/CommonInterface.h"
#include "common/ProductFactory.h"
#include "common/HiveConfig.h"
#include "Packet.h"
#include "UserPacketHandlingThread.h"
#include "UDPDataReceivingThread.h"
#include "UDPDataSendingThread.h"
#include "TCPIPCommon.h"
#include "TCPIPInterface.h"
#include "NetworkInterface.h"

using namespace hiveTCPIP;

hiveCommon::CProductFactory<CUDPNode> theCreator(DEFAULT_UDP_NODE_SIG);

CUDPNode::CUDPNode() : m_pIOService(new boost::asio::io_service)
{
}

CUDPNode::~CUDPNode()
{
	delete m_pIOService;
}

//*********************************************************************************
//FUNCTION:
void CUDPNode::__createNetworkThreadsV(std::vector<hiveNetworkCommon::INetworkThread*>& voOutput)
{
	_ASSERTE(voOutput.empty());

	hiveNetworkCommon::INetworkThread *pPacketHandlingThread = nullptr;
	hiveNetworkCommon::INetworkThread *pPacketSendingThread  = nullptr;

	const hiveConfig::CHiveConfig* pNodeConfig = getConfig();
	_ASSERTE(pNodeConfig);

	for (unsigned int i=0; i<pNodeConfig->getNumSubConfig(); i++)
	{
		const hiveConfig::CHiveConfig* pSocketConfig = pNodeConfig->getSubConfigAt(i);
		unsigned char SocketUsage = _hiveInterpretSocketUsage(pSocketConfig->getAttribute<std::string>(CONFIG_KEYWORD::UDP_SOCKET_USAGE));

		if (!pPacketHandlingThread && (SocketUsage & SOCKET_RECEIVE)) pPacketHandlingThread = new hiveNetworkCommon::CUserPacketHandlingThread(this);
		if (!pPacketSendingThread && ((SocketUsage & SOCKET_BROADCAST) || (SocketUsage & SOCKET_MULTICAST) || (SocketUsage & SOCKET_UNICAST))) pPacketSendingThread = new CUDPDataSendingThread(this);
	}
	
	if (pPacketHandlingThread) voOutput.push_back(pPacketHandlingThread);
	if (pPacketSendingThread) voOutput.push_back(pPacketSendingThread);
}

//*********************************************************************************
//FUNCTION:
bool CUDPNode::__extraInitV()
{
	const hiveConfig::CHiveConfig* pNodeConfig = getConfig();
	_ASSERTE(pNodeConfig);

	for (unsigned int i=0; i<pNodeConfig->getNumSubConfig(); i++)
	{
		const hiveConfig::CHiveConfig* pSocketConfig = pNodeConfig->getSubConfigAt(i);
		unsigned char SocketUsage = _hiveInterpretSocketUsage(pSocketConfig->getAttribute<std::string>(CONFIG_KEYWORD::UDP_SOCKET_USAGE));
		if (SocketUsage & SOCKET_UNDEFINED)
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("Invalid socket usage for [%1%].", pSocketConfig->getName()));
			continue;
		}

		if (SocketUsage & SOCKET_UNICAST)   __createAndOpenUDPSocket(pSocketConfig->getName(), SOCKET_UNICAST, hiveNetworkCommon::DUMMY_IP, hiveNetworkCommon::DUMMY_PORT);
		if (SocketUsage & SOCKET_BROADCAST) __createAndOpenUDPSocket(pSocketConfig->getName(), SOCKET_BROADCAST, hiveNetworkCommon::DUMMY_IP, hiveNetworkCommon::DUMMY_PORT);
		if (SocketUsage & SOCKET_MULTICAST)
		{
			_ASSERT(pSocketConfig->isAttributeExisted(CONFIG_KEYWORD::TARGET_IP));
			__createAndOpenUDPSocket(pSocketConfig->getName(), SOCKET_MULTICAST, pSocketConfig->getAttribute<std::string>(CONFIG_KEYWORD::TARGET_IP), hiveNetworkCommon::DUMMY_PORT);
		}
		if (SocketUsage & SOCKET_RECEIVE)   	
		{
			boost::asio::ip::udp::socket* pSocket = __createAndOpenUDPSocket(pSocketConfig->getName(), SOCKET_RECEIVE, hiveNetworkCommon::DUMMY_IP, pSocketConfig->getAttribute<int>(CONFIG_KEYWORD::PORT));
			if (pSocketConfig->isAttributeExisted(CONFIG_KEYWORD::TARGET_IP))
			{
				pSocket->set_option(boost::asio::ip::udp::socket::reuse_address(true)); // allow multiple sockets to use the same PORT number
				pSocket->set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(pSocketConfig->getAttribute<std::string>(CONFIG_KEYWORD::TARGET_IP))));
			}
			
			CUDPDataReceivingThread *pReceivingThread = new CUDPDataReceivingThread(this);
			pReceivingThread->setSocket(pSocket);
		}
	}
	return true;
}

//***********************************************************
//FUNCTION:
boost::asio::ip::udp::socket* CUDPNode::__createAndOpenUDPSocket(const std::string& vSocketName, ESocketUsage vSocketUsage, const std::string& vIP, unsigned short vPort)
{
	_ASSERTE(m_pIOService);
	boost::asio::ip::udp::socket *pSocket = new boost::asio::ip::udp::socket(*m_pIOService);

	SSocketInfo SocketInfo;
	SocketInfo.Usage = vSocketUsage;
	SocketInfo.Port  = vPort;
	SocketInfo.IP    = vIP;
	if (registerSocket(SocketInfo, pSocket))
	{
		boost::system::error_code ErrorCode;
		pSocket->open(boost::asio::ip::udp::v4(), ErrorCode);
		if (ErrorCode)
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR2("Fail to open socket [%1%] with the following error message [%2%].", vSocketName, ErrorCode.message()));
			delete pSocket;
			return nullptr;
		}
		hiveCommon::hiveOutputEvent(_BOOST_STR2("Succeed to create and open socket [%1%] of node [%2%].", vSocketName, getName()));
	}
	else
	{
		delete pSocket;
		return nullptr;
	}

	__configSocket(SocketInfo, pSocket);
	return pSocket;
}

//****************************************************************************
//FUNCTION:
void CUDPNode::__configSocket(const SSocketInfo& vSocketInfo, boost::asio::ip::udp::socket* vioSocket)
{
	_ASSERTE(vioSocket && vioSocket->is_open());

	switch (vSocketInfo.Usage)
	{
	case SOCKET_BROADCAST:
		{
			vioSocket->set_option(boost::asio::socket_base::broadcast(true));
		}
		break;
	case SOCKET_MULTICAST:
		{
			vioSocket->set_option(boost::asio::ip::udp::socket::reuse_address(true)); // allow multiple sockets to use the same PORT number			
			vioSocket->set_option(boost::asio::ip::multicast::join_group(boost::asio::ip::address::from_string(vSocketInfo.IP)));
		}
		break;
	case SOCKET_UNICAST:
		{
		}
		break;
	case SOCKET_RECEIVE:
		{
  			boost::asio::ip::udp::endpoint EndPoint(boost::asio::ip::address::from_string(vSocketInfo.IP), vSocketInfo.Port);
 			vioSocket->bind(EndPoint);
		}
		break;
	}
}

//******************************************************************
//FUNCTION:
void CUDPNode::__closeAndDestroyAllSocketsV()
{
	boost::system::error_code ErrorCode;
	for (unsigned int i=0; i<_getNumSockets(); ++i) 
	{
		boost::asio::ip::udp::socket *pSocket = (boost::asio::ip::udp::socket*)(_fetchSocketAt(i));  //TODO: reinterpret_cast does not work here, why?
		_ASSERTE(pSocket);
		if (pSocket->is_open()) pSocket->close(ErrorCode);
		delete pSocket;
	}
}