#pragma once
#include "../common/Packet.h"
#include <string>
#include <boost/serialization/serialization.hpp>
#include "CSNetworkTopologyExport.h"

namespace hiveNetworkTopology
{
	class NETWORK_TOPOLOGY_DLL_EXPORT IMessage : public hiveNetworkCommon::IUserPacket
	{
	public:
		enum EMessageType
		{
			REGISTER_MESSAGE,
			REGISTER_REPLY_MESSAGE,
			KEEP_ALIVE_MESSAGE,
			KEEP_ALIVE_REPLY_MESSAGE
		};

	public:
		IMessage() {}
		IMessage(EMessageType vMessageType);
		IMessage(const IMessage* vMessage);
		virtual ~IMessage() {}

		EMessageType getMessageType() const {return m_MessageType;}

		virtual void processV(hiveNetworkCommon::CLogicalNode *vHostNode) override;
		virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;

		const unsigned int getTCPListeningPort() const {return m_TCPListeningPort;}
		const unsigned int getUDPListeningPort() const {return m_UDPListeningPort;}

		void setTCPListeningPort(unsigned int vTCPListeningPort) {m_TCPListeningPort = vTCPListeningPort;}
		void setUDPListeningPort(unsigned int vUDPListeningPort) {m_UDPListeningPort = vUDPListeningPort;}

	protected:
		EMessageType m_MessageType;

	private:
		unsigned int m_TCPListeningPort;
		unsigned int m_UDPListeningPort;

		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& vAr, const unsigned vVersion)
		{
			vAr & BOOST_SERIALIZATION_BASE_OBJECT_NVP(IUserPacket);
			vAr & m_MessageType;
			vAr & m_TCPListeningPort;
			vAr & m_UDPListeningPort;
		}
	};

	BOOST_SERIALIZATION_ASSUME_ABSTRACT(IMessage) 
}