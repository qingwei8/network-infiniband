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
		bool             m_IsLocked;   //ASSUMPTION: 这里只是简单的用了一个bool变量而不是信号量来检查当前block是否正被使用，这是高效但线程不安全的实现
		unsigned int	 m_IBDataSize;
		unsigned int     m_NumUnavailableQuery;
		unsigned int     m_WorkRequestID;  //NOTE: 每次发送和接收IB数据，都会有一个唯一的ID，通过这个ID可以用来确定与之对应的CIBMemoryBlock对象
		unsigned char   *m_pBuffer;
		EWorkRequestType m_WorkReqType;

		friend class CIBMemoryBlockPool;
	};
}