#include "TCPConnectionListenThread.h"
#include <mstcpip.h>
#include <boost/format.hpp>
#include "common/HiveCommonMicro.h"
#include "common/CommonInterface.h"
#include "LogicalNode.h"
#include "TCPDataReceivingThread.h"
#include "TCPConnection.h"
#include "TCPCommon.h"
#include "TCPNode.h"

using namespace hiveTCPIP;

CTCPConnectionListenThread::CTCPConnectionListenThread() : m_pIOService(nullptr), m_pAcceptor(nullptr), m_ListingPort(hiveNetworkCommon::DUMMY_PORT)
{
	_setThreadName("CTCPConnectionListenThread");
}

CTCPConnectionListenThread::CTCPConnectionListenThread(hiveNetworkCommon::CLogicalNode *vHostNode, int vListeningPort) : m_pIOService(nullptr), m_pAcceptor(nullptr), m_ListingPort(vListeningPort)
{
	_ASSERTE(vHostNode);
	_setHostLogicalNode(vHostNode);
	_setThreadName("CTCPConnectionListenThread");
}

CTCPConnectionListenThread::~CTCPConnectionListenThread()
{
	__stopListening();
	delete m_pAcceptor;
	delete m_pIOService;
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CTCPConnectionListenThread::__runV()
{
	_ASSERTE(m_pAcceptor);
	boost::system::error_code ErrorCode;
	CTCPNode* pTCPNode = dynamic_cast<CTCPNode*>(fetchHostLogicalNode());
	_ASSERTE(pTCPNode);

	while (isRunning() && !isInterruptionRequested())
	{
		boost::asio::ip::tcp::socket *pIncommingConnectionSocket = new boost::asio::ip::tcp::socket(*m_pIOService);
		
		m_pAcceptor->accept(*pIncommingConnectionSocket, ErrorCode);  //NOTE: ×èÈûµÈ´ý

		if (ErrorCode)
		{
			delete pIncommingConnectionSocket;
			_ASSERTE(getHostLogicalNode());
			hiveCommon::hiveOutputEvent(_BOOST_STR3("Error occurs in node [%3%] when listening the incoming connection: %1% (%2%)", ErrorCode.message(), ErrorCode.value(), pTCPNode->getName()));
			ErrorCode.clear();
			continue;
		}

		__onIncomingConnectionAccepted(pTCPNode, pIncommingConnectionSocket);

		hiveCommon::hiveOutputEvent(_BOOST_STR2("Succeed to build an incomming TCP connection from [%1%:%2%].", pIncommingConnectionSocket->remote_endpoint().address().to_string(), pIncommingConnectionSocket->remote_endpoint().port()));
	}
}

//****************************************************************************
//FUNCTION:
void CTCPConnectionListenThread::__onIncomingConnectionAccepted(CTCPNode *vHostNode, boost::asio::ip::tcp::socket *vIncommingSocket)
{
	_ASSERTE(vIncommingSocket && vHostNode);
	
	SSocketInfo SocketInfo;
	SocketInfo.Port  = vIncommingSocket->remote_endpoint().port();
	SocketInfo.IP    = vIncommingSocket->remote_endpoint().address().to_string();;
	vHostNode->registerSocket(SocketInfo, vIncommingSocket);

	CTCPConnection* pTCPConnection = new CTCPConnection(vHostNode);
	pTCPConnection->initAsIncommingConnection(vIncommingSocket);
	_ASSERT(pTCPConnection->isConnected());
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CTCPConnectionListenThread::_initThreadV()
{
	_ASSERTE((m_ListingPort != hiveNetworkCommon::DUMMY_PORT) && !m_pIOService && !m_pAcceptor);

	m_pIOService = new boost::asio::io_service;
	m_pAcceptor  = new boost::asio::ip::tcp::acceptor(*m_pIOService, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), m_ListingPort));
}

//*********************************************************************************************************************************************************
//FUNCTION:
void CTCPConnectionListenThread::__stopListening()
{
	if (!m_pAcceptor && m_pAcceptor->is_open())
	{
		boost::system::error_code ErrorCode;
		m_pAcceptor->close(ErrorCode);
		if (ErrorCode) hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR2("Fail to close TCP acceptor: %1% (%2%).", ErrorCode.message(), ErrorCode.value())); 
	}
}