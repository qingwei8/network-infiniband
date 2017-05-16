#pragma once
#include "../../common/Packet.h"

class CTestPacket : public hiveNetworkCommon::IUserPacket
{
public:
	CTestPacket(void);
	CTestPacket(const std::string& vSig);
	CTestPacket(const CTestPacket* vOther);
	~CTestPacket(void);

	void setMessage(const char* vMessage) {m_Message = std::string(vMessage);}

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