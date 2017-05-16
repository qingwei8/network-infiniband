#include "CSNetworkTopologyMessagePool.h"

//**************************************************************************
//FUNCTION:
void hiveNetworkTopology::CCSNTMessagePool::pushMessage(hiveNetworkCommon::IUserPacket* vMessage)
{
	m_MessageQuque.push(vMessage);
}
  
//**************************************************************************
//FUNCTION:
int hiveNetworkTopology::CCSNTMessagePool::getSize()
{
	return m_MessageQuque.size();
}

//*********************************************************************************
//FUNCTION:
hiveNetworkCommon::IUserPacket* hiveNetworkTopology::CCSNTMessagePool::waitAndPop()
{
	hiveNetworkCommon::IUserPacket* pPacket = nullptr;
	m_MessageQuque.waitAndPop(pPacket);
	return pPacket;
}

//*********************************************************************************
//FUNCTION:
bool hiveNetworkTopology::CCSNTMessagePool::tryPop(hiveNetworkCommon::IUserPacket* voPacket)
{
	return m_MessageQuque.tryPop(voPacket);
}
