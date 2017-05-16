#include "NetworkConnection.h"
#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>
#include "common/CommonInterface.h"
#include "common/HiveCommonMicro.h"
#include "LogicalNode.h"

using namespace hiveNetworkCommon;

INetworkConnection::INetworkConnection() : m_State(STATE_CLOSED), m_pHostNode(nullptr), m_RemotePort(DUMMY_PORT), m_RemoteIP(DUMMY_IP)
{
}

INetworkConnection::~INetworkConnection()
{
//NOTE: 不能在析构函数中调用closeConnection()，因为closeConnection()调用了虚函数。（不能在构造/析构函数中调用虚函数）
	_ASSERTE(m_State == STATE_CLOSED);
}

//****************************************************************************
//FUNCTION:
void INetworkConnection::_initRemoteInfo(const std::string& vRemoteIP, unsigned short vRemotePort/* =DUMMY_PORT */)
{
	_ASSERTE((m_RemoteIP == DUMMY_IP) && (vRemoteIP != DUMMY_IP)); //NOTE: 我们只希望一个INetworkConnection对象在其生命周期内只调用本函数一次，即INetworkConnection对象在其生命周期内只表示一个连接而不复用

	m_RemoteIP = vRemoteIP;
	m_RemotePort = vRemotePort;
}

//*********************************************************************************
//FUNCTION:
bool INetworkConnection::buildConnection(const std::string& vRemoteIP, unsigned short vRemotePort, bool vTagAsConnectedImmediately)
{
	_ASSERTE(getHostNode());

	_HIVE_EARLY_RETURN((m_State == STATE_CONNECTED), _BOOST_STR3("The requirement of building connection from node [%1%] to [%2%][%3%] is ignored because the connection is existed.", 
		getHostNode()->getName(), vRemoteIP, vRemotePort), true);

	_setState(INetworkConnection::STATE_CONNECTING);

	if (!__buildV(vRemoteIP, vRemotePort)) 
	{
		_setState(STATE_CLOSED);
		return false;
	}

	if (!_validateConnectionV())
	{
		hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, _BOOST_STR3("Validation of connection from node [%1%] to [%2%][%3%] is failed.", 
			getHostNode()->getName(), vRemoteIP, vRemotePort));
		_setState(STATE_CLOSED);
		return false;
	}

//NOTE：对于TCP来说，buildConnection()如果能成功执行到这里，就表明连接已经完全建立了；但对IB来说，由于要互相交换QP信息等，因此在此处还不能将连接标记为CONNECTED
	if (vTagAsConnectedImmediately) _setState(STATE_CONNECTED);
	return true;
}

//****************************************************************************
//FUNCTION:
bool INetworkConnection::isConnectedTo(const std::string& vRemoteIP, unsigned short vRemotePort)
{
	if (!isConnected() || (vRemoteIP != m_RemoteIP)) return false;
	return (vRemotePort == DUMMY_PORT) ? true : (vRemotePort == m_RemotePort);
}

//*********************************************************************************
//FUNCTION:
void INetworkConnection::closeConnection()
{
	_HIVE_EARLY_EXIT((m_State == STATE_CLOSED), "The request of closing connection is ignored because the connection has been closed.");

	_setState(STATE_CLOSING);
	
	__closeConnectionV();

	CLogicalNode *pHostNode = fetchHostNode();
	_ASSERTE(pHostNode);
	pHostNode->__unregisterConnection(this);

	_setState(STATE_CLOSED);
}

//*********************************************************************************
//FUNCTION:
bool INetworkConnection::__buildV(const std::string& vRemoteIP, unsigned short vRemotePort)
{
	hiveCommon::hiveOutputWarning(__EXCEPTION_SITE__, "INetworkConnection::__buildV() should be overrided.");
	return false;
}

//*********************************************************************************
//FUNCTION:
bool INetworkConnection::_validateConnectionV() const
{
	return true;
}

//*********************************************************************************
//FUNCTION:
void INetworkConnection::_setState(EConnectionState vState)
{
	switch (vState)
	{
	case STATE_CONNECTED:
		_ASSERTE((m_State == STATE_CONNECTING) || (m_State == STATE_CLOSED));
		__onConnectedV();
		break;
	case STATE_CLOSED:
		_ASSERTE((m_State == STATE_CLOSING) || (m_State == STATE_CONNECTING));
		break;
	case STATE_CONNECTING:
		_ASSERTE(m_State == STATE_CLOSED);
		break;
	case STATE_CLOSING:
		_ASSERTE((m_State == STATE_CONNECTED) || (m_State == STATE_BROKEN));
		break;
	case STATE_BROKEN:
		break;
	}

	hiveCommon::hiveOutputEvent(_BOOST_STR2("The state of connection is changed from [%1%] to [%2%].", __interpretState(m_State), __interpretState(vState)));
	m_State = vState;
}

//*********************************************************************************
//FUNCTION:
std::string INetworkConnection::__interpretState(EConnectionState vState)
{
	switch (vState)
	{
	case STATE_CONNECTED:
		return std::string("STATE_CONNECTED");
	case STATE_CLOSED:
		return std::string("STATE_CLOSED");
	case STATE_CONNECTING:
		return std::string("STATE_CONNECTING");
	case STATE_CLOSING:
		return std::string("STATE_CLOSING");
	case STATE_BROKEN:
		return std::string("STATE_BROKEN");
	default:
		return std::string("STATE_UNDEFINED");
	}
}