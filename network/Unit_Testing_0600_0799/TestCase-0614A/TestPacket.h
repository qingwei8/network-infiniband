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
	CTestPacket(const CTestPacket* vOther);
	~CTestPacket(void);

	void creatBuffer(int vBufferSize); 
	void setAllPacketsNum(int vNums) {m_AllPacketsNum = vNums;}

	virtual void processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override;
	virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;

	static int m_SuccessfulReceivedPacketsNum;

private:
	std::string m_Buffer;
	int m_BufferSize;

	int m_AllPacketsNum;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		vAr & m_Buffer;
		vAr & m_BufferSize;
		vAr & m_AllPacketsNum;
	}

	friend class boost::serialization::access;
};
