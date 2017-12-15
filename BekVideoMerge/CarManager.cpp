#include "stdafx.h"
#include "CarManager.h"

#define MODULE_NAME	_T("CCarManager")

CCarManager::CCarManager()
{
	m_nUserId = -1;
	m_nCarNo = 0;
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
}

//������̬����
bool CCarManager::StartDynamicDecode(CHANNEL_CONFIG channel, int wnd)
{
	return CBekHikUtil::StartDynamicDecode(m_nUserId, m_decChan[wnd], channel);
}

//������������
bool CCarManager::StartPassiveDecode(int wnd, LONG &lpHandle)
{
	return CBekHikUtil::StartPassiveDecode(m_nUserId, m_decChan[wnd], lpHandle);
}

//ֹͣ��̬����
bool CCarManager::StopDynamicDecode(int wnd)
{
	return CBekHikUtil::StopDynamicDecode(m_nUserId, m_decChan[wnd]);
}