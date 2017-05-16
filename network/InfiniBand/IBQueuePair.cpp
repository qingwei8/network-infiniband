#include "IBQueuePair.h"
#include <boost/format.hpp>
#include "IBCommon.h"
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "common\HiveConfig.h"
#include "IBChannelAdapter.h"

using namespace hiveInfiniband;

CIBQueuePair::CIBQueuePair(void) : m_QueuePairHandle(0), m_LocalQueuePairID(0), m_pHostAdapter(nullptr),m_IsShareCompletionQueue(false), m_RemoteIBInfo(1, 1)
{
}

CIBQueuePair::~CIBQueuePair(void)
{
	if (m_IsShareCompletionQueue)
	{
		_SAFE_DELETE(m_pRecvCompletionQueue);
		m_pSendCompeletionQueue = nullptr;
	}
	else 
	{
		_SAFE_DELETE(m_pRecvCompletionQueue);
		_SAFE_DELETE(m_pSendCompeletionQueue);
	}
}

//*******************************************************************
//FUNCTION:
bool CIBQueuePair::changeState2RTR()
{
	_ASSERT((m_RemoteIBInfo.LocalID > 0) && (m_RemoteIBInfo.QueuePairID > 0));

	ib_qp_mod_t p_qp_mod;

	memset(&p_qp_mod, 0, sizeof(ib_qp_mod_t));
	p_qp_mod.req_state            = IB_QPS_RTR;
	p_qp_mod.state.rtr.rq_psn     = DEF_PSN;  //FIXME: the RQ's start ePSN must be set to the start PSN assigned to the remote QP's SQ. It is supplied in the REP message during the connection establishment process
	p_qp_mod.state.rtr.pkey_index = DEF_PKEY_IX;
	p_qp_mod.state.rtr.dest_qp    = m_RemoteIBInfo.QueuePairID;
	p_qp_mod.state.rtr.primary_av.port_num    = IB_PORT;
	p_qp_mod.state.rtr.primary_av.sl          = DEF_SL;  //sl = service level, determining how quickly the packet will begin transmission from source port. refer to page 43
	p_qp_mod.state.rtr.primary_av.dlid        = m_RemoteIBInfo.LocalID;  //dlid = destination local ID
	p_qp_mod.state.rtr.primary_av.grh_valid   = 0; /* we don't need any GRH */
	p_qp_mod.state.rtr.primary_av.static_rate = DEF_STATIC_RATE;  //defines the interval that must be observed between sending packets to the destination QP. refer to page 44
	p_qp_mod.state.rtr.primary_av.path_bits   = DEF_PATH_BITS;
	p_qp_mod.state.rtr.primary_av.conn.path_mtu          = 4;
	p_qp_mod.state.rtr.primary_av.conn.local_ack_timeout = ACK_TIMEOUT;  
	p_qp_mod.state.rtr.primary_av.conn.seq_err_retry_cnt = ACK_RETRY_COUNT; //number of times that SQ retry the transmission of a request packet. refer to page 44 
	p_qp_mod.state.rtr.primary_av.conn.rnr_retry_cnt     = ACK_RNR_RETRY_COUNT;  //number of times that SQ retry the transmission of a request packet due to receiving RNR(Recvr Not Ready) from remote RQ. refer to page 45
	p_qp_mod.state.rtr.resp_res        = DEF_OUTS_RDMA;
	p_qp_mod.state.rtr.rnr_nak_timeout = DEF_RNR_NAK_TIMER;

	_HIVE_CALL_IB_FUNC(ib_modify_qp(m_QueuePairHandle, &p_qp_mod),"ib_modify_qp()",false);

#ifdef _DEBUG
	__queryAttributes(true);
#endif

	return true;
}

//*******************************************************************
//FUNCTION:
bool CIBQueuePair::changeState2RTS()
{
	ib_qp_mod_t p_qp_mode;

	memset(&p_qp_mode, 0, sizeof(ib_qp_mod_t));

	p_qp_mode.req_state                   = IB_QPS_RTS;
	p_qp_mode.state.rts.sq_psn            = DEF_PSN;
	//p_qp_mode.state.rts.resp_res          = 1;//FIXME::don't know the mean of the paramter 
	p_qp_mode.state.rts.local_ack_timeout = ACK_TIMEOUT;
	p_qp_mode.state.rts.retry_cnt         = ACK_RETRY_COUNT;
	p_qp_mode.state.rts.rnr_retry_cnt     = ACK_RNR_RETRY_COUNT;
	p_qp_mode.state.rts.init_depth        = DEF_OUTS_RDMA;
	//p_qp_mode.state.rts.opts              = IB_MOD_QP_RNR_RETRY_CNT | IB_MOD_QP_RETRY_CNT | IB_MOD_QP_LOCAL_ACK_TIMEOUT;//FIXME: i don't know the mean of the parameter

	_HIVE_CALL_IB_FUNC(ib_modify_qp(m_QueuePairHandle, &p_qp_mode), "ib_modify_qp()", false);

#ifdef _DEBUG
	__queryAttributes(true);
#endif

	return true;
}

