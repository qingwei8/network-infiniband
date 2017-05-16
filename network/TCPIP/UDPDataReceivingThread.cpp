#include "UDPDataReceivingThread.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/ProductFactory.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "NetworkInterface.h"
#include "TCPIPCommon.h"
#include "UDPNode.h"

using namespace hiveTCPIP;

hiveCommon::CProductFactory<CUDPDataReceivingThread> theCreator("CUDPDataReceivingThread");

CUDPDataReceivingThread::CUDPDataReceivingThread()
{
	_setThreadName("CUDPDataReceivingThread");
}

CUDPDataReceivingThread::CUDPDataReceivingThread(hiveNetworkCommon::CLogicalNode *vHostNode)
{
	_ASSERTE(vHostNode);
	_setHostLogicalNode(vHostNode);
	_setThreadName("CUDPDataReceivingThread");
}

CUDPDataReceivingThread::~CUDPDataReceivingThread()
{
}

//**************************************************************************
//FUNCTION:
void CUDPDataReceivingThread::_initThreadV()
{
	const hiveConfig::CHiveConfig *pNetworkConfig = hiveNetworkCommon::hiveGetNetworkConfig();
	_ASSERTE(pNetworkConfig && pNetworkConfig->isAttributeExisted(CONFIG_KEYWORD::UDP_RECEIVE_BUFFER_SIZE));
	initReceiveBuffer(pNetworkConfig->getAttribute<int>(CONFIG_KEYWORD::UDP_RECEIVE_BUFFER_SIZE));
}

//*********************************************************************************
//FUNCTION:
int CUDPDataReceivingThread::__receivingDataV(boost::asio::socket_base *vSocket, SRawDataBuffer& voReceiveBuffer)
{
	_ASSERTE(voReceiveBuffer.isValidBuffer());
	boost::asio::ip::udp::socket *pSocket = (boost::asio::ip::udp::socket*)(vSocket);
	_ASSERTE(pSocket && pSocket->is_open());
	boost::asio::ip::udp::endpoint EndPoint;
	return pSocket->receive_from(boost::asio::buffer(voReceiveBuffer.fetchBufferAddress(), voReceiveBuffer.getBufferSize()), EndPoint, 0, m_ErrorCode);
}