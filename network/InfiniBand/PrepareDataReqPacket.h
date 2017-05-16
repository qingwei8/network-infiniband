#pragma once
#include "Packet.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

namespace hiveInfiniband
{
	class CPrepareDataReqPacket : public hiveNetworkCommon::IUserPacket
	{
	public:
		CPrepareDataReqPacket();
		CPrepareDataReqPacket(const CPrepareDataReqPacket* vPacket);
		virtual ~CPrepareDataReqPacket();

		void setBufferSize(unsigned int vBufferSize)   { _ASSERT(m_BufferSize == 0); m_BufferSize = vBufferSize; }
		void setSendRequestID(unsigned int vRequestID) { m_SendRequestID = vRequestID; }
		void setIBPacketSig(const std::string& vSig)   { _ASSERTE(!vSig.empty() && m_IBPacketSig.empty()); m_IBPacketSig = vSig; }

		unsigned int getBufferSize() const    { return m_BufferSize; }
		unsigned int getSendRequestID() const { return m_SendRequestID; }

		virtual void processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override;

	private:
		unsigned int m_BufferSize;
		unsigned int m_SendRequestID;  //���ݷ��Ͷ˿���ͬʱ�����ڵ㷢�Ͷ��IB���ݰ�����������һ�����⣬�����յ�һ��CPrepareDataAckPacket��������֪�����������Ӧ��ǰ���������ĸ�
		                               //CPrepareDataReqPacket��Ϊ�˽��������⣬���ǽ�����IB���ݰ����������Ψһ��work request ID������Ա������һ�����͸����ݽ��նˣ����ݽ��ն�
									   //����CPrepareDataAckPacket�лش����ֵ���������Ͷ˾�֪��CPrepareDataAckPacket����Ӧ����
		std::string m_IBPacketSig;

		template<class TArchieve>
		void serialize(TArchieve & ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
			ar & m_BufferSize;
			ar & m_SendRequestID;
			ar & m_IBPacketSig;
			_ASSERTE(!m_IBPacketSig.empty());
		}
		friend class boost::serialization::access;
	};
}