#pragma once
#include "Packet.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}
namespace hiveInfiniband
{
	class CPrepareDataReqPacket;
	class CPrepareDataAckPacket : public hiveNetworkCommon::IUserPacket
	{
	public:
		CPrepareDataAckPacket();
		CPrepareDataAckPacket(const CPrepareDataAckPacket* vPacket);
		~CPrepareDataAckPacket();

		virtual void processPacketV(hiveNetworkCommon::CLogicalNode* vHostNode) override;

		unsigned int getSendRequestID() const { return m_SendRequestID; }

		void setSendRequestID(unsigned int vRequestID) { m_SendRequestID = vRequestID; }

	private:
		unsigned int m_SendRequestID;

		template<class TArchieve>
		void serialize(TArchieve & ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
			ar & m_SendRequestID;
		}
		friend class boost::serialization::access;
	};
}