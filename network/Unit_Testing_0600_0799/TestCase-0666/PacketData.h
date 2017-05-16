#pragma once
#include "packet.h"
#include "common/ComputerInfo.h"
#include "common/ComputerInfoInterface.h"

class CPacketData :	public hiveNetworkCommon::IUserPacket
{
public:
	CPacketData(void);
	CPacketData(const CPacketData* vOther);
	virtual ~CPacketData(void);

	virtual void     fillPacketV(hiveNetworkCommon::CLogicalNode* vHostNode) override;
	virtual void     processPacketV(hiveNetworkCommon::CLogicalNode* vHostNode) override;
	virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;

private:
	hiveComputerInfo::SComputerInfo   m_SComputerInfo;
	hiveComputerInfo::SComputerStatus m_SComputerStatus;
	std::string                       m_Message;

	bool		  m_HasGPU;
	double		  m_MemorySizeGB;
	double		  m_CPUBaseFrequency;
	unsigned char m_NumCPUProcessors;
	std::string	  m_CPUType;

	double        m_CpuAvailableRate;
	double        m_MemoryAvailableRate;
	double        m_MemoryRemainingGB;

	template<class TArchieve>
	void serialize(TArchieve& vAr, const unsigned int vVersion)
	{
		vAr & boost::serialization::base_object<hiveNetworkCommon::IUserPacket>(*this);
		vAr & m_HasGPU;
		vAr & m_MemorySizeGB;
		vAr & m_CPUBaseFrequency;
		vAr & m_NumCPUProcessors;
		vAr & m_CPUType;
		vAr & m_Message;

		vAr & m_CpuAvailableRate;
		vAr & m_MemoryAvailableRate;
		vAr & m_MemoryRemainingGB;
	}
	friend class boost::serialization::access;
};

