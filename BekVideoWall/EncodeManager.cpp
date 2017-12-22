#include "stdafx.h"
#include "EncodeManager.h"

#define MODULE_NAME	_T("CEncodeManager")

CEncodeManager::CEncodeManager()
{
	m_nUserId = -1;
	m_nCarNo = -1;
	m_dwCarWnd = -1;
	m_dwItemWnd = -1;
	m_wsProgramPath = _T("");
}

CEncodeManager::~CEncodeManager()
{
	
}

void CEncodeManager::InitCar(int userId, int carNo, DWORD carWnd, DWORD itemWnd)
{
	m_nUserId = userId;
	m_nCarNo = carNo;
	m_dwCarWnd = carWnd;
	m_dwItemWnd = itemWnd;

	CWinUtils::GetCurrentProcessPath(m_wsProgramPath);
}

bool CEncodeManager::StartCarEncode(CHANNEL_CONFIG channel)
{
	return StartDynamicDecode(channel, m_dwCarWnd);
}

bool CEncodeManager::StartItemEncode(CHANNEL_CONFIG channel)
{
	return StartDynamicDecode(channel, m_dwItemWnd);
}

//开启动态解码
bool CEncodeManager::StartDynamicDecode(CHANNEL_CONFIG channel, DWORD wnd)
{
	return CBekHikUtil::StartDynamicDecode(m_nUserId, wnd, channel);
}

//停止动态解码
bool CEncodeManager::StopDynamicDecode(DWORD wnd)
{
	return CBekHikUtil::StopDynamicDecode(m_nUserId, wnd);
}

