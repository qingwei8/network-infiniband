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

		CLogicalNode* findLogicalNodeByName(const std::string& vName) const;      //NOTE: �����ṩ��������ͬ�Ľӿ���Ѱ��logical node����ԭ����logical node�����������û����壬�ڲ�ͬӦ���У��û�����ʹ�ò�ͬ�����֣���protocal�ǹ̶���
		CLogicalNode* findLogicalNodeByProtocal(const std::string& vProtocal) const;  //ASSUMPTION: �����������һ������Э��ֻ����һ����Ӧ��logical node
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