//********************************************************************
//FUNCTION: this function only works for RC service
bool CIBQueuePair::createQueuePair(const CIBChannelAdapter *vAdapter, const SQueueAttributes& vSendQueue, const SQueueAttributes& vRecvQueue, bool vShareCompletionQueue)
{
	if (!vShareCompletionQueue)
	{
		_HIVE_UNIMPLEMENTED_FUNC;
		return false;
	}

	_ASSERTE(vAdapter);
	m_pHostAdapter = vAdapter;
	__createCompletionQueues(vSendQueue, vRecvQueue, vShareCompletionQueue);

	const hiveConfig::CHiveConfig *pIBNodeConfig = m_pHostAdapter->getHostNodeConfig();
	_ASSERTE(pIBNodeConfig->isAttributeExisted(CONFIG_KEYWORD::MAX_NUM_RECEIVE_REQUEST) && pIBNodeConfig->isAttributeExisted(CONFIG_KEYWORD::MAX_NUM_SEND_REQUEST));

	ib_qp_create_t  QPCreationInfo;
	cl_memclr(&QPCreationInfo, sizeof(ib_qp_create_t));

	_ASSERT(m_pSendCompeletionQueue && m_pRecvCompletionQueue);
	QPCreationInfo.h_sq_cq = m_pSendCompeletionQueue->getCompletionQueue();
	QPCreationInfo.h_rq_cq = m_pRecvCompletionQueue->getCompletionQueue();
	QPCreationInfo.sq_depth = pIBNodeConfig->getAttribute<int>(CONFIG_KEYWORD::MAX_NUM_SEND_REQUEST);
	QPCreationInfo.rq_depth = pIBNodeConfig->getAttribute<int>(CONFIG_KEYWORD::MAX_NUM_RECEIVE_REQUEST);

	QPCreationInfo.qp_type = IB_QPT_RELIABLE_CONN;   //FIXME: maybe it is better let user determine this parameter
	QPCreationInfo.sq_signaled = true;  //FIXME: make sure this parameter is correct (
	//If set to TRUE, send work requests will always generate a completion event.
	//If set to FALSE, a completion event will only be generated if the send_opt field of the send work request has the
	//IB_SEND_OPT_SIGNALED flag set.)

	_HIVE_EARLY_RETURN((QPCreationInfo.h_srq && (QPCreationInfo.rq_depth != 0)),
		"Fail to create the queue pair due to invalid creation parameters.", false);
	_HIVE_EARLY_RETURN(((QPCreationInfo.h_sq_cq || QPCreationInfo.h_rq_cq) && ((QPCreationInfo.qp_type ==  IB_QPT_MAD) || (QPCreationInfo.qp_type ==  IB_QPT_QP0_ALIAS) || (QPCreationInfo.qp_type ==  IB_QPT_QP1_ALIAS))), 
		"Fail to create the queue pair due to invalid creation parameters.", false);

	_HIVE_CALL_IB_FUNC(ib_create_qp(vAdapter->getProtectionDomain(), &QPCreationInfo, nullptr, nullptr, &m_QueuePairHandle), "ib_create_qp()", false);

	ib_qp_attr_t  QPAttributes;
	_HIVE_CALL_IB_FUNC((ib_query_qp(m_QueuePairHandle, &QPAttributes)), "ib_query_qp", false);
	m_LocalQueuePairID = QPAttributes.num;

	__changeState2INIT();

	return true;
}

//********************************************************************
//FUNCTION:
bool CIBQueuePair::__queryAttributes(bool vISOutputAttributes)
{
	memset(&m_CurrentState, 0, sizeof(ib_qp_attr_t));
	_HIVE_CALL_IB_FUNC((ib_query_qp(m_QueuePairHandle, &m_CurrentState)), "ib_query_qp", false);
	if (vISOutputAttributes) __outputQueuePairAttributes(m_CurrentState);
	return true;
}

//********************************************************************
//FUNCTION:
void CIBQueuePair::__outputQueuePairAttributes(const ib_qp_attr_t& vInfo)
{
	//FIXME: more attributes need to be output
	std::string AttrStr = _BOOST_STR1("Queue pair [%1%] attributes", m_LocalQueuePairID);
	AttrStr += _BOOST_STR1("\n  Current state: %1%.", __getStateString(vInfo.state));
	AttrStr += _BOOST_STR1("\n  Send queue size:    %1%.", vInfo.sq_depth);
	AttrStr += _BOOST_STR1("\n  Recv queue size: %1%.", vInfo.rq_depth);
	AttrStr += _BOOST_STR1("\n  Maximum number of scatter-gather elements in send queue:    %1%.", vInfo.sq_sge);
	AttrStr += _BOOST_STR1("\n  Maximum number of scatter-gather elements in receive queue: %1%.", vInfo.rq_sge);
	AttrStr += _BOOST_STR1("\n  Packet Sequence Number of send queue:    %1%.", vInfo.sq_psn);
	AttrStr += _BOOST_STR1("\n  Packet Sequence Number of receive queue: %1%.", vInfo.rq_psn);
	hiveCommon::hiveOutputEvent(AttrStr);
}

