#pragma once

#include "iba/ib_types.h"
#include "iba/ib_al.h"
#include <string>

#define DEF_PKEY_IX         0
#define DEF_PSN             0
#define DEF_SL              0
#define DEF_STATIC_RATE     IB_PATH_RECORD_RATE_30_GBS; //define TODO according to link width
#define DEF_PATH_BITS       0
#define ACK_TIMEOUT         5
#define ACK_RETRY_COUNT     5
#define ACK_RNR_RETRY_COUNT 5
#define DEF_RNR_NAK_TIMER   0x10
#define DEF_OUTS_RDMA       1

namespace hiveInfiniband
{
	const int IB_PORT = 1;

	const std::string PROTOCAL_IB = "IB";

	const std::string DEFAULT_IB_PACKET_SIG = "IBPacket";

	const unsigned int DEFAULT_COMPLETION_QUEUE_SIZE = 128;

	const std::string DEFAULT_IB_NODE_SIG = "IB_NODE";
	const std::string DEFAULT_IB_CONNECTION_REQ_PACKET = "IB_CONNECTION_REQ_PACKET";
	const std::string DEFAULT_IB_CONNECTION_ACK_PACKET = "IB_CONNECTION_ACK_PACKET";

	const std::string DEFAULT_IB_CQ_HANDLE_THREAD_SIG    = "IBWCEHandlingThread";
	const std::string DEFAULT_IB_CQ_SENDING_THREAD_SIG   = "IBWCESendingThread";
	const std::string DEFAULT_IB_CQ_RECEIVING_THREAD_SIG = "IBWCEReceivingThread";

	const std::string DEFAULT_IB_CQ_HANDLER_THREAD_SIGNATURE = "IBWCEHandlingThread";

	const int DEFAULT_IB_QP_SQ_DEPTH = 100;
	const int DEFAULT_IB_QP_RQ_DEPTH = 100;
	const int DEFAULT_IB_NUM_QUEUE_PAIR = 2;
	const int DEFAULT_PERFORMANCE_TEST_NUM_REQUEST = 200;

	const int DEFAULT_IB_MEMORY_BLOCK_SIZE = 1920 * 1080 * 3;

	const int DEFAULT_IB_NUM_MEMORY_BLOCK = 32;
	const int DEFAULT_NUM_TRY_BUILDING_CONN = 5;

	#define _HIVE_CALL_IB_FUNC(func, func_name, return_value) {ib_api_status_t t = func; if (t!=IB_SUCCESS) {hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, boost::str(boost::format("Error occured in the excution of IB function [%1%] : %2%") % func_name % std::string(ib_get_err_str(t)))); _ASSERTE(false); return return_value;}}
	#define _HIVE_CALL_IB_FUNC1(func, failed_value, func_name, return_value) {ib_api_status_t t = func; if (t!=failed_value) {hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, boost::str(boost::format("Error occured in the excution of IB function [%1%] : %2%") % func_name % std::string(ib_get_err_str(t)))); _ASSERTE(false); return return_value;}}
	#define _HIVE_CALL_IB_FUNC2(func, func_name) {ib_api_status_t t = func; if (t!=IB_SUCCESS) {hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, boost::str(boost::format("Error occured in the excution of IB function [%1%] : %2%") % func_name % std::string(ib_get_err_str(t)))); _ASSERTE(false);}}

	enum EIBConnectionType
	{
		IB_UNKNOWN,
		IB_INBOUND,
		IB_OUTBOUND,
	};

	enum EWorkRequestType
	{
		SEND_TYPE,
		RECV_TYPE,
	};

	namespace CONFIG_KEYWORD
	{
		const std::string IB_AUX_NODE    = "IB_AUX_NODE";
		const std::string IB_CONNECTION  = "IB_CONNECTION";
		const std::string IB_NUM_QUEUE_PAIR    = "IB_NUM_QUEUE_PAIR";
		const std::string MAX_NUM_SEND_REQUEST = "MAX_NUM_SEND_REQUEST";
		const std::string MAX_NUM_RECEIVE_REQUEST = "MAX_NUM_RECEIVE_REQUEST";
		const std::string REMOTE_IP      = "REMOTE_IP";
		const std::string REMOTE_PORT    = "REMOTE_PORT";
		const std::string IB_MEMORY_BLOCK_SIZE		= "IB_MEMORY_BLOCK_SIZE";
		const std::string IB_NUM_MEMORY_BLOCK		= "IB_NUM_MEMORY_BLOCK";
		const std::string NUM_TRY_BUILDING_CONN     = "NUM_TRY_BUILDING_CONN";
		const std::string IB_CQ_HANDLE_THREAD_SIG   = "IB_CQ_HANDLE_THREAD_SIG";
		const std::string IB_PERFORMANCE_TEST_MODE  = "IB_PERFORMANCE_TEST_MODE";
		const std::string IB_PERFORMANCE_TEST_NUM_REQUEST = "IB_PERFORMANCE_TEST_NUM_REQUEST";
	}
}