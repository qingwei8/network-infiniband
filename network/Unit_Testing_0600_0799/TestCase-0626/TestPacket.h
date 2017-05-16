#pragma once
#include "../../common/Packet.h"
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "CSNetworkTopologyInterface.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

class CTestPacket : public hiveNetworkCommon::IUserPacket
{
public:
	CTestPacket(void);
	CTestPacket(const CTestPacket* vOther);
	~CTestPacket(void);

	void setMessage(const std::string& vMessage) {m_Message = vMessage;}
	virtual void processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override;
	virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;

private:
	std::string m_Message;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		vAr & m_Message;
	}

	friend class boost::serialization::access;
};