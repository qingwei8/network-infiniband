#include "NetworkThread.h"
#include "LogicalNode.h"

using namespace hiveNetworkCommon;

INetworkThread::INetworkThread() : m_pHostLogicalNode(nullptr), m_NodeResponsible4DestroyThreadHint(true)
{
}

INetworkThread::~INetworkThread()
{
}

//***********************************************************
//FUNCTION:
void INetworkThread::_setHostLogicalNode(CLogicalNode *vNode)
{
	_ASSERTE(!m_pHostLogicalNode && vNode); 
	m_pHostLogicalNode = vNode;
	m_pHostLogicalNode->__registerNetworkThread(this);
}