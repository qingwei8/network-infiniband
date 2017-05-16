#pragma once
#include <boost/atomic.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/algorithm/string.hpp>
#include "IBCommon.h"
#include "NetworkConnection.h"
#include "IBChannelAdapter.h"
#include "IBQueuePair.h"
#include "IBMemoryBlockPool.h"

namespace hiveInfiniband
{
	class CIBChannelAdapter;
	class CIBQueuePair;
	class CLogicalNode;
	class CIBNode;

	class CIBConnection : public hiveNetworkCommon::INetworkConnection
	{
	public:
		CIBConnection();
		CIBConnection(const CIBChannelAdapter* vAdapter);
		~CIBConnection(void);

		bool postIBSendRequest(unsigned int vSendRequestID);
 		bool onConnectionReqPacketReceived(CIBNode* vHostNode, const std::string& vRemoteNodeName, const std::string& vRemoteIP, ib_net16_t vRemoteLID, const std::vector<ib_net32_t>& vRemoteQPIDSet, const CIBChannelAdapter* vChannleAdapter);
		void setRemoteNodeName(const std::string& vRemoteNodeName) {m_RemoteNodeName = vRemoteNodeName;}
		void setConnectionType(const EIBConnectionType& vType);
		void setRemoteIBInfo(const std::string& vRemoteIP, ib_net16_t vRemoteLID, const std::vector<ib_net32_t>& vRemoteQPIDSet);

		std::vector<ib_net32_t> getQueuePairIDSet();

		const std::string& getRemoteNodeName() const {return m_RemoteNodeName;}
		CIBMemoryBlock*    findMemoryBlock(unsigned int vWorkRequestID, const EWorkRequestType& vWorkReqType);
		unsigned int       prepareData2Send(const unsigned char *vBuffer, unsigned int vBufferSize);
		unsigned int       postIBReceiveRequest(unsigned int vRecvBufferSize);

		ib_net16_t getRemoteLID() const { return m_RemoteLID; }

	private:
		std::string      m_RemoteNodeName;
		ib_net16_t       m_RemoteLID;
		int	             m_SendQPIndex;
		int	             m_RecvQPIndex;
		unsigned int     m_SendWorkRequestID;
		unsigned int     m_RecvWorkRequestID;
		boost::mutex     m_Mutex;
		EIBConnectionType m_ConnectionType;
		CIBMemoryBlockPool         m_MemoryBlockPool;
		std::string		           m_CompletionQueueHandlerThreadSig;
		std::vector<CIBQueuePair*> m_QueuePairSet;  //one connection can contain more than one queue pair
		std::vector<ib_net32_t>    m_RemoteQueuePairIDSets;
		const CIBChannelAdapter*   m_pChannelAdatper;

		virtual bool __buildV(const std::string& vRemoteIP, unsigned short vRemotePort) override;
		virtual void __onConnectedV() override;
		virtual void __closeConnectionV() override;

		void __setChannelAdapter(const CIBChannelAdapter* vChannelAdapter) { _ASSERT(vChannelAdapter); m_pChannelAdatper = vChannelAdapter; }
		void __sendBuildConnectionReqPacket(const std::string& vTargetIP, int vTargetPort, ib_net16_t vRequesterLID, const std::vector<ib_net32_t>& vRequesterQueuePairIDSet);
		bool __makeQueuePairReady();
		bool __createQueuePairObjects(unsigned int vNumQueuePair);

		friend class CBuildCIBConnectionAckPacket;
		friend class CIBNode;
	};
}