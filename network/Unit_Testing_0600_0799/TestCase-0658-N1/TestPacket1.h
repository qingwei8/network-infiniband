#pragma once
#include "../../common/Packet.h"
#include "common/CommonInterface.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

class CTestPacket1 : public hiveNetworkCommon::IUserPacket
{
public:
	CTestPacket1(void);
	CTestPacket1(const std::string& vSig);
	~CTestPacket1(void);
	
	void setMessage(const std::string& vMessage) {m_Message = vMessage; m_MD5 = hiveCommon::hiveComputeMD5(m_Message);}

	unsigned int getMessageSize() const {return m_Message.size();}

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