#include "IPBasedDataSendingThread.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "Packet.h"
#include "NetworkCommon.h"
#include "IPBasedNode.h"

using namespace hiveTCPIP;

IIPBasedDataSendingThread::IIPBasedDataSendingThread()
{
}

IIPBasedDataSendingThread::~IIPBasedDataSendingThread()
{
}

//*********************************************************************************
//FUNCTION:
void IIPBasedDataSendingThread::__runV()
{
	CIPBasedNode *pHostNode = dynamic_cast<CIPBasedNode*>(fetchHostLogicalNode());
	_ASSERTE(pHostNode);
	const hiveNetworkCommon::IUserPacket *pUserPacket = nullptr;
	std::vector<SRawDataBuffer> IPPacketSet;
	unsigned int Counter=0;
	unsigned int NumBytesSend = 0;

	while (isRunning() && !isInterruptionRequested())
	{
		std::pair<const hiveNetworkCommon::IUserPacket*, hiveNetworkCommon::SSendOptions> UserPacket = pHostNode->_waitAndPopOutgoingUserPacket();
		pUserPacket = UserPacket.first;
		if (!pUserPacket) continue;
 
		if (UserPacket.second.TargetIP.empty())
		{
#ifdef _DEBUG
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Fail to send packet because it does not contain target IP.");
#endif
			continue;
		}

		if (boost::asio::socket_base *pSocket = __chooseSocketV(UserPacket.second))
		{
			IPPacketSet.clear();         //TODO: 是否可以不释放IPPacketSet中的内容而复用已分配的内存空间
			m_UserPacketSplitter.splitUserPacket(pUserPacket, IPPacketSet);

			NumBytesSend = __sendIPPacketSetV(IPPacketSet, pUserPacket, UserPacket.second, pSocket);  

			if (m_ErrorCode)   			       
			{
#ifdef _DEBUG
				hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR2("Fail to send data by [%1%]: %2%.", pHostNode->getName(), m_ErrorCode.message()));
#endif
				_HIVE_SIMPLE_IF_ELSE((__onSendingDataFailV(m_ErrorCode, pSocket) == CONTINUE_THREAD), continue, break);
			}
			for (unsigned int i=0; i<IPPacketSet.size(); i++) IPPacketSet[i].release();
		}
		else
		{
#ifdef _DEBUG
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "Fail to send packet because no proper socket can be found to conduct sending.");
#endif
		}

		if (UserPacket.second.DeletePacketAfterSendingHint) delete pUserPacket;
	
		Counter++;
		if (Counter % CHECK_OUTGOING_PACKET_QUEUE_FREQUENCY == 0)
		{
			unsigned int NumUnsendPacket = pHostNode->_getNumUnsendPacket();
			_HIVE_SIMPLE_IF((NumUnsendPacket > OUTGOING_PACKET_QUEUE_JAM_SIZE),
				hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR2("Too many outgoing packets [%1%] are jammed in node [%2%].", NumUnsendPacket, pHostNode->getName())));
		}
	}
}