#pragma once
#include "common/BaseProduct.h"
#include "NetworkCommon.h"
#include "NetworkExport.h"

namespace hiveNetworkCommon
{
	class CLogicalNode;

	class NETWORK_DLL_EXPORT IUserPacket : public hiveCommon::CBaseProduct
	{
	public:
		IUserPacket();
		IUserPacket(const std::string& vPacketSig);
		IUserPacket(const IUserPacket *vPacket);
		virtual ~IUserPacket();

		const std::string& getPacketSourceIP() const {return m_PacketSourceIP;}

		virtual void processPacketV(CLogicalNode* vHostNode) { }
		virtual bool isReady2SendV() const;

		virtual IUserPacket* clonePacketV();

	protected:
		void _setPacketSourceIP(const std::string& vIP) { _ASSERTE(!vIP.empty() && (vIP != DUMMY_IP)); m_PacketSourceIP = vIP; }

	private:
		std::string m_PacketSourceIP;

		template<class TArchieve>
		void serialize(TArchieve& vAr, const unsigned int vVersion)
		{
			vAr & boost::serialization::base_object<hiveCommon::CBaseProduct>(*this);
			vAr & m_PacketSourceIP;
			_ASSERTE(!m_PacketSourceIP.empty());
		}

	friend class boost::serialization::access;
	friend class CLogicalNode;
	};
}