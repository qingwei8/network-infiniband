#include "IPBasedDataReceivingThread.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/HiveCommonMicro.h"
#include "common/CommonInterface.h"
#include "IPBasedNode.h"
#include "TCPIPCommon.h"

using namespace hiveTCPIP;

#define IP_PACKET_BOUNDRARY_SIZE 4

IIPBasedDataReceivingThread::IIPBasedDataReceivingThread() : m_NumUnreceivedBytesOfDanglingIPPacket(0), m_pSocket(nullptr)
{
}

IIPBasedDataReceivingThread::~IIPBasedDataReceivingThread()
{
	m_ReceiveBuffer.release();
}

//******************************************************************
//FUNCTION:
void IIPBasedDataReceivingThread::initReceiveBuffer(unsigned int vBufferSize)
{
	if (m_ReceiveBuffer.isValidBuffer()) m_ReceiveBuffer.release();
	m_ReceiveBuffer = SRawDataBuffer(vBufferSize);
	m_ReceiveBuffer.clear();

	_ASSERTE(getHostLogicalNode());
	hiveCommon::hiveOutputEvent(_BOOST_STR2("The size of receiving buffer of node [%1%] is set to [%2%].", getHostLogicalNode()->getName(), vBufferSize));
}

//*********************************************************************************************************************************************************
//FUNCTION:
void IIPBasedDataReceivingThread::__runV()
{
	_ASSERTE(m_ReceiveBuffer.isValidBuffer() && fetchHostLogicalNode() && m_pSocket);

	hiveCommon::hiveOutputEvent(_BOOST_STR2("Thread [%1%] of node [%2%] starts receiving data ...", getThreadName(), getHostLogicalNode()->getName())); //如何获取端口号？ 

	hiveNetworkCommon::IUserPacket *pUserPacket = nullptr;
	CIPBasedNode *pHostNode = dynamic_cast<CIPBasedNode*>(fetchHostLogicalNode());
	_ASSERTE(pHostNode);

	std::vector<SRawDataBuffer> ReceivedIPPacketSet;
	int NumBytesReceived = 0;

	while (isRunning() && !isInterruptionRequested())
	{
		NumBytesReceived = __receivingDataV(m_pSocket, m_ReceiveBuffer);

		if (m_ErrorCode)
		{
#ifdef _DEBUG
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR3("Fail to receive data by [%1%]: %2% (%3%).", pHostNode->getName(), m_ErrorCode.message(), m_ErrorCode.value()));
#endif
			_HIVE_SIMPLE_IF_ELSE((__onReceivingDataFailV(m_ErrorCode, m_pSocket) == CONTINUE_THREAD), continue, break);
		}
		if (NumBytesReceived == 0) continue;

		if (__isWholeIPPacketReceivedV())
		{
			SRawDataBuffer ReceivedIPPacket;
			ReceivedIPPacket.incompleteShallowCopy(NumBytesReceived, m_ReceiveBuffer);
			pUserPacket = m_UserPacketAssembler.addIPPacket(ReceivedIPPacket);				
			if (pUserPacket) pHostNode->_onUserPacketReceived(pUserPacket);
		}
		else
		{
			ReceivedIPPacketSet.clear();
			__extractIPPacketSet(NumBytesReceived, ReceivedIPPacketSet);
			for (unsigned int i=0; i<ReceivedIPPacketSet.size(); i++) 
			{
				pUserPacket = m_UserPacketAssembler.addIPPacket(ReceivedIPPacketSet[i]);
				if (pUserPacket) pHostNode->_onUserPacketReceived(pUserPacket);
			}
		}
	}
}

