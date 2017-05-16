#pragma once
#include "MessageBase.h"

namespace hiveNetworkTopology
{
	class CKeepAliveMessage : public IMessage
	{
	public:
		CKeepAliveMessage();
		virtual ~CKeepAliveMessage() {};

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