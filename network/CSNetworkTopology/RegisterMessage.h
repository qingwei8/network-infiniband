#pragma once
#include "MessageBase.h"

namespace hiveNetworkTopology
{
	class CRegisterMessage : public IMessage
	{
	public:
		CRegisterMessage();
		virtual ~CRegisterMessage() {}

		bool getKeepAliveOpenFlag() {return m_KeepAliveOpenFlag;}
		void setKeepAliveOpenFlag(bool vFlag) {m_KeepAliveOpenFlag = vFlag;}
		virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;

	private:
		bool m_KeepAliveOpenFlag;

		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& vAr, const unsigned vVersion)
		{
			vAr & BOOST_SERIALIZATION_BASE_OBJECT_NVP(IMessage);
			vAr & m_KeepAliveOpenFlag;
		}
	};
}