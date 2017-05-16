#pragma once
#include "Packet.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;
}

struct SData
{
public:
	int m_Int;
	double m_Double;
	char m_Char;
	std::string m_String;

private:
	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		//vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		vAr & m_String;
		vAr & m_Double;
		vAr & m_Int;
		vAr & m_Char;
	}

	friend class boost::serialization::access;
};

class CTestPacket : public hiveNetworkCommon::IUserPacket
{
public:
	CTestPacket(void);
	CTestPacket(const std::string& vSig);
	~CTestPacket(void);
	
	void setData(const SData& vData) {m_Data = vData;}
	void setMessage(const std::string& vMessage) {m_Message = vMessage;}

	virtual void processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override; //Q : 为什么要加这个参数

private:
	SData m_Data;
	std::string m_Message;
	double m_Double;
	int    m_Int;
	char   m_Char;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		vAr & m_Data;
		vAr & m_Message;
		vAr & m_Double;
		vAr & m_Int;
		vAr & m_Char;
	}

	friend class boost::serialization::access;
};