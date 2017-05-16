#pragma once
#include "MessageBase.h"

namespace hiveNetworkTopology
{
	class CKeepAliveReplyMessage : public IMessage
	{
	public:
		CKeepAliveReplyMessage();
		virtual ~CKeepAliveReplyMessage() {};

		virtual hiveNetworkCommon::IUserPacket* clonePacketV() override;

	private:
		friend class boost::serialization::access;

		template<class Archive>
		void serialize(Archive& vAr, const unsigned vVersion)
		{
			vAr & BOOST_SERIALIZATION_BASE_OBJECT_NVP(IMessage);
		}
	};
}