//******************************************************************
//FUNCTION:
void IIPBasedDataReceivingThread::__extractIPPacketSet(unsigned int vNumBytesReceived, std::vector<SRawDataBuffer>& voOutput)
{
 	_ASSERTE(voOutput.empty() && m_ReceiveBuffer.isValidBuffer() && (vNumBytesReceived <= m_ReceiveBuffer.getBufferSize()));
 
 	unsigned int NumBytesExtracted = 0;
 	char *pReceiveBuffer = m_ReceiveBuffer.fetchBufferAddress();
 
 	if (m_NumUnreceivedBytesOfDanglingIPPacket > 0)
 	{
 		//NOTE YHY add: 接收到的IP包可能继续应该是悬挂包，所以不能__handle，而是应该添加在上一步创建的悬挂包中。
 		if (vNumBytesReceived < m_NumUnreceivedBytesOfDanglingIPPacket)
 		{
 			__addLastDanglingPacket(pReceiveBuffer, vNumBytesReceived);
 			return;
 		}
 
 		if (m_DanglingIPPacket.getBufferSize() == IP_PACKET_BOUNDRARY_SIZE)
 		{
 			NumBytesExtracted += __handleBoundaryDanglingPacket(pReceiveBuffer);
 			if (vNumBytesReceived - NumBytesExtracted < ((unsigned int*)m_DanglingIPPacket.fetchBufferAddress())[0])
 			{
 				__mergeBoundaryDanglingPacket2NewDanglingPacket(pReceiveBuffer, NumBytesExtracted, vNumBytesReceived);
 				return;
 			}
 		}
 		else
 			NumBytesExtracted += __handleLastDanglingPacket(pReceiveBuffer, voOutput);
 
 		_ASSERTE(NumBytesExtracted <= vNumBytesReceived);	
 		pReceiveBuffer += NumBytesExtracted;
 	}
 
 	unsigned int *pBuffer = (unsigned int*)pReceiveBuffer;
 	unsigned int NextIPPacketSize = pBuffer[0];
 
 	if (m_DanglingIPPacket.isValidBuffer())
 		NextIPPacketSize = ((unsigned int*)m_DanglingIPPacket.fetchBufferAddress())[0] - IP_PACKET_BOUNDRARY_SIZE;
 
 	while ((NextIPPacketSize + NumBytesExtracted <= vNumBytesReceived) && (NumBytesExtracted < vNumBytesReceived))
 	{
 		SRawDataBuffer IPPacket(NextIPPacketSize + m_DanglingIPPacket.getBufferSize()); //NOTE YHY add: 只有在边界时候m_DanglingIPPacket.getBufferSize()才有值, 因为普通悬挂包__handleLastDanglingPacket会reset.
 		if (m_DanglingIPPacket.isValidBuffer())
 		{
 			memcpy(IPPacket.fetchBufferAddress(), m_DanglingIPPacket.fetchBufferAddress(), IP_PACKET_BOUNDRARY_SIZE);
 			m_DanglingIPPacket.reset();
 			memcpy(IPPacket.fetchBufferAddress()+IP_PACKET_BOUNDRARY_SIZE, pReceiveBuffer, NextIPPacketSize);
 		}
 		else
 			memcpy(IPPacket.fetchBufferAddress(), pReceiveBuffer, NextIPPacketSize);
 		voOutput.push_back(IPPacket);
 
 		NumBytesExtracted += NextIPPacketSize;
 		//NOTE YHY add: 因为m_ReceivedBuffer最大初始化为TCP最大IP包的两倍，而如果发送端连续发送了大于两次的用户包，pBuffer[0]将越界
 		//				或者剩余不到4个字节也将越界
 		if (NumBytesExtracted >= vNumBytesReceived || vNumBytesReceived-NumBytesExtracted < IP_PACKET_BOUNDRARY_SIZE)
 			break;
 		pReceiveBuffer += NextIPPacketSize;
 		pBuffer = (unsigned int*)pReceiveBuffer;
 		NextIPPacketSize = pBuffer[0];
 		_ASSERTE(NextIPPacketSize <= vNumBytesReceived);
 	}
 	_ASSERTE(NumBytesExtracted <= vNumBytesReceived);
 
 	//NOTE YHY add: 假如剩余未提取的字节小于4字节，那么pBuffer[0]强制转换将越界,所以创建一个边界处理悬挂包
 	if (vNumBytesReceived - NumBytesExtracted < IP_PACKET_BOUNDRARY_SIZE && vNumBytesReceived - NumBytesExtracted > 0)
 	{
 		__createDanglingIPPacket(IP_PACKET_BOUNDRARY_SIZE, vNumBytesReceived - NumBytesExtracted, pReceiveBuffer);
 		NumBytesExtracted = vNumBytesReceived;
 	}
 
 	if (NumBytesExtracted < vNumBytesReceived)	__createDanglingIPPacket(NextIPPacketSize, vNumBytesReceived - NumBytesExtracted, pReceiveBuffer);
}

