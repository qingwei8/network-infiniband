#pragma once
#define _datatype_  //NOTE: ����������Ŀ�����ڣ���ĳЩ�����ı����У������������꣬�����__int8�����¶���ı�����󣬸ô���������cl_types_osd.h��87��

#include "IBPacket.h"

class CMyIBPacket : public hiveInfiniband::CIBPacket
{
public:
	CMyIBPacket();
	virtual ~CMyIBPacket();

	virtual void processPacketV(hiveNetworkCommon::CLogicalNode* vHostNode) override;
};

