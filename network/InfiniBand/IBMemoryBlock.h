#pragma once
#include "IBCommon.h"

namespace hiveInfiniband
{
	class CIBMemoryBlock
	{
	public:
		CIBMemoryBlock(void);
		~CIBMemoryBlock(void);

		uint32_t getCapacity()            const {return m_Capacity;}
		uint32_t getLocalKey()            const {return m_LocalKey;}
		EWorkRequestType getWorkReqType() const {return m_WorkReqType;}
		unsigned char* getAddr()          const {return m_pBuffer;}
		unsigned int getUsedBufferSize()  const {return m_IBDataSize;}
		unsigned int getWorkRequestID()   const {return m_WorkRequestID;}

		void setIBDataSize(unsigned int vSize) { _ASSERTE(vSize <= m_Capacity);  m_IBDataSize = vSize; }
		void closeIBMemoryBlock();
		void copyBuffer(unsigned int vSourceBufferSize, const unsigned char *vSourceBuffer);
		void lockIBMemoryBlock(unsigned int vWorkRequestID, EWorkRequestType vWorkReqType);
		void unlockIBMemoryBlock() { _ASSERTE(m_IsLocked); m_IsLocked = false; m_WorkRequestID = UINT_MAX; m_IBDataSize = 0; }

		bool createMemoryBlock(ib_pd_handle_t vProtectionDomain, uint32_t vBlockSize);
		bool isLocked();

	private:
		ib_mr_handle_t   m_MemoryRegion;
		ib_pd_handle_t   m_ProtectionDomain;
		uint32_t         m_LocalKey;
		uint32_t         m_RemoteKey;
		uint32_t         m_Capacity;
		bool             m_IsLocked;   //ASSUMPTION: ����ֻ�Ǽ򵥵�����һ��bool�����������ź�������鵱ǰblock�Ƿ�����ʹ�ã����Ǹ�Ч���̲߳���ȫ��ʵ��
		unsigned int	 m_IBDataSize;
		unsigned int     m_NumUnavailableQuery;
		unsigned int     m_WorkRequestID;  //NOTE: ÿ�η��ͺͽ���IB���ݣ�������һ��Ψһ��ID��ͨ�����ID��������ȷ����֮��Ӧ��CIBMemoryBlock����
		unsigned char   *m_pBuffer;
		EWorkRequestType m_WorkReqType;

		friend class CIBMemoryBlockPool;
	};
}