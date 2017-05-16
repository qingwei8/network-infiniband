#pragma once
#include <boost/serialization/string.hpp>
#include <boost/serialization/vector.hpp>
#include <boost/algorithm/string.hpp>
#include "Packet.h"
#include "IBCommon.h"
#include "InfinibandExport.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

namespace hiveInfiniband
{
	class INFINIBAND_DLL_EXPORT CBuildCIBConnectionAckPacket : public hiveNetworkCommon::IUserPacket
	{
	public:
		CBuildCIBConnectionAckPacket();
		CBuildCIBConnectionAckPacket(const CBuildCIBConnectionAckPacket *vPacket);
		virtual ~CBuildCIBConnectionAckPacket();

		void setAckInfo(ib_net16_t vAckLocalID, std::vector<ib_net32_t>& vAckQueuePairIDSet, const std::string& vAckNodeName)
		{
			_ASSERTE(vAckLocalID > 0);
			m_AckLID		    = vAckLocalID;
			m_AckNodeName		= vAckNodeName;
			m_AckQueuePairIDSet = vAckQueuePairIDSet;
		}

		virtual void processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override;

		virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;

	private:
		std::string  m_AckNodeName;
		ib_net16_t   m_AckLID;
		std::vector<ib_net32_t> m_AckQueuePairIDSet;

		template<class TArchieve>
		void serialize(TArchieve & ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
			ar & m_AckNodeName;
			ar & m_AckLID;
			ar & m_AckQueuePairIDSet;
		}

		friend class boost::serialization::access;
	};
}