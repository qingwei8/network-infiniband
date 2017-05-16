#pragma once
#include <string>
#include <boost/any.hpp>
#include "common/BaseProduct.h"
#include "NetworkExport.h"
#include "NetworkCommon.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;

	class NETWORK_DLL_EXPORT INetworkConnection : public hiveCommon::CBaseProduct
	{
	public:
		enum EConnectionState
		{
			STATE_CLOSED = 0,     
			STATE_CONNECTING, 
			STATE_CONNECTED, 
			STATE_CLOSING,  //NOTE: broken״̬��ʾ�������ڸ���ԭ����Ͽ�������Ӧ��Դ��û���ͷţ�closed״̬�������ӶϿ������������Դ�����ͷ�
			STATE_BROKEN,
		};

	public:
		INetworkConnection();
		virtual ~INetworkConnection();
	
		void setRemoteIP(const std::string& vRemoteIP) {m_RemoteIP = vRemoteIP;}

		const CLogicalNode* getHostNode() const {return m_pHostNode;}
		const std::string&  getRemoteIP() const {return m_RemoteIP;}
		unsigned short	    getRemotePort() const {return m_RemotePort;}

		CLogicalNode* fetchHostNode() const {return m_pHostNode;}

		bool isClosed() const     {return m_State == STATE_CLOSED;}
		bool isConnected() const  {return m_State == STATE_CONNECTED;}
		bool isConnecting() const {return m_State == STATE_CONNECTING;}
		bool isBroken() const     {return m_State == STATE_BROKEN;}
		bool isConnectedTo(const std::string& vRemoteIP, unsigned short vRemotePort=DUMMY_PORT);
		bool buildConnection(const std::string& vRemoteIP, unsigned short vRemotePort, bool vTagAsConnectedImmediately);

		void notifyConnectionIsBroken() {_setState(STATE_BROKEN);}
		void closeConnection();

	protected:
		void _setState(EConnectionState vState);
		void _setHostNode(CLogicalNode *vNode) {_ASSERTE(vNode && !m_pHostNode); m_pHostNode = vNode;}
		void _initRemoteInfo(const std::string& vRemoteIP, unsigned short vRemotePort=DUMMY_PORT); 

		virtual bool _validateConnectionV() const;

	private:
		std::string       m_RemoteIP;
		unsigned short    m_RemotePort;
		EConnectionState  m_State;
		CLogicalNode     *m_pHostNode;

		virtual bool __buildV(const std::string& vRemoteIP, unsigned short vRemotePort);
		virtual void __onConnectedV() {}
		virtual void __closeConnectionV() = 0;   //NOTE: �����ͷź�������صĸ�����Դ������TCP�йر�socket��IB���ͷ�queue pair�ȵȣ�

		std::string __interpretState(EConnectionState vState);

	    friend class CLogicalNode;
	};
}