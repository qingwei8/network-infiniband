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

	virtual void processV(hiveNetworkCommon::CLogicalNode *vHostNode) override;
	void calRate();

private:
	std::string m_Message;
	std::string m_MD5;
	clock_t m_StartTime;
	unsigned int m_PacketNum;
	long double m_TotalPacketOfPast;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		vAr & m_Message;
		vAr & m_MD5;
		vAr & m_StartTime;
		vAr & m_PacketNum;
		vAr & m_TotalPacketOfPast;
	}

	friend class boost::serialization::access;
};