//********************************************************************
//FUNCTION:
std::string CIBQueuePair::__getStateString(ib_qp_state_t vState)
{
	//FIXME: not all states are covered
	switch (vState)
	{
	case IB_QPS_RESET:
		return std::string("RESET");
	case IB_QPS_INIT:
		return std::string("INIT");
	case IB_QPS_RTR:
		return std::string("Ready to Recv");
	case IB_QPS_RTS:
		return std::string("Ready to Send");
	case IB_QPS_ERROR:
		return std::string("Error");
	case IB_QPS_SQERR:
		return std::string("SQ Error");
	case IB_QPS_TIME_WAIT:
		return std::string("Time wait");
	default:
		return std::string("UNDEFINED");
	}
}

//********************************************************************
//FUNCTION:
void CIBQueuePair::startCompletionQueueMonitorThread()
{
	_ASSERT(m_pRecvCompletionQueue && m_IsShareCompletionQueue);
	m_pRecvCompletionQueue->startMonitorThread();
	hiveCommon::hiveOutputEvent("The completion queue monitor thread has been started.");
}

//********************************************************************
//FUNCTION:
void CIBQueuePair::__createCompletionQueues(const SQueueAttributes& vSendQueue, const SQueueAttributes& vRecvQueue, bool vShareCompletionQueue)
{
	_ASSERTE(vShareCompletionQueue);
	m_IsShareCompletionQueue = vShareCompletionQueue;
	m_pRecvCompletionQueue = __createCompletionQueue(vRecvQueue);
	m_pSendCompeletionQueue = vShareCompletionQueue ? m_pRecvCompletionQueue : __createCompletionQueue(vSendQueue);
}

//********************************************************************
//FUNCTION:
CIBCompletionQueue* CIBQueuePair::__createCompletionQueue(const SQueueAttributes& vQueueAttr)
{
	_ASSERT(m_pHostAdapter);
	CIBCompletionQueue *pCQ = new CIBCompletionQueue;
	if (vQueueAttr.isEventDrivenCQ)
	{
		pCQ->createEventDrivenCQ(m_pHostAdapter, vQueueAttr.CompletionQueueSize, vQueueAttr.CompletionQueueHandlerThreadSig);
	}
	else
	{
		_HIVE_UNIMPLEMENTED_FUNC;
	}
	return pCQ;
}

//********************************************************************
//FUNCTION: from the INIT state, the only valid state that software may transition the QP to is the RTR state
bool CIBQueuePair::__changeState2INIT()
{
	ib_qp_mod_t p_qp_mod;

	memset(&p_qp_mod, 0, sizeof(ib_qp_mod_t));
	p_qp_mod.req_state = IB_QPS_INIT;
	p_qp_mod.state.init.primary_port = IB_PORT;
	p_qp_mod.state.init.pkey_index   = DEF_PKEY_IX; //pkey = partition key, refer to page 239 and 319
	p_qp_mod.state.init.access_ctrl  = 0; 

	_HIVE_CALL_IB_FUNC(ib_modify_qp(m_QueuePairHandle, &p_qp_mod), "ib_modify_qp()", false);

#ifdef _DEBUG
	__queryAttributes(true);
#endif

	return true;
}

//********************************************************************
//FUNCTION:
void CIBQueuePair::setHostConnection(const CIBConnection* vConn)
{
	_ASSERT(vConn && m_pRecvCompletionQueue);
	m_pRecvCompletionQueue->setHostConnection(vConn);
	if(!m_IsShareCompletionQueue) m_pSendCompeletionQueue->setHostConnection(vConn);
}

//*********************************************************************************
//FUNCTION:
void CIBQueuePair::close()
{
	if (m_pRecvCompletionQueue)
	{
		m_pRecvCompletionQueue->close();
		_SAFE_DELETE(m_pRecvCompletionQueue);
		if (!m_IsShareCompletionQueue)
		{
			_ASSERT(m_pSendCompeletionQueue);
			m_pSendCompeletionQueue->close();
			_SAFE_DELETE(m_pSendCompeletionQueue);
		}
	}

	if (m_QueuePairHandle)
	{
		ib_destroy_qp(m_QueuePairHandle,0);
		m_QueuePairHandle = 0;
	}

	hiveCommon::hiveOutputEvent("The queue pair has been destroyed.");
}

//*********************************************************************************
//FUNCTION:
bool CIBQueuePair::SQueueAttributes::isValid(const CIBChannelAdapter *vAdapter) const
{
	return true;
}