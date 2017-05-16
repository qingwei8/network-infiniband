#pragma once
#include "common/HiveThread.h"
#include "NetworkExport.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;

	class NETWORK_DLL_EXPORT INetworkThread : public hiveThread::CHiveThread
	{
	public:
		INetworkThread();
		virtual ~INetworkThread();
	
		const CLogicalNode* getHostLogicalNode() const {return m_pHostLogicalNode;}
		
		CLogicalNode* fetchHostLogicalNode() const {return m_pHostLogicalNode;}

		bool isNodeResponsible4DestroyThread() const {return m_NodeResponsible4DestroyThreadHint;}

	protected:
		void _letNodeReleaseThread() {m_NodeResponsible4DestroyThreadHint = true;}
		void _setHostLogicalNode(CLogicalNode *vNode);

	private:
		bool          m_NodeResponsible4DestroyThreadHint;
		CLogicalNode *m_pHostLogicalNode;

		virtual bool __isReady2RunV() override { return m_pHostLogicalNode; }  //NOTE: ǿ������������thread�������_setHostLogicalNode()�����߲��ܽ��߳�����ע�ᵽ�ڵ���

	friend class CLogicalNode;
	};
}