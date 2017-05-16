#include "PacketData.h"
#include "common/ProductFactory.h"
#include "CSNetworkTopologyInterface.h"

hiveCommon::CProductFactory<CPacketData> theCreator("PACKETDATA_SIG");

CPacketData::CPacketData(void)
{
	fillPacketV(NULL);
}

CPacketData::CPacketData(const CPacketData* vOther) : hiveNetworkCommon::IUserPacket(vOther)
{
	m_Message = vOther->m_Message;
}

CPacketData::~CPacketData(void)
{
}

//******************************************************************
//FUNCTION:
void CPacketData::fillPacketV(hiveNetworkCommon::CLogicalNode* vHostNode)
{		
	m_SComputerInfo   = hiveComputerInfo::hiveGetComputerInfo();
	m_SComputerStatus = hiveComputerInfo::hiveGetComputerStatus();

	m_HasGPU              = m_SComputerInfo.HasGPU;
	m_MemorySizeGB        = m_SComputerInfo.MemorySizeGB;
	m_CPUBaseFrequency    = m_SComputerInfo.CPUBaseFrequency;
	m_NumCPUProcessors    = m_SComputerInfo.NumCPUProcessors;
	m_CPUType             = m_SComputerInfo.CPUType;

	m_CpuAvailableRate    = m_SComputerStatus.CpuAvailableRate;
	m_MemoryAvailableRate = m_SComputerStatus.MemoryAvailableRate;
	m_MemoryRemainingGB   = m_SComputerStatus.MemoryRemainingGB;
}

//******************************************************************
//FUNCTION:
void CPacketData::processPacketV(hiveNetworkCommon::CLogicalNode* vHostNode)
{
	if(m_HasGPU)
		std::cout << "This computer has GPU..." << std::endl;
	std::cout << m_MemorySizeGB << std::endl;
	std::cout << m_CPUBaseFrequency << std::endl;
	std::cout << m_NumCPUProcessors << std::endl;
	std::cout << m_CPUType << std::endl;

	std::cout << m_CpuAvailableRate << std::endl;
	std::cout << m_MemoryAvailableRate << std::endl;
	std::cout << m_MemoryRemainingGB << std::endl;

	std::cout << std::endl;
	std::cout << "Data Received Succeed!" << std::endl;
	std::cout << std::endl;
}

//******************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* CPacketData::clonePacketV()
{
	return new CPacketData(this);
}