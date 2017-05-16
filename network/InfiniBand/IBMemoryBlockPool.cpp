#include "IBMemoryBlockPool.h"
#include <boost\format.hpp>
#include <boost\algorithm\string.hpp>
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "IBMemoryBlock.h"

using namespace hiveInfiniband;

CIBMemoryBlockPool::CIBMemoryBlockPool(void)
{
}


CIBMemoryBlockPool::~CIBMemoryBlockPool(void)
{
}

//*******************************************************************
//FUNCTION:
void CIBMemoryBlockPool::initMemoryPool(ib_pd_handle_t vProtectionDomain, unsigned int vNumBlcok, unsigned int vBlockSize)
{
	_ASSERT(vProtectionDomain > 0);

	m_ProtectionDomain = vProtectionDomain;
	for (unsigned int i=0; i<vNumBlcok; ++i)
	{
		CIBMemoryBlock *pMemoryBlock = new CIBMemoryBlock;
		pMemoryBlock->createMemoryBlock(m_ProtectionDomain, vBlockSize);
		m_ReceivePool.Pool.push_back(pMemoryBlock);

		pMemoryBlock = new CIBMemoryBlock;
		pMemoryBlock->createMemoryBlock(m_ProtectionDomain, vBlockSize);
		m_SendPool.Pool.push_back(pMemoryBlock);
	}
}

//*******************************************************************
//FUNCTION:
CIBMemoryBlock* CIBMemoryBlockPool::requestMemoryBlock(unsigned int vBlockSize, unsigned int vWorkRequestID, EWorkRequestType vWorkReqType)
{
	_ASSERTE((vWorkReqType == SEND_TYPE) || (vWorkReqType == RECV_TYPE));

	SMemoryBlockPool& BlockPool = (vWorkReqType == SEND_TYPE) ? m_SendPool : m_ReceivePool;
	CIBMemoryBlock *pBlock = BlockPool.requestMemoryBlock(vBlockSize);

	if (!pBlock)
	{
		_ASSERTE(vWorkRequestID > BlockPool.Pool.size());
		pBlock = new CIBMemoryBlock;
		pBlock->createMemoryBlock(m_ProtectionDomain, vBlockSize);
		BlockPool.addNewMemoryBlock(pBlock);
		hiveCommon::hiveOutputEvent("A new memory block is created because the block pool is exhausted.");
	}

	pBlock->lockIBMemoryBlock(vWorkRequestID, vWorkReqType);
	return pBlock;
}


//*********************************************************************************
//FUNCTION:
CIBMemoryBlock* SMemoryBlockPool::requestMemoryBlock(unsigned int vBlockSize)
{
	CIBMemoryBlock *pResult = nullptr;

	for (unsigned int i=0; i<Pool.size(); i++)
	{
		CIBMemoryBlock *pBlock = Pool[(i+NextAvailableID)%Pool.size()];
		if ((pBlock->getCapacity() >= vBlockSize) && !pBlock->isLocked())
		{
			pResult = pBlock;
			NextAvailableID++;     //ASSUMPTION: 这里假设本函数只在一个线程被调用，因此不用考虑NextAvailableID的线程安全问题
			break;
		}
	}

	return pResult;
}

//*********************************************************************************
//FUNCTION:
void CIBMemoryBlockPool::destroyMemoryPool()
{
	m_ReceivePool.releaseAllMemoryBlock();
	m_SendPool.releaseAllMemoryBlock();
	hiveCommon::hiveOutputEvent("All memory blocks have been destroyed.");
}

//*********************************************************************************
//FUNCTION:
void SMemoryBlockPool::releaseAllMemoryBlock()
{
	for (unsigned int i = 0; i < Pool.size(); i++)
	{
		Pool[i]->closeIBMemoryBlock();
		delete Pool[i];
	}
	Pool.clear();
}

//*********************************************************************************
//FUNCTION:
CIBMemoryBlock* SMemoryBlockPool::findMemoryBlock(unsigned int vWorkRequestID)
{
	boost::mutex::scoped_lock Lock(Mutex);
	for (unsigned int i=0; i<Pool.size(); i++)
	{
		if (Pool[i]->getWorkRequestID() == vWorkRequestID) return Pool[i];
	}
	return nullptr;
}

//*********************************************************************************
//FUNCTION:
CIBMemoryBlock* CIBMemoryBlockPool::findMemoryBlock(unsigned int vWorkRequestID, EWorkRequestType vWorkReqType)
{
	_ASSERTE((vWorkReqType == SEND_TYPE) || (vWorkReqType == RECV_TYPE));

	SMemoryBlockPool& BlockPool = (vWorkReqType == SEND_TYPE) ? m_SendPool : m_ReceivePool;
	return BlockPool.findMemoryBlock(vWorkRequestID);
}

//*********************************************************************************
//FUNCTION:
void SMemoryBlockPool::addNewMemoryBlock(CIBMemoryBlock *vBlock)
{
	_ASSERTE(vBlock && !vBlock->isLocked());

	boost::mutex::scoped_lock Lock(Mutex);
	Pool.push_back(vBlock);
	NextAvailableID = 0;
}