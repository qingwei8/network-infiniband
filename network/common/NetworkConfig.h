//#pragma once
//#include "common/HiveConfig.h"
//#include "NetworkExport.h"
//
//namespace hiveNetworkCommon
//{
//	class NETWORK_DLL_EXPORT CNetworkConfig : public hiveConfig::CHiveConfig
//	{
//	public:
//		CNetworkConfig();
//		virtual ~CNetworkConfig();
//
//	protected:
//		virtual void _onLoadedV () override;
//		virtual bool _verifyConfigV() const override;
//		
//		virtual void _extralOnLoadedV() = 0;
//		virtual bool _isValidNodeConfigurationV(const hiveConfig::CHiveConfig *vNodeConfig) const {return true;}
//		
//	private:
//		bool __isValidNodeConfiguration(const hiveConfig::CHiveConfig *vNodeConfig) const;
//	};
//}