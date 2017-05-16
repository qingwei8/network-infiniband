#pragma once
#include "../../common/Packet.h"
#include <boost/serialization/array.hpp>

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

	virtual void processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override;
	
private:
	unsigned int m_ImageDataSize;
	unsigned char* m_pImageData;
	time_t m_LastTime;
	time_t m_TotalTime;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & m_ImageDataSize;
		vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		if (!m_pImageData) m_pImageData = new unsigned char[m_ImageDataSize];
		vAr & boost::serialization::make_array(m_pImageData, m_ImageDataSize);
	}

	friend class boost::serialization::access;
};