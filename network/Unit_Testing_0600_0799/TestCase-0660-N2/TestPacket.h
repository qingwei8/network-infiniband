#pragma once
#include "../../common/Packet.h"

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
	
	void setMessage(const std::string& vMessage) {m_Message = vMessage;}

	virtual void processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override;

private:
	std::string m_Message;
	double m_Double;
	int    m_Int;
	char   m_Char;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		vAr & m_Message;
		vAr & m_Double;
		vAr & m_Int;
		vAr & m_Char;
	}

	friend class boost::serialization::access;
};