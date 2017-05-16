#include "TCPDataSendingThread.h"
#include <boost/format.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "Packet.h"
#include "NetworkInterface.h"
#include "TCPNode.h"

using namespace hiveTCPIP;

CTCPDataSendingThread::CTCPDataSendingThread()
{
	_setThreadName("CTCPDataSendingThread");
}

CTCPDataSendingThread::CTCPDataSendingThread(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	_ASSERTE(vHostNode);
	_setHostLogicalNode(vHostNode);
	_setThreadName("CTCPDataSendingThread");
}

CTCPDataSendingThread::~CTCPDataSendingThread()
{
}

//****************************************************************************
//FUNCTION:
void CTCPDataSendingThread::_initThreadV()
{
	const hiveConfig::CHiveConfig *pNetworkConfig = hiveNetworkCommon::hiveGetNetworkConfig();
	_ASSERTE(pNetworkConfig && pNetworkConfig->isAttributeExisted(CONFIG_KEYWORD::TCP_MAX_PACKET_SIZE));
	setMaxIPPacketSize(pNetworkConfig->getAttribute<int>(CONFIG_KEYWORD::TCP_MAX_PACKET_SIZE));

	_ASSERTE(getHostLogicalNode());
	hiveCommon::hiveOutputEvent(_BOOST_STR2("The maximum size of IP packet of node [%1%] is set to [%2%].", getHostLogicalNode()->getName(), pNetworkConfig->getAttribute<int>(CONFIG_KEYWORD::TCP_MAX_PACKET_SIZE)));
}

//***********************************************************
//FUNCTION:
unsigned int CTCPDataSendingThread::__sendIPPacketSetV(const std::vector<SRawDataBuffer>& vIPPacketSet, const hiveNetworkCommon::IUserPacket *vUserPacket, const hiveNetworkCommon::SSendOptions& vSendOption, boost::asio::socket_base *vSocket)
{
	boost::asio::ip::tcp::socket *pSocket = (boost::asio::ip::tcp::socket*)(vSocket);
	_ASSERTE(pSocket && pSocket->is_open());

	unsigned int NumBytesSend = 0;
	for (unsigned int i=0; i<vIPPacketSet.size(); i++)
	{
		NumBytesSend = pSocket->send(boost::asio::buffer(vIPPacketSet[i].fetchBufferAddress(), vIPPacketSet[i].getBufferSize()), 0, m_ErrorCode);
		if (m_ErrorCode) break;
	}
	return NumBytesSend;
}

//***********************************************************
//FUNCTION:
boost::asio::socket_base* CTCPDataSendingThread::__chooseSocketV(const hiveNetworkCommon::SSendOptions& vSendOption)
{
	CTCPNode *pHostNode = dynamic_cast<CTCPNode*>(fetchHostLogicalNode());
	_ASSERT(pHostNode);
	return pHostNode->findSocket(vSendOption.TargetIP, vSendOption.TargetPort);
}

//****************************************************************************
//FUNCTION:
EOpOnNetworkFailure CTCPDataSendingThread::__onSendingDataFailV(const boost::system::error_code& vErrorCode, boost::asio::socket_base *vSocket)
{
	switch (vErrorCode.value())
	{
	case 10053:
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