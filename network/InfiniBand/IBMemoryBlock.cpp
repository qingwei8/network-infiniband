#include "IBMemoryBlock.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"

using namespace hiveInfiniband;

CIBMemoryBlock::CIBMemoryBlock(void): m_NumUnavailableQuery(0), m_Capacity(0), m_MemoryRegion(0), m_LocalKey(0), m_RemoteKey(0), m_pBuffer(nullptr), m_IBDataSize(0), m_WorkRequestID(UINT_MAX), m_IsLocked(false)
{
}


CIBMemoryBlock::~CIBMemoryBlock(void)
{
}

//*******************************************************************
//FUNCTION:
bool CIBMemoryBlock::createMemoryBlock(ib_pd_handle_t vProtectionDomain, uint32_t vBlockSize)
{
	_ASSERT(vProtectionDomain > 0 && vBlockSize > 0);

	m_MemoryRegion     = 0 ;
	m_LocalKey         = 0 ; 
	m_RemoteKey        = 0 ;
	m_ProtectionDomain = vProtectionDomain;
	m_Capacity		   = vBlockSize;

	m_pBuffer = new unsigned char[m_Capacity];

	ib_mr_create_t  MemRegionCreate;
	MemRegionCreate.length      = m_Capacity;
	MemRegionCreate.vaddr       = m_pBuffer;
	MemRegionCreate.access_ctrl = IB_AC_LOCAL_WRITE;

	_HIVE_CALL_IB_FUNC(ib_reg_mem(m_ProtectionDomain, &MemRegionCreate, &m_LocalKey, &m_RemoteKey, &m_MemoryRegion), "ib_reg_mem()", false);
#ifdef _DEBUG
	hiveCommon::hiveOutputEvent(_BOOST_STR3("A memory block [%1%] has been registered to IB with local key [%2%] and remote key [%3%].", vBlockSize, m_LocalKey, m_RemoteKey));
#endif

	return true;
}

//*******************************************************************
//FUNCTION:
bool CIBMemoryBlock::isLocked()
{
#ifdef _DEBUG
	if (m_IsLocked)
	{
		m_NumUnavailableQuery++;
		if (m_NumUnavailableQuery > 10000)
		{
			hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR1("One memory block with size [%1%] has been locked for a very long time.", m_Capacity));
			m_NumUnavailableQuery = 0;
		}
		return true;
	}
	else
	{
		m_NumUnavailableQuery = 0;
		return false;
	}
#else
	return m_IsLocked;
#endif
}

//*******************************************************************
//FUNCTION:
void CIBMemoryBlock::copyBuffer(unsigned int vSourceBufferSize, const unsigned char *vSourceBuffer)
{
	_ASSERTE((vSourceBufferSize <= m_Capacity) && (vSourceBufferSize > 0) && vSourceBuffer);
	memcpy_s(m_pBuffer, m_Capacity, vSourceBuffer, vSourceBufferSize);
	setIBDataSize(vSourceBufferSize);
}

//*********************************************************************************
//FUNCTION:
void CIBMemoryBlock::closeIBMemoryBlock()
{
	if (m_MemoryRegion)
	{
		_HIVE_CALL_IB_FUNC2(ib_dereg_mr(m_MemoryRegion), "ib_dereg_mr");
		m_MemoryRegion = 0;
	}
	m_LocalKey = 0;
	m_RemoteKey = 0;

	_SAFE_DELETE_ARRAY(m_pBuffer);
}

//*********************************************************************************
//FUNCTION:
void CIBMemoryBlock::lockIBMemoryBlock(unsigned int vWorkRequestID, EWorkRequestType vWorkReqType)
{
	_ASSERTE(!m_IsLocked && (m_WorkRequestID == UINT_MAX));
	m_WorkRequestID = vWorkRequestID;
	m_WorkReqType = vWorkReqType;
	m_IsLocked = true;
}