#pragma once
#include "../../common/Packet.h"
#include "common/CommonInterface.h"
#include <numeric>

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

	void setMessage(const std::string& vMessage) {m_Message = vMessage; m_MD5 = hiveCommon::hiveComputeMD5(m_Message);}
	void setStartTime(clock_t vTime) {m_StartTime = vTime;}
	unsigned int getMessageSize() const {return m_Message.size();}
	void setPacketNum(int vNum) {m_PacketNum = vNum;}
	void setTotalPacketOfPast(long double vTotalPacket) {m_TotalPacketOfPast = vTotalPacket;} 
	
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