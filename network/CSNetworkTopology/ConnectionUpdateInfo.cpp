#include "ConnectionUpdateInfo.h"
#include "common\HiveCommonMicro.h"
#include "common\CommonInterface.h"

using namespace hiveNetworkTopology;

CConnectionUpdateInfo::CConnectionUpdateInfo() : m_State(false)
{

}

CConnectionUpdateInfo::~CConnectionUpdateInfo()
{
	__clearVector(m_AddedSet);
	__clearVector(m_DeletedSet);
}

CConnectionUpdateInfo& CConnectionUpdateInfo::operator=(CConnectionUpdateInfo& vConnectUpdateInfo)
{
	m_State = vConnectUpdateInfo.isUpdated();
	std::vector<SConnectionInfo*> TempAddVec = vConnectUpdateInfo.getAddedConnectionInfo();
	std::vector<SConnectionInfo*> TempDelVec = vConnectUpdateInfo.getDeletedConnectionInfo();

	for (unsigned int i=0; i<TempAddVec.size(); i++)
	{
		SConnectionInfo* temp = new SConnectionInfo(*TempAddVec[i]);
		m_AddedSet.push_back(temp);
	}

	for (unsigned int i=0; i<TempDelVec.size(); i++)
	{
		SConnectionInfo* temp = new SConnectionInfo(*TempDelVec[i]);
		m_DeletedSet.push_back(temp);
	}
	return *this;
}

//******************************************************************
//FUNCTION:
const std::vector<SConnectionInfo*>& CConnectionUpdateInfo::getAddedConnectionInfo() const
{
	return m_AddedSet;
}

//******************************************************************
//FUNCTION:
const std::vector<SConnectionInfo*>& CConnectionUpdateInfo::getDeletedConnectionInfo() const
{
	return m_DeletedSet;
}

//******************************************************************
//FUNCTION:
void CConnectionUpdateInfo::_addConnetion(const SConnectionInfo* vClientInfo)
{
	m_State = true;
	SConnectionInfo* ClientInfo = new SConnectionInfo(vClientInfo->ConnectionIP, vClientInfo->ConnectionTCPPort, vClientInfo->ConnectionUDPPort, vClientInfo->ConnectionUpdateTime);
	m_AddedSet.push_back(ClientInfo);
}

//******************************************************************
//FUNCTION:
void CConnectionUpdateInfo::_deleteConnection(const SConnectionInfo* vClientInfo)
{
	m_State = true;
	SConnectionInfo* ClientInfo = new SConnectionInfo(vClientInfo->ConnectionIP, vClientInfo->ConnectionTCPPort, vClientInfo->ConnectionUDPPort, vClientInfo->ConnectionUpdateTime);
	m_DeletedSet.push_back(ClientInfo);
}

//******************************************************************
//FUNCTION:
void CConnectionUpdateInfo::_clear()
{
	__clearVector(m_AddedSet);
	__clearVector(m_DeletedSet);
	m_State = false;
}

//******************************************************************
//FUNCTION:
void CConnectionUpdateInfo::__clearVector(std::vector<SConnectionInfo*>& vVector)
{
	if (m_State == true)
	{
		auto itr = vVector.begin();

		for (; itr!=vVector.end(); itr++)
		{
			if (*itr != nullptr)
			{
				delete (*itr);
				*itr = nullptr;
			}
		}
	}

	vVector.clear();
}