//***********************************************************
//FUNCTION:
unsigned int IIPBasedDataReceivingThread::__handleLastDanglingPacket(const char *vReceiveBuffer, std::vector<SRawDataBuffer>& voOutput)
{
	_ASSERTE(m_DanglingIPPacket.isValidBuffer());

	unsigned int TargetBufferOffset = m_DanglingIPPacket.getBufferSize() - m_NumUnreceivedBytesOfDanglingIPPacket;
	memcpy(m_DanglingIPPacket.fetchBufferAddress()+TargetBufferOffset, vReceiveBuffer, m_NumUnreceivedBytesOfDanglingIPPacket);
	
	voOutput.push_back(m_DanglingIPPacket);

	unsigned int NumBytesExtracted = m_NumUnreceivedBytesOfDanglingIPPacket;
	m_DanglingIPPacket.reset();
	m_NumUnreceivedBytesOfDanglingIPPacket = 0;

	return NumBytesExtracted;
}

//***********************************************************
//FUNCTION:
void IIPBasedDataReceivingThread::__createDanglingIPPacket(unsigned int vDanglingIPPacketSize, unsigned int vNumAvailableBytesInBuffer, const char *vReceiveBuffer)
{
	_ASSERTE(vReceiveBuffer && (vNumAvailableBytesInBuffer < vDanglingIPPacketSize));

	m_DanglingIPPacket = SRawDataBuffer(vDanglingIPPacketSize);
	memcpy(m_DanglingIPPacket.fetchBufferAddress(), vReceiveBuffer, vNumAvailableBytesInBuffer);
	m_NumUnreceivedBytesOfDanglingIPPacket = vDanglingIPPacketSize - vNumAvailableBytesInBuffer;
}

//******************************************************************
//FUNCTION:
void IIPBasedDataReceivingThread::__addLastDanglingPacket(const char *vReceiveBuffer, unsigned int vNumBytesReceived)
{
	_ASSERTE(vReceiveBuffer && m_DanglingIPPacket.isValidBuffer());
	unsigned int TargetBufferOffset = m_DanglingIPPacket.getBufferSize() - m_NumUnreceivedBytesOfDanglingIPPacket;
	_ASSERTE(vNumBytesReceived < m_NumUnreceivedBytesOfDanglingIPPacket);

	memcpy(m_DanglingIPPacket.fetchBufferAddress()+TargetBufferOffset, vReceiveBuffer, vNumBytesReceived);
	m_NumUnreceivedBytesOfDanglingIPPacket -= vNumBytesReceived;
}

//*********************************************************************************
//FUNCTION:
void hiveTCPIP::IIPBasedDataReceivingThread::__mergeBoundaryDanglingPacket2NewDanglingPacket(const char *vReceiveBuffer, unsigned int vNumBytesExtracted, unsigned int vNumBytesReceived)
{
	_ASSERTE(vReceiveBuffer && m_DanglingIPPacket.isValidBuffer() && vNumBytesExtracted < vNumBytesReceived);
	SRawDataBuffer BoundaryDanglingPacket(m_DanglingIPPacket.getBufferSize());
	memcpy(BoundaryDanglingPacket.fetchBufferAddress(), m_DanglingIPPacket.fetchBufferAddress(), m_DanglingIPPacket.getBufferSize());

	unsigned int NextPacketSize = ((unsigned int*)m_DanglingIPPacket.fetchBufferAddress())[0];
	m_DanglingIPPacket.reset();
	m_DanglingIPPacket = SRawDataBuffer(NextPacketSize);
	memcpy(m_DanglingIPPacket.fetchBufferAddress(), BoundaryDanglingPacket.fetchBufferAddress(), BoundaryDanglingPacket.getBufferSize());
	memcpy(m_DanglingIPPacket.fetchBufferAddress()+BoundaryDanglingPacket.getBufferSize(), vReceiveBuffer+vNumBytesExtracted, vNumBytesReceived-vNumBytesExtracted);

	m_NumUnreceivedBytesOfDanglingIPPacket = NextPacketSize - (vNumBytesReceived -vNumBytesExtracted+BoundaryDanglingPacket.getBufferSize());
}

//*********************************************************************************
//FUNCTION:
unsigned int hiveTCPIP::IIPBasedDataReceivingThread::__handleBoundaryDanglingPacket(const char *vReceiveBuffer)
{
	_ASSERTE(m_DanglingIPPacket.isValidBuffer() && vReceiveBuffer);

	unsigned int TargetBufferOffset = m_DanglingIPPacket.getBufferSize() - m_NumUnreceivedBytesOfDanglingIPPacket;
	memcpy(m_DanglingIPPacket.fetchBufferAddress()+TargetBufferOffset, vReceiveBuffer, m_NumUnreceivedBytesOfDanglingIPPacket);
	unsigned int NumBytesExtracted = m_NumUnreceivedBytesOfDanglingIPPacket;
	m_NumUnreceivedBytesOfDanglingIPPacket = 0;

	return NumBytesExtracted;
}