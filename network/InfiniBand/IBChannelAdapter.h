#pragma once
#include "IBCommon.h"

namespace hiveConfig
{
	class CHiveConfig;
}

namespace hiveInfiniband
{
	class CIBNode;

	class CIBChannelAdapter
	{
	public:
		CIBChannelAdapter(void);
		~CIBChannelAdapter(void);

		bool openAdapter();
		void closeAdapter();
		void setHostNode(const CIBNode *vNode) { _ASSERTE(!m_pHostNode && vNode); m_pHostNode = vNode; }
		int	 getNumPorts() const { _ASSERT(m_pChannelAdapterAttributes); return m_pChannelAdapterAttributes->num_ports; }

		ib_ca_handle_t getAdapterHandle() const {return m_ChannelAdapterHandle;}
		ib_pd_handle_t getProtectionDomain() const {return m_ProtectionDomain;}

		const hiveConfig::CHiveConfig* getHostNodeConfig() const;
	
		uint16_t getLocalID(int vIBPort) const					      {_ASSERT(m_pChannelAdapterAttributes); return m_pChannelAdapterAttributes->p_port_attr[vIBPort - 1].lid; } //p_port_attr:包含CA的所有port信息
		uint32_t getMaxNumQueurPair() const							  {_ASSERT(m_pChannelAdapterAttributes); return m_pChannelAdapterAttributes->max_qps;}
		uint32_t getMaxNumWorkRequests() const					      {_ASSERT(m_pChannelAdapterAttributes); return m_pChannelAdapterAttributes->max_wrs;}
		uint32_t getMaxNumCompletionQueue() const				      {_ASSERT(m_pChannelAdapterAttributes); return m_pChannelAdapterAttributes->max_cqs;}
		uint32_t getMaxNumElementsInCompletionQueue() const		      {_ASSERT(m_pChannelAdapterAttributes); return m_pChannelAdapterAttributes->max_cqes;}
		uint32_t getMaxNumProtectionDomain() const				      {_ASSERT(m_pChannelAdapterAttributes); return m_pChannelAdapterAttributes->max_pds;}
		uint32_t getMaxNumScatterGatherElementsPerWorkRequest() const {_ASSERT(m_pChannelAdapterAttributes); return m_pChannelAdapterAttributes->max_sges;}
	
	private:
		const CIBNode  *m_pHostNode;
		ib_al_handle_t	m_AccessLayerHandle; 
		ib_ca_handle_t	m_ChannelAdapterHandle;
		ib_ca_attr_t*	m_pChannelAdapterAttributes;
		ib_pd_handle_t	m_ProtectionDomain;

		void __outputChannelAdapterAttributes();
	};
}