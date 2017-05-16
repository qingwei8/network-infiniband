#include "Packet.h"
#include "common/HiveCommonMicro.h"
#include "common/CommonInterface.h"
#include "common/ProductFactory.h"
#include "NetworkCommon.h"
#include "NetworkInterface.h"

using namespace hiveNetworkCommon;

hiveCommon::CProductFactory<IUserPacket> PacketCreator("Packet");

IUserPacket::IUserPacket() 
{
}

IUserPacket::IUserPacket(const std::string& vPacketSig) : hiveCommon::CBaseProduct(vPacketSig)
{
}

IUserPacket::IUserPacket(const IUserPacket *vPacket) : hiveCommon::CBaseProduct(vPacket)
{
	_ASSERTE(vPacket);
	m_PacketSourceIP = vPacket->m_PacketSourceIP;
}
 
IUserPacket::~IUserPacket()
{
}

//*********************************************************************************
//FUNCTION:
IUserPacket* IUserPacket::clonePacketV()
{
	hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "The virtual function clonePacketV() should be overrided by the derrived class.");
	return nullptr;
}

//*********************************************************************************
//FUNCTION:
bool IUserPacket::isReady2SendV() const
{
	return (!m_PacketSourceIP.empty() && (m_PacketSourceIP != DUMMY_IP) && !getProductCreationSig().empty());
}