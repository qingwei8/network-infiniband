#pragma once
#include <boost/algorithm/string.hpp>
#include "common\HiveConfig.h"
#include "NetworkExport.h"

namespace hiveNetworkCommon
{
	enum ENodeStartupType;

	class NETWORK_DLL_EXPORT CLogicalNodeConfig : public hiveConfig::CHiveConfig
	{
	public:
		CLogicalNodeConfig(void);
		CLogicalNodeConfig(const CLogicalNodeConfig& vConfig);
		virtual ~CLogicalNodeConfig(void);

		void setNodeCreationSig(const std::string& vSig) {m_NodeCreationSig = boost::to_upper_copy(vSig);}
		void setStartupType(ENodeStartupType vType)      {m_NodeStartupType = vType;}
		void setLocalIP(const std::string& vIP)          {m_LocalIP = vIP;}

		ENodeStartupType   getNodeStartupType() const {return m_NodeStartupType;}
		const std::string& getNodeCreationSig() const {return m_NodeCreationSig;}
		const std::string& getLocalIP() const         {return m_LocalIP;}
		
	protected:
		virtual hiveConfig::CHiveConfig* cloneConfigV() const override;
		virtual bool _verifyConfigV() const override;

	private:
		std::string      m_LocalIP;
		std::string      m_NodeCreationSig;
		ENodeStartupType m_NodeStartupType;
	};
}