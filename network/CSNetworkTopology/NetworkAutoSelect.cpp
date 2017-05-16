#include "NetworkAutoSelect.h"
#include "common\CommonInterface.h"
#include "common\HiveCommonMicro.h"
#include "../common/Packet.h"
#include "../common/PhysicalNode.h"
#include "../common/LogicalNode.h"

using namespace hiveNetworkTopology;

CNetworkAutoSelect::CNetworkAutoSelect()
{
	__initAvailableNodeMap();
}

CNetworkAutoSelect::~CNetworkAutoSelect()
{

}

//**************************************************************************
//FUNCTION:
hiveNetworkCommon::CLogicalNode* CNetworkAutoSelect::selectNetworkNode(hiveNetworkCommon::IUserPacket* vPacket)
{
	_ASSERT(vPacket);

	if (m_AvailableNodeMap.size() == 0) __initAvailableNodeMap();

	if (m_AvailableNodeMap.empty())
	{
		hiveCommon::hiveOutputEvent("No network node is available!");
		return nullptr;
	}

	std::stringstream SerializedStream;
	hiveCommon::hiveSerializeProduct(vPacket, hiveCommon::MODE_BINARY, true, SerializedStream); //FIXME: twice Serialized waste time
	_ASSERT(!SerializedStream.str().empty());
	int PacketDataLen = SerializedStream.str().length();

	if (PacketDataLen < MAX_UDP_SEND_SIZE)
	{
		if (m_AvailableNodeMap.count("UDP") != 0)
			return m_AvailableNodeMap["UDP"];
		else if (m_AvailableNodeMap.count("TCP") != 0)
			return m_AvailableNodeMap["TCP"];
		else if (m_AvailableNodeMap.count("IB") != 0)
			return m_AvailableNodeMap["IB"];
		else
			return nullptr;
	}
	else
	{
		if (m_AvailableNodeMap.count("IB") != 0)
			return m_AvailableNodeMap["IB"];
		else if (m_AvailableNodeMap.count("TCP") != 0)
			return m_AvailableNodeMap["TCP"];
		else
			return nullptr;
	}
}

//**************************************************************************
//FUNCTION:
hiveNetworkCommon::CLogicalNode* CNetworkAutoSelect::selectNetworkNode(const std::string& vNetworkNodeName)
{
	if (m_AvailableNodeMap.size() == 0) __initAvailableNodeMap();

	if (m_AvailableNodeMap.empty())
	{
		hiveCommon::hiveOutputEvent("No network node is available!");
		return nullptr;
	}

	return (m_AvailableNodeMap.count(vNetworkNodeName) == 0) ? nullptr : m_AvailableNodeMap[vNetworkNodeName];
}

//**************************************************************************
//FUNCTION:
void CNetworkAutoSelect::__initAvailableNodeMap()
{
	hiveNetworkCommon::CPhysicalNode::CLogicalNodeIterator itr;
	hiveNetworkCommon::CLogicalNode *pLogicalNode;

	for (pLogicalNode = itr.fetchFirstLogicalNode(); pLogicalNode != nullptr; pLogicalNode = itr.fetchNextLogicalNode())
	{
		pLogicalNode->start();
		if (pLogicalNode->isRunning())
		{
			m_AvailableNodeMap[pLogicalNode->getName()] = dynamic_cast<hiveNetworkCommon::CLogicalNode*>(pLogicalNode);
		}
	}
}