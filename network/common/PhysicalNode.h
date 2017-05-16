#pragma once
#include <map>
#include <string>
#include <boost/any.hpp>
#include "NetworkExport.h"
#include "common/Singleton.h"

namespace hiveConfig
{
	class CHiveConfig;
}

namespace hiveNetworkCommon
{
	class CLogicalNode;

	class NETWORK_DLL_EXPORT CPhysicalNode : public hiveCommon::CSingleton<CPhysicalNode>
	{
	public:
		virtual ~CPhysicalNode();

		CLogicalNode* findLogicalNodeByName(const std::string& vName) const;      //NOTE: 这里提供了两个不同的接口来寻找logical node，其原因在logical node的名字是由用户定义，在不同应用中，用户可能使用不同的名字，而protocal是固定的
		CLogicalNode* findLogicalNodeByProtocal(const std::string& vProtocal) const;  //ASSUMPTION: 这个函数假设一种网络协议只能有一个对应的logical node
		unsigned int  getNumLogicalNodes() const {return m_LogicalNodeSet.size();}

		void stop();
		void startLogicalNode(const std::string& vNodeName);
		void updateLogicalNodeConfig(const std::string& vNodeName, const std::string& vAttributePath, boost::any vAttributeValue);
		bool init(const hiveConfig::CHiveConfig *vConfig);

		std::string getIP() const;

		const hiveConfig::CHiveConfig* getConfig() const {return m_pConfig;}

	private:
		CPhysicalNode();

		const hiveConfig::CHiveConfig *m_pConfig;
		std::map<std::string, CLogicalNode*> m_LogicalNodeSet;  // <std::string, CLogicalNode*> = <node name, node pointer>
		
		void __registerLogicalNode(CLogicalNode *vNode);
		void __createLogicalNodes(const hiveConfig::CHiveConfig *vNetworkConfig);
		void __destroyAllLogicalNodes();

	friend class hiveCommon::CSingleton<CPhysicalNode>;
	};
}