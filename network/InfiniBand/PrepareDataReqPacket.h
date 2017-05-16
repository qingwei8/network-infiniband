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
		unsigned int m_SendRequestID;  //数据发送端可能同时向多个节点发送多个IB数据包，这样导致一个问题，当它收到一个CPrepareDataAckPacket后，它并不知道这个包是响应以前它发出的哪个
		                               //CPrepareDataReqPacket。为了解决这个问题，我们将发送IB数据包所必须的且唯一的work request ID（本成员变量）一并发送给数据接收端，数据接收端
									   //会在CPrepareDataAckPacket中回传这个值，这样发送端就知道CPrepareDataAckPacket的响应对象
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