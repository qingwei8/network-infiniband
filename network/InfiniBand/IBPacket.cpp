#include "IBPacket.h"
#include "common/ProductFactory.h"
#include "IBNode.h"
#include "NetworkInterface.h"

using namespace hiveInfiniband;

hiveCommon::CProductFactory<CIBPacket> TheCreator(DEFAULT_IB_PACKET_SIG);

CIBPacket::CIBPacket() : m_pData(nullptr), m_DataSize(0), m_IsResponsible2ReleaseData(false)
{
}

CIBPacket::CIBPacket(const std::string& vPacketSig) : hiveNetworkCommon::IUserPacket(vPacketSig), m_pData(nullptr), m_DataSize(0), m_IsResponsible2ReleaseData(false)
{
	_ASSERTE(!getProductCreationSig().empty());
}

CIBPacket::~CIBPacket()
{
	if (m_IsResponsible2ReleaseData) delete[] m_pData;
}

//*********************************************************************************
//FUNCTION:
bool CIBPacket::isReady2SendV() const
{
	return (m_pData && m_DataSize);
}

//*********************************************************************************
//FUNCTION:
void CIBPacket::extractSourceIP(const ib_wc_t* vWorkCompletion)
{
	_ASSERTE(vWorkCompletion && (vWorkCompletion->status == IB_WCS_SUCCESS));

	CIBNode *pNode = dynamic_cast<CIBNode*>(hiveNetworkCommon::hiveFindLogicalNodeByProtocal("IB"));
	_ASSERTE(pNode);
	std::string RemoteIP = pNode->queryRemoteIP(vWorkCompletion->recv.ud.remote_lid);
	_ASSERTE(RemoteIP != hiveNetworkCommon::DUMMY_IP);
	_setPacketSourceIP(RemoteIP);
}