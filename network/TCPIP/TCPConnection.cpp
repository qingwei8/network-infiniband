#include "TCPConnection.h"
#include <mstcpip.h>
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "common/ProductFactory.h"
#include "LogicalNode.h"
#include "TCPCommon.h"
#include "TCPNode.h"
#include "TCPDataReceivingThread.h"

using namespace hiveTCPIP;

hiveCommon::CProductFactory<CTCPConnection> theCreator("TCP_CONNECTION");

CTCPConnection::CTCPConnection() : m_pIOService(nullptr), m_pConnectionSocket(nullptr), m_ConnectionType(UNDEFINED), m_pDataReceivingThread(nullptr)
{

}

CTCPConnection::CTCPConnection(hiveNetworkCommon::CLogicalNode* vHostNode) : m_pIOService(nullptr), m_pConnectionSocket(nullptr), m_ConnectionType(UNDEFINED), m_pDataReceivingThread(nullptr)
{
	_ASSERTE(vHostNode);
	_setHostNode(vHostNode);
}

CTCPConnection::~CTCPConnection()
{
//NOTE: m_pDataReceivingThread虽然由CTCPConnection负责创建，但其析构由CTCPNode负责。这时因为和TCP相关的所有线程都注册到CTCPNode中统一管理
	delete m_pIOService;
}

//**********************************************************************************
//FUNCTION:
void CTCPConnection::__closeConnectionV()
{
	_ASSERTE(m_pConnectionSocket && m_pDataReceivingThread && fetchHostNode());

	if (m_pConnectionSocket->is_open()) 
	{
		boost::system::error_code ErrorCode;
		m_pConnectionSocket->close(ErrorCode);
		_HIVE_SIMPLE_IF(ErrorCode, hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR2("Fail to close socket because: %1% (%2%).", ErrorCode.message(), ErrorCode.value())));
		_SAFE_DELETE(m_pConnectionSocket);
	}
}

//**************************************************************************************
//FUCTION:
void CTCPConnection::initAsIncommingConnection(boost::asio::ip::tcp::socket* vSocket) 
{
	_ASSERTE(!m_pConnectionSocket && vSocket); 
	m_pConnectionSocket = vSocket;
	m_ConnectionType = INCOMING;

	CTCPNode *pHostNode = dynamic_cast<CTCPNode*>(fetchHostNode());
	_ASSERTE(pHostNode);
	pHostNode->registerConnection(this);

	boost::asio::ip::tcp::endpoint RemoteEndPoint = vSocket->remote_endpoint();
	_initRemoteInfo(RemoteEndPoint.address().to_string(), RemoteEndPoint.port());

	_setState(STATE_CONNECTED);
}

//****************************************************************************
//FUNCTION:
void CTCPConnection::__initAsOutgoingConnection(const std::string& vRemoteIP, unsigned int vRemotePort)
{
	_ASSERTE(!m_pConnectionSocket && !m_pIOService);

	m_ConnectionType    = OUTGOING;
	m_pIOService        = new boost::asio::io_service;
	m_pConnectionSocket = new boost::asio::ip::tcp::socket(*m_pIOService);
	_initRemoteInfo(vRemoteIP, vRemotePort);
}

//**************************************************************************************
//FUCTION:
bool CTCPConnection::__buildV(const std::string& vRemoteIP, unsigned short vRemotePort)
{
	_ASSERTE((m_ConnectionType != INCOMING) && !vRemoteIP.empty() && isConnecting());

	boost::asio::ip::tcp::endpoint RemoteEndPoint(boost::asio::ip::address_v4::from_string(vRemoteIP), vRemotePort);

	if (m_ConnectionType == UNDEFINED) __initAsOutgoingConnection(vRemoteIP, vRemotePort);

	boost::system::error_code ErrorCode;
	m_pConnectionSocket->connect(RemoteEndPoint, ErrorCode);
	_HIVE_EARLY_RETURN(ErrorCode, _BOOST_STR4("Fail to build an outgoing TCP connection to [%1%:%2%] because: %3% (%4%).", 
		vRemoteIP, vRemotePort, ErrorCode.message(), ErrorCode.value()), false);

	CTCPNode *pHostNode = dynamic_cast<CTCPNode*>(fetchHostNode());
	_ASSERTE(pHostNode);
	pHostNode->registerConnection(this);

	hiveCommon::hiveOutputEvent(_BOOST_STR2("Succeed to build an outgoing TCP connection to [%1%:%2%].", vRemoteIP, vRemotePort));
	return true;
}

//**********************************************************************************
//FUNCTION:
void CTCPConnection::__onConnectedV()
{
	_ASSERTE(getHostNode() && m_pConnectionSocket && m_pConnectionSocket->is_open() && !m_pDataReceivingThread);
	m_pDataReceivingThread = new CTCPDataReceivingThread(fetchHostNode(), (boost::asio::ip::tcp::socket*)m_pConnectionSocket);
	m_pDataReceivingThread->startThread();
}