#include "TCPDataReceivingThread.h"
#include <boost/format.hpp>
#include "common/HiveCommonMicro.h"
#include "common/CommonInterface.h"
#include "NetworkInterface.h"
#include "TCPIPCommon.h"
#include "TCPCommon.h"
#include "TCPNode.h"

using namespace hiveTCPIP;

CTCPDataReceivingThread::CTCPDataReceivingThread()
{
	_setThreadName("CTCPDataReceivingThread");
}

CTCPDataReceivingThread::CTCPDataReceivingThread(hiveNetworkCommon::CLogicalNode *vHostNode, boost::asio::ip::tcp::socket *vSocket)
{
	_ASSERTE(vHostNode && vSocket);
	_setThreadName("CTCPDataReceivingThread");
	_setHostLogicalNode(vHostNode);
	setSocket(vSocket);
}

CTCPDataReceivingThread::~CTCPDataReceivingThread()
{
}

//*********************************************************************************************************************************************************
//FUNCTION:
int CTCPDataReceivingThread::__receivingDataV(boost::asio::socket_base *vSocket, SRawDataBuffer& voReceiveBuffer)
{	
	_ASSERTE(voReceiveBuffer.isValidBuffer());
	boost::asio::ip::tcp::socket *pSocket = (boost::asio::ip::tcp::socket*)(vSocket);
	_ASSERTE(pSocket && pSocket->is_open());
	return pSocket->receive(boost::asio::buffer(voReceiveBuffer.fetchBufferAddress(), voReceiveBuffer.getBufferSize()), 0, m_ErrorCode);  //NOTE: 这里不能调用boost::asio::read()，因为这个函数会阻塞进程直到缓冲区被填满才返回
}

//******************************************************************
//FUNCTION:
void CTCPDataReceivingThread::_initThreadV() 
{
	const hiveConfig::CHiveConfig *pNetworkConfig = hiveNetworkCommon::hiveGetNetworkConfig();
	_ASSERTE(pNetworkConfig && pNetworkConfig->isAttributeExisted(CONFIG_KEYWORD::TCP_RECEIVE_BUFFER_SIZE));
	initReceiveBuffer(pNetworkConfig->getAttribute<int>(CONFIG_KEYWORD::TCP_RECEIVE_BUFFER_SIZE));
}

//****************************************************************************
//FUNCTION:
EOpOnNetworkFailure CTCPDataReceivingThread::__onReceivingDataFailV(const boost::system::error_code& vErrorCode, boost::asio::socket_base *vSocket)
{
	switch (vErrorCode.value())
	{
	case 10054:
		{
			boost::asio::ip::tcp::socket *pSocket = (boost::asio::ip::tcp::socket*)vSocket;
			_ASSERTE(vSocket);
			boost::asio::ip::tcp::endpoint RemoteEndPoint = pSocket->remote_endpoint();
			CTCPNode *pHostNode = dynamic_cast<CTCPNode*>(fetchHostLogicalNode());
			_ASSERT(pHostNode);
			pHostNode->notifyConnectionIsBroken(RemoteEndPoint.address().to_string(), RemoteEndPoint.port());
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR2("The connection to [%1%:%2%] is broken.", RemoteEndPoint.address().to_string(), RemoteEndPoint.port()));
		}
		return TERMINATE_THREAD;
	}
	return CONTINUE_THREAD;
}