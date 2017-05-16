#pragma once
#include "../../common/Packet.h"

class  CMessageTestPacket : public hiveNetworkCommon::IUserPacket
{
public:
	CMessageTestPacket(void);
	CMessageTestPacket(const CMessageTestPacket* vPacket);
	virtual ~CMessageTestPacket(void);

	virtual void processPacketV(hiveNetworkCommon::CLogicalNode *vHostNode) override;

	virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;
};
