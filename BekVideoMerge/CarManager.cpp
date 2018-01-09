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
	m_bDrawMap = false;
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

	wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
	int nDrawMap = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_LOADMAP, 0, wsEnvConfPath.c_str());
	if (1 == nDrawMap)
	{
		m_bDrawMap = true;
	}
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

//�������ź�
bool CCarManager::HandleCarSignal(CarSignal signal)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.SetCarSignal(signal);
	}

	return true;
}

//���Կ�ʼ
bool CCarManager::Handle17C51(StudentInfo studentInfo)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.Handle17C51();
	}
	
	return true;
}

//��Ŀ��ʼ
bool CCarManager::Handle17C52(int xmNo, wstring xmName)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.Handle17C52(xmNo, xmName);
	}
	
	return true;
}

//��Ŀ�۷�
bool CCarManager::Handle17C53(ERROR_DATA judgeInfo)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.Handle17C53(judgeInfo);
	}

	return true;
}

//��Ŀ����
bool CCarManager::Handle17C55(int xmNo, wstring xmName)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.Handle17C55(xmNo, xmName);
	}

	return true;
}

//���Խ���
bool CCarManager::Handle17C56(bool bPass, int nScore)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.Handle17C56(bPass, nScore);
	}

	return true;
}

void CCarManager::InitStudentInfoPic()
{
	
}

void CCarManager::InitMapPic()
{
}

