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
	class INFINIBAND_DLL_EXPORT CBuildCIBConnectionReqPacket : public hiveNetworkCommon::IUserPacket
	{
	public:
		CBuildCIBConnectionReqPacket();
		CBuildCIBConnectionReqPacket(const CBuildCIBConnectionReqPacket *vPacket);
		virtual ~CBuildCIBConnectionReqPacket();
		
		void setRequesterInfo(ib_net16_t vRequesterLID, const std::string& vRequesterNodeName, const std::vector<ib_net32_t>& vRequesterQueuePairIDSet)
		{
			_ASSERTE(!vRequesterQueuePairIDSet.empty());
			m_RequesterLID		    = vRequesterLID;
			m_RequesterNodeName	    = vRequesterNodeName;
			m_RequesterQueuePairSet = vRequesterQueuePairIDSet;
		}

		virtual void     processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override;
		virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;

		const std::string& getRequireNodeName() const {return m_RequesterNodeName;}

	private:
		std::string  m_RequesterNodeName;
		ib_net16_t   m_RequesterLID;
		std::vector<ib_net32_t> m_RequesterQueuePairSet;

		template<class TArchieve>
		void serialize(TArchieve & ar, const unsigned int version)
		{
			ar & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
			ar & m_RequesterNodeName;
			ar & m_RequesterLID;
			ar & m_RequesterQueuePairSet;
		}

	friend class boost::serialization::access;
	};
}