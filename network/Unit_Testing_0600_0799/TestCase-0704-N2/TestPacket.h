#pragma once
#include "../../common/Packet.h"
#include "common/CommonInterface.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

class CTestPacket : public hiveNetworkCommon::IUserPacket
{
public:
	CTestPacket(void);
	CTestPacket(const std::string& vSig);
	~CTestPacket(void);

	void setMessage(const char* vMessage) { m_Message = std::string(vMessage); m_MD5 = hiveCommon::hiveComputeMD5(m_Message); }
	
	virtual void processV(hiveNetworkCommon::CLogicalNode *vHostNode) override;

private:
	std::string m_Message;
	std::string m_MD5;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		vAr & m_Message;
		vAr & m_MD5;
	}

	friend class boost::serialization::access;
};