#include "common/CommonInterface.h"
#include "../../common/Packet.h"
#include "PacketExtraInfoOnMessage.h"
#include "IBDataRequestPacket.h"

int main()
{

	hiveCommon::hiveLoadTestingDLLs();

	hiveInfiniband::CIBDataRequestPacket *pPacket = new hiveInfiniband::CIBDataRequestPacket;
	pPacket->setProductCreationSig("IBDataRequestPacket");
	pPacket->setPacketSourceIP("127.0.0.1");
	pPacket->setPacketSourcePort(8888);
	pPacket->setPacketTargetIP("127.0.0.1");
	pPacket->setPacketTargetPort(9999);

	std::stringstream Stream;
	hiveCommon::hiveSerializeProduct(pPacket, hiveCommon::MODE_BINARY, false, Stream);
	hiveInfiniband::CIBDataRequestPacket *pDeserializePacket = dynamic_cast<hiveInfiniband::CIBDataRequestPacket*>(hiveCommon::hiveDeserializeProduct(hiveCommon::MODE_BINARY, Stream));
	if (pDeserializePacket)
	{
		std::cout << "Success to serialize and deserialize success a packet without packet extra info!" << std::endl;
	}
	else
	{
		std::cout << "Failed to serialize and deserialize success a packet without packet extra info!" << std::endl;
	}

	hiveNetworkCommon::CPacketExtraInfo *pPacketExtraInfo = new hiveNetworkCommon::CPacketExtraInfoOnMessage;
	pPacketExtraInfo->setProductCreationSig("PacketExtraInfoOnMessage");
	dynamic_cast<hiveNetworkCommon::CPacketExtraInfoOnMessage*>(pPacketExtraInfo)->setPacketSig("Packet");

	Stream.clear();
	hiveCommon::hiveSerializeProduct(pPacketExtraInfo, hiveCommon::MODE_BINARY, false, Stream);
	hiveNetworkCommon::CPacketExtraInfo *pDeserializePacketExtraInfo = dynamic_cast<hiveNetworkCommon::CPacketExtraInfo*>(hiveCommon::hiveDeserializeProduct(hiveCommon::MODE_BINARY, Stream));
	if (pDeserializePacketExtraInfo)
	{
		std::cout << "Success to serialize and deserialize success a packet extra info!" << std::endl;
	}
	else
	{
		std::cout << "Failed to serialize and deserialize success a packet extra info!" << std::endl;
	}

	pPacket->setContainExtraInfoFlag();
	pPacket->setPacketExtraInfo(pPacketExtraInfo);

	Stream.clear();
	hiveCommon::hiveSerializeProduct(pPacket, hiveCommon::MODE_BINARY, false, Stream);
	hiveInfiniband::CIBDataRequestPacket *pDeserializePacketWithExtraInfo = dynamic_cast<hiveInfiniband::CIBDataRequestPacket*>(hiveCommon::hiveDeserializeProduct(hiveCommon::MODE_BINARY, Stream));
	if (pDeserializePacketWithExtraInfo)
	{
		std::cout << "Success to serialize and deserialize success a packet with packet extra info!" << std::endl;
	}
	else
	{
		std::cout << "Failed to serialize and deserialize success a packet with packet extra info!" << std::endl;
	}

	return 0;
}