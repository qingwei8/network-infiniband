#pragma once
#include "Packet.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "CSNetworkTopologyInterface.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

class CTestReplyPacket : public hiveNetworkCommon::CPacket
{
public:
	CTestReplyPacket(void);
	CTestReplyPacket(const CTestReplyPacket* vOther);
	~CTestReplyPacket(void);

	void setMessage(const std::string& vMessage) {m_Message = vMessage;}
	virtual void processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override;
	virtual CPacket* clonePacketV() override;

private:
	std::string m_Message;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & boost::serialization::base_object<hiveNetworkCommon::CPacket>(*this);
		vAr & m_Message;
	}

	friend class boost::serialization::access;
};