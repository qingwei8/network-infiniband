#pragma once
#include "MessageBase.h"

namespace hiveNetworkTopology
{
	class CRegisterReplyMessage : public IMessage
	{
	public:
		CRegisterReplyMessage();
		virtual ~CRegisterReplyMessage() {};

		void setBuildTCPConnectionFlag() {m_IsNeedBuildTCPConnection = true;}
		bool isNeedBuildTCPConnection() {return m_IsNeedBuildTCPConnection;}

		virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;

	private:
		bool m_IsNeedBuildTCPConnection;

		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& vAr, const unsigned vVersion)
		{
			vAr & BOOST_SERIALIZATION_BASE_OBJECT_NVP(IMessage);
			vAr & m_IsNeedBuildTCPConnection;
		}
	};
}