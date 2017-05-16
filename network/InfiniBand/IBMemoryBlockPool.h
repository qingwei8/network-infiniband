#pragma once
#include <vector>
#include <boost/thread/mutex.hpp>
#include "IBCommon.h"

namespace hiveInfiniband
{
	class CIBMemoryBlock;

	struct SMemoryBlockPool
	{
		std::vector<CIBMemoryBlock*> Pool;
		unsigned int NextAvailableID;
		boost::mutex Mutex;

		SMemoryBlockPool() : NextAvailableID(0) {}

		CIBMemoryBlock* requestMemoryBlock(unsigned int vBlockSize);
		CIBMemoryBlock* findMemoryBlock(unsigned int vWorkRequestID);

		void releaseAllMemoryBlock();
		void addNewMemoryBlock(CIBMemoryBlock *vBlock);
	};

	class CIBMemoryBlockPool
	{
	public:
		CIBMemoryBlockPool(void);
		~CIBMemoryBlockPool(void);

		void initMemoryPool(ib_pd_handle_t vProtectionDomain, unsigned int vNumBlcok, unsigned int vBlockSize);
		void destroyMemoryPool();
		
		CIBMemoryBlock* requestMemoryBlock(unsigned int vBlockSize, unsigned int vWorkRequestID, EWorkRequestType vWorkReqType);
		CIBMemoryBlock* findMemoryBlock(unsigned int vWorkRequestID, EWorkRequestType vWorkReqType);

	private:
		ib_pd_handle_t	 m_ProtectionDomain;
		SMemoryBlockPool m_ReceivePool;
		SMemoryBlockPool m_SendPool;
	};
}