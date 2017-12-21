#include "stdafx.h"
#include "CarManager.h"

#define MODULE_NAME	_T("CCarManager")

CCarManager::CCarManager()
{
	m_nUserId = -1;
	m_nCarNo = -1;
	m_nStudentInfoHandle = -1;
	m_nMapHandle = -1;

	m_wsProgramPath = _T("");
}

CCarManager::~CCarManager()
{
	
}

void CCarManager::InitCar(int userId, int carNo, BYTE (&decChan)[DISPLAY_CHAN_NUMS])
{
	m_nUserId = userId;
	m_nCarNo = carNo;
	for (int i = 0; i < DISPLAY_CHAN_NUMS; i++)
	{
		m_decChan[i] = decChan[i];
	}

	CWinUtils::GetCurrentProcessPath(m_wsProgramPath);
}

void CCarManager::InitPassiveMode(int studentInfoHandle, int mapHandle)
{
	m_nStudentInfoHandle = studentInfoHandle;
	m_nMapHandle = mapHandle;

	m_studentInfoRefleshClass.Init(m_wsProgramPath, m_nCarNo, m_nStudentInfoHandle);
	m_studentInfoRefleshClass.StartWork();

	m_mapRefleshClass.Init(m_wsProgramPath, m_nCarNo, m_nMapHandle);
	m_mapRefleshClass.StartWork();
}

//开启动态解码
bool CCarManager::StartDynamicDecode(CHANNEL_CONFIG channel, int wnd)
{
	return CBekHikUtil::StartDynamicDecode(m_nUserId, m_decChan[wnd], channel);
}

//开启被动解码
bool CCarManager::StartPassiveDecode(int wnd, LONG &lpHandle)
{
	return CBekHikUtil::StartPassiveDecode(m_nUserId, m_decChan[wnd], lpHandle);
}

//停止动态解码
bool CCarManager::StopDynamicDecode(int wnd)
{
	return CBekHikUtil::StopDynamicDecode(m_nUserId, m_decChan[wnd]);
}

void CCarManager::InitStudentInfoPic()
{
	
}

void CCarManager::InitMapPic()
{
}

