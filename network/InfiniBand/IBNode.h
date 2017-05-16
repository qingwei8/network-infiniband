#pragma once
#include <vector>
#include <boost\atomic.hpp>
#include <common\ConcurrentQueue.h>
#include "LogicalNode.h"
#include "IBChannelAdapter.h"
#include "InfinibandExport.h"

namespace hiveNetworkCommon
{
	class INetworkThread;
	class IUserPacket;
};

namespace hiveInfiniband
{
	class CIBConnection;
	class CIBChannelAdapter;
	class CIBPacket;

	class INFINIBAND_DLL_EXPORT CIBNode : public hiveNetworkCommon::CLogicalNode
	{
	public:
		CIBNode(void);
		virtual ~CIBNode(void);

		hiveNetworkCommon::CLogicalNode* fetchAuxNode() const      {return m_pAuxNode;}
		const CIBChannelAdapter*		 getChannelAdapter() const {return m_pChannelAdapter;}
		CIBConnection*					 findConnection(const std::string& vTargetIP);

		bool isConnectionReqPacketReceived(const std::string& vRemoteIP, const std::string& vRemoteNodeName) const;
		void sendBuildConnectionReqPacket(const std::string& vTargetIP, int vTargetPort, ib_net16_t vRequesterLID, const std::vector<ib_net32_t>& vRequesterQueuePairIDSet);
		void registerIBPacketSignature(unsigned int vWorkRequestID, const std::string& vIBPacketSig) { m_ReceiveRequest2PacketSigMap.push_back(std::make_pair(vWorkRequestID, vIBPacketSig)); }

		std::string queryRemoteIP(ib_net16_t vRemoteLID) const;
		std::string queryIBPacketSignature(unsigned int vWorkRequestID);

	private:
		virtual bool __extraInitV() override;
		virtual void __sendPacketV(const hiveNetworkCommon::IUserPacket *vPacket, const hiveNetworkCommon::SSendOptions& vSendOption) override;
		virtual void _startNodeV() override {}
		virtual void __createNetworkThreadsV(std::vector<hiveNetworkCommon::INetworkThread*>& voOutput) override {}

		bool __startAuxNode(const std::string& vAuxNodeName);
		bool __onConnectionReqPacketReceived(ib_net16_t vRequesterLID, const std::vector<ib_net32_t>& vRequesterQPIDSet, const std::string& vRequesterIP, const std::string& vRequesterNodeName);
		void __sendBuildConnectionAckPacket(const std::string& vTargetIP);

		std::pair<const unsigned char*, unsigned int> __convertPacket2IBData(const CIBPacket *vPacket) const;

	private:
		CIBChannelAdapter* m_pChannelAdapter;
		hiveNetworkCommon::CLogicalNode*     m_pAuxNode;
		std::list<std::pair<unsigned int, std::string>> m_ReceiveRequest2PacketSigMap;

		friend class CBuildCIBConnectionReqPacket;
	};
}