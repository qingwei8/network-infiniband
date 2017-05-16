#pragma once
#define _datatype_  //NOTE: 加入这个宏的目的在于，在某些机器的编译中，如果不加这个宏，会出现__int8被重新定义的编译错误，该错误来自于cl_types_osd.h第87行

#include "IBPacket.h"

class CMyIBPacket : public hiveInfiniband::CIBPacket
{
public:
	CMyIBPacket();
	virtual ~CMyIBPacket();

	virtual void processPacketV(hiveNetworkCommon::CLogicalNode* vHostNode) override;
};

