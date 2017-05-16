#pragma once
#include <boost\thread\condition.hpp>
#include "common\Singleton.h"

namespace hiveNetworkCommon
{
	class CNetworkNodeConfig;
}

namespace hiveNetworkTopology
{
	class CRegisterThread;
	class CTimingSendPacketThread;
	class CCSNTMessageHandleThread;
	class CConfirmConnectedThread;

	class CClientPhysicalNode : public hiveCommon::CSingleton<CClientPhysicalNode>
	{
	public:
		virtual ~CClientPhysicalNode();

		void waitRegister();
		void notifyRegister();

		void stopPhysicalNode();
		void insertSendPacketSig(int vTime, std::string vSig);
		void setKeepAliveOpenFlag(bool vFlag) {m_IsKeepAliveOpen = vFlag;}
		
		bool isKeepAliveOpen() {return m_IsKeepAliveOpen;}
		bool init(const hiveNetworkCommon::CNetworkNodeConfig *vConfig, bool vInitLogicalNode);

		const std::multimap<int, std::string>& getSendPacketSigSets() {return m_SendPacketSigSets;}

		const time_t getLastConfirmConnectionTimeStamp() const;

	private:
		bool                      m_IsNodeInit;
		bool                      m_IsKeepAliveOpen;
		time_t                    m_ConfirmConnectionTimeStamp;
		CRegisterThread*          m_pRegisterThread;
		CTimingSendPacketThread*  m_pKeepAliveThread;
		CConfirmConnectedThread*  m_pConfirmConnetedThread;
		CCSNTMessageHandleThread* m_pCSNTMessageHandleThread;
		boost::condition_variable m_RegisterSucceedSignal;

		std::multimap<int, std::string> m_SendPacketSigSets;

		CClientPhysicalNode();

		void __stopRegisterThread();
		void __startRegisterThread();
		void __stopKeepAliveThread();
		void __startKeepAliveThread();
		void __stopConfirmConnectionThread();
		void __startConfirmConnectionThread();

		void __updataConfirmConnectionTimeStamp();

		friend class CConfirmConnectedThread;
		friend class CCSNTMessageHandleThread;
		friend class hiveCommon::CSingleton<CClientPhysicalNode>;
	};
}
