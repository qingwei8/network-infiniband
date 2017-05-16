#pragma once
#include "Packet.h"
#include <vector>
#include "iba/ib_types.h"

namespace hivNetworkCommon
{
	class CLogicalNode;
}

class CTestPacket : public hiveNetworkCommon::IUserPacket
{
public:
	CTestPacket(void);
	CTestPacket(const std::string& vSig);
	~CTestPacket(void);

	void setReqQueuePairIDSet(const std::vector<ib_net32_t>& vReqQueuePairIDSet) {m_ReqQueuePairIDSet = vReqQueuePairIDSet;}

private:
	ib_net16_t m_ReqLID;
	std::string m_ReqNodeName;
	std::vector<ib_net32_t> m_ReqQueuePairIDSet;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		vAr & m_ReqLID;
		vAr & m_ReqNodeName;
		vAr & m_ReqQueuePairIDSet;
	}

	friend class boost::serialization::access;
};