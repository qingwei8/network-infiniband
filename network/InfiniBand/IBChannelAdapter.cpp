#include "IBChannelAdapter.h"
#include <boost/scoped_ptr.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common\HiveCommonMicro.h"
#include "common\CommonInterface.h"
#include "common\HiveConfig.h"
#include "IBNode.h"

using namespace hiveInfiniband;

CIBChannelAdapter::CIBChannelAdapter(void) : m_pChannelAdapterAttributes(nullptr), m_AccessLayerHandle(0), m_ChannelAdapterHandle(0), m_ProtectionDomain(0), m_pHostNode(nullptr)
{
}


CIBChannelAdapter::~CIBChannelAdapter(void)
{
	free(m_pChannelAdapterAttributes);
	m_pChannelAdapterAttributes = nullptr;
}

//********************************************************************
//FUNCTION:
const hiveConfig::CHiveConfig* CIBChannelAdapter::getHostNodeConfig() const
{
	_ASSERTE(m_pHostNode);
	return m_pHostNode->getConfig();
}

//********************************************************************
//FUNCTION:
bool CIBChannelAdapter::openAdapter()
{
	//open an instance of the access layer
	_HIVE_CALL_IB_FUNC(ib_open_al(&m_AccessLayerHandle), "ib_open_al()", false);

	//get the number of channel adapters currently available in the system
	size_t NumGlobalUniqueID;
	_HIVE_CALL_IB_FUNC1(ib_get_ca_guids(m_AccessLayerHandle, nullptr, &NumGlobalUniqueID), IB_INSUFFICIENT_MEMORY, "ib_get_ca_guids()", false); 

	//get a list of global unique IDs for all channel adapter currently available in the system	
	boost::scoped_ptr<ib_net64_t> pChannelAdapterGuid(new ib_net64_t[NumGlobalUniqueID]);
	_HIVE_CALL_IB_FUNC(ib_get_ca_guids(m_AccessLayerHandle, pChannelAdapterGuid.get(), &NumGlobalUniqueID), "ib_get_ca_guids()", false); 
	hiveCommon::hiveOutputEvent(_BOOST_STR1("Total number of channel adapter in the system: %1%.", NumGlobalUniqueID));

	//open a channel adapter for additional access
	_HIVE_CALL_IB_FUNC(ib_open_ca(m_AccessLayerHandle, pChannelAdapterGuid.get()[0], nullptr, nullptr, &m_ChannelAdapterHandle), "ib_open_ca()", false);   //FIXME-028: if the system has more than one channel adapter, the following codes only configure the first one

	//query the size to store the adapter attributes
	uint32_t AttrBufSize = 0;
	_HIVE_CALL_IB_FUNC1(ib_query_ca(m_ChannelAdapterHandle, nullptr, &AttrBufSize), IB_INSUFFICIENT_MEMORY, "ib_query_ca()", false); 

	//query the attributes of an opened channel adapter
	m_pChannelAdapterAttributes = (ib_ca_attr_t *)malloc(AttrBufSize);
	_HIVE_CALL_IB_FUNC(ib_query_ca(m_ChannelAdapterHandle, m_pChannelAdapterAttributes, &AttrBufSize), "ib_query_ca()", false);
	_ASSERT(AttrBufSize == m_pChannelAdapterAttributes->size);
	if (m_pChannelAdapterAttributes->p_port_attr[0].lid == 0)
	{
		hiveCommon::hiveOutputEvent("The local identifier is zero, please check whether the Subnet Manager is running."); //NOTE: 可以参考这里https://msdn.microsoft.com/en-us/library/dd391828(v=ws.10).aspx来获得如何在Windows下启动OpenSM服务
		return false;
	}
	__outputChannelAdapterAttributes();

	//allocate a protection domain on the specified channel adapter
	 //FIXME: 1. the second parameter of ib_alloc_pd() should be determined by outside configuration; 2. using nullptr as the third parameter is always OK?
	_HIVE_CALL_IB_FUNC(ib_alloc_pd(m_ChannelAdapterHandle, IB_PDT_NORMAL, nullptr, &m_ProtectionDomain), "ib_alloc_pd()", false);

	hiveCommon::hiveOutputEvent("Succeed to open the IB adapter.");

	return true;
}

//********************************************************************
//FUNCTION:
void CIBChannelAdapter::__outputChannelAdapterAttributes()
{
	std::string AttrStr = "IB channel adapter attributes";
	AttrStr += _BOOST_STR1("\n  Maximum number of queue pairs:                  %1%.", getMaxNumQueurPair());
	AttrStr += _BOOST_STR1("\n  Maximum number of work requests:                %1%.", getMaxNumWorkRequests());
	AttrStr += _BOOST_STR1("\n  Maximum number of completion queues:            %1%.", getMaxNumCompletionQueue());
	AttrStr += _BOOST_STR1("\n  Maximum number of elements in completion queue: %1%.", getMaxNumElementsInCompletionQueue());
	AttrStr += _BOOST_STR1("\n  Maximum number of protection domains:           %1%.", getMaxNumProtectionDomain());
	AttrStr += _BOOST_STR1("\n  Number of ports:                                %1%.", getNumPorts());
	AttrStr += _BOOST_STR1("\n  Maximum Number of scatter gather elements:      %1%.", getMaxNumScatterGatherElementsPerWorkRequest());
	hiveCommon::hiveOutputEvent(AttrStr);
}

//********************************************************************
//FUNCTION:
void CIBChannelAdapter::closeAdapter()
{
	if (m_ProtectionDomain) 
	{
		ib_api_status_t ibStatus = ib_dealloc_pd(m_ProtectionDomain, 0);
		m_ProtectionDomain = 0;
	}

	if (m_ChannelAdapterHandle)
	{
		ib_close_ca(m_ChannelAdapterHandle, 0);
		m_ChannelAdapterHandle = 0;
	}

	if (m_AccessLayerHandle)
	{
		ib_close_al(m_AccessLayerHandle);
		m_AccessLayerHandle = 0;
	}
}