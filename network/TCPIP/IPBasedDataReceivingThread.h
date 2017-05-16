#pragma once
#include "NetworkThread.h"
#include <boost/asio/socket_base.hpp>
#include "UserPacketAssembler.h"
#include "TCPCommon.h"

namespace hiveTCPIP
{
//NOTE: ע��ֻ����ͬ����ʽ�£��Ż��н����߳�
	class IIPBasedDataReceivingThread : public hiveNetworkCommon::INetworkThread
	{
	public:
		IIPBasedDataReceivingThread();
		virtual ~IIPBasedDataReceivingThread();

		void initReceiveBuffer(unsigned int vBufferSize);
		void setSocket(boost::asio::socket_base *vSocket) {_ASSERTE(vSocket && !m_pSocket); m_pSocket = vSocket;}

	protected:
		boost::system::error_code m_ErrorCode;

	private:
		boost::asio::socket_base *m_pSocket;  //��ͬ����ʽ�£������̺߳ͽ���socket����һһ��Ӧ��������һ���߳����������еĶ˿ڵ�����
		SRawDataBuffer		 m_ReceiveBuffer;
		CUserPacketAssembler m_UserPacketAssembler;
		SRawDataBuffer       m_DanglingIPPacket;
		unsigned int         m_NumUnreceivedBytesOfDanglingIPPacket;
		
		virtual void __runV() override;
		virtual bool __isWholeIPPacketReceivedV() = 0;
		virtual int  __receivingDataV(boost::asio::socket_base *vSocket, SRawDataBuffer& voReceiveBuffer) = 0;

		virtual EOpOnNetworkFailure __onReceivingDataFailV(const boost::system::error_code& vErrorCode, boost::asio::socket_base *vSocket) {return CONTINUE_THREAD;}

		unsigned int __handleBoundaryDanglingPacket(const char *vReceiveBuffer);
		void __extractIPPacketSet(unsigned int vNumBytesReceived, std::vector<SRawDataBuffer>& voOutput);
		void __createDanglingIPPacket(unsigned int vDanglingIPPacketSize, unsigned int vNumAvailableBytesInBuffer, const char *vReceiveBuffer);
		void __addLastDanglingPacket(const char *vReceiveBuffer, unsigned int vNumBytesReceived);
		void __mergeBoundaryDanglingPacket2NewDanglingPacket(const char *vReceiveBuffer, unsigned int vNumBytesExtracted, unsigned int vNumBytesReceived);

		unsigned int __handleLastDanglingPacket(const char *vReceiveBuffer, std::vector<SRawDataBuffer>& voOutput);
	};
}