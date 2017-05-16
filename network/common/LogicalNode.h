#pragma once
#include <boost/any.hpp>
#include "common/BaseProduct.h"
#include "common/HiveConfig.h"
#include "common/ConcurrentQueue.h"
#include "NetworkCommon.h"
#include "NetworkExport.h"

namespace hiveConfig
{
	class CHiveConfig;
}

namespace hiveNetworkCommon
{
	class CPhysicalNode;
	class INetworkThread;
	class IUserPacket;
	class INetworkConnection;

	class NETWORK_DLL_EXPORT CLogicalNode : public hiveCommon::CBaseProduct
	{
	public:
		CLogicalNode();
		virtual ~CLogicalNode();

		inline bool isRunning() const     {return m_IsRunning;}
		inline bool isInitialized() const {return m_IsInitialized;}

		const std::string&   getName() const             {return m_Config.getName();}
		const CPhysicalNode* getHostPhysicalNode() const {return m_pHostPhysicalNode;} 
		const hiveConfig::CHiveConfig* getConfig() const {return &m_Config;}

		void unregisterNetworkThread(const INetworkThread* vNetworkThread);	
		void start();
		void stop();
		void init();
		void sendPacket(const IUserPacket *vUserPacket, const SSendOptions& vSendOption);
		void registerConnection(INetworkConnection* vConnection);
		void notifyConnectionIsBroken(const std::string& vRemoteIP, unsigned short vRemotePort);
		void updateNodeConfig(const std::string& vAttributePath, boost::any vAttributeValue);

		bool isConnectionBuilt(const std::string& vRemoteIP, unsigned short vRemotePort=DUMMY_PORT);
		bool buildConnection(const std::string& vRemoteIP, unsigned short vRemotePort, bool vTagAsConnectedImmediately);

		std::string getIP() const;

	protected:
		std::vector<INetworkConnection*> m_ConnectionSet;  //NOTE: ���ｫ��������Ա��������Ϊ������Ա������Ϊ������������Ҫ��������m_ConnectionSet
		boost::mutex m_Mutex4Connection;

		hiveCommon::CConcurrentQueue<IUserPacket*> m_IncomingPacketQueue;  //NOTE: ���ｫ��������Ա��������Ϊ������Ա������Ϊ��������ܻ�ǳ�Ƶ���Ĳ�����������Ա������Ϊ�����Ч�ʣ���������Ϊ������Ա
		hiveCommon::CConcurrentQueue<std::pair<const IUserPacket*, SSendOptions>> m_OutgoingPacketQueue;

		virtual void _startNodeV() {}
		virtual void _stopNodeV()  {}
	
		void _startAllThreads();
		void _closeAllConnections();

		std::pair<const IUserPacket*, SSendOptions> _waitAndPopOutgoingUserPacket();

		IUserPacket* _waitAndPopIncommingUserPacket();
		unsigned int _getNumUnsendPacket()  {return m_OutgoingPacketQueue.size();}

	private:
		bool m_IsRunning;
		bool m_IsInitialized;
		const CPhysicalNode     *m_pHostPhysicalNode;
		hiveConfig::CHiveConfig  m_Config;
		std::vector<std::pair<INetworkThread*, bool>> m_NetworkThreadSet;

		void __setConfig(const hiveConfig::CHiveConfig *vConfig);
		void __setHostPhysicalNode(const CPhysicalNode* vNode)   {_ASSERTE(vNode && !m_pHostPhysicalNode); m_pHostPhysicalNode = vNode;}
		void __registerNetworkThread(INetworkThread* vNetworkThread);
		void __notifyAllWaitingConcurrentQueue();
		void __joinAllThreads();
		void __clearAllConcurrentQueue();
		void __interruptAllThreads();
		void __unregisterConnection(INetworkConnection* vConnection);

		virtual void __createNetworkThreadsV(std::vector<INetworkThread*>& voOutput) = 0;
		virtual bool __extraInitV()  {return true;}
		virtual void __sendPacketV(const IUserPacket *vPacket, const SSendOptions& vSendOption) = 0;

	friend class CPhysicalNode;
	friend class CUserPacketHandlingThread;
	friend class INetworkThread;
	friend class INetworkConnection;
	};
}