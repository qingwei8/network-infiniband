#include "IBCompletionQueue.h"
#include <boost/format.hpp>
#include <boost/algorithm/string.hpp>
#include "common\CommonInterface.h"
#include "IBWCEHandlingThread.h"
#include "IBConnection.h"

using namespace hiveInfiniband;

CIBCompletionQueue::CIBCompletionQueue(void): m_CompletionEvent(0), m_CompletionQueue(0), m_pHostConnection(nullptr), m_Type(UNDEFINED), m_pWorkCompletionHandleThread(nullptr)
{
}

CIBCompletionQueue::~CIBCompletionQueue(void)
{
}

//********************************************************************
//FUNCTION: 
bool CIBCompletionQueue::createEventDrivenCQ(const CIBChannelAdapter *vAdapter, uint32_t vQueueSize, const std::string& vCQHandlingThreadSig)
{
	m_pWorkCompletionHandleThread = dynamic_cast<CIBWCEHandlingThread*>(hiveCommon::hiveCreateProduct(vCQHandlingThreadSig));
	_HIVE_EARLY_RETURN(!m_pWorkCompletionHandleThread,"Fail to create the IB completion queue handling thread.",false);

	_ASSERT(vAdapter && vAdapter->getAdapterHandle());	
	_HIVE_EARLY_RETURN((m_Type != UNDEFINED), "Cannot create the IB completion queue twice.", false);

	m_CompletionEvent = CreateEvent(0, false, false, 0);

	ib_cq_create_t cqCreate;
	cqCreate.h_wait_obj  = m_CompletionEvent;
	cqCreate.pfn_comp_cb = 0;
	cqCreate.size		 = vQueueSize;
	_HIVE_CALL_IB_FUNC(ib_create_cq(vAdapter->getAdapterHandle(), &cqCreate, nullptr, nullptr, &m_CompletionQueue), "ib_create_cq()", false); //FIXME: setting the third and fourth parameter as nullptr is OK for all situations?

	m_Type = EVENT_DRIVEN;
	m_pWorkCompletionHandleThread->setHostCompletionQueue(this);
	return true;
}

//********************************************************************
//FUNCTION:
void CIBCompletionQueue::close()
{
	if (m_CompletionQueue)
	{
		ib_destroy_cq(m_CompletionQueue, 0);
		m_CompletionQueue = 0;
	}

	if (m_CompletionEvent)
	{
		CloseHandle(m_CompletionEvent);
		m_CompletionEvent = 0;
	}

	hiveCommon::hiveOutputEvent("The IB completion queue has been destroyed.");
}

//********************************************************************
//FUNCTION:
void CIBCompletionQueue::startMonitorThread()
{
	if (!m_pWorkCompletionHandleThread->isRunning()) m_pWorkCompletionHandleThread->startThread();
}