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

//处理车载信号
bool CCarManager::HandleCarSignal(CarSignal signal)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.SetCarSignal(signal);
	}

	return true;
}

//考试开始
bool CCarManager::Handle17C51(StudentInfo studentInfo)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.Handle17C51();
	}
	
	return true;
}

//项目开始
bool CCarManager::Handle17C52(int xmNo, wstring xmName)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.Handle17C52(xmNo, xmName);
	}
	
	return true;
}

//项目扣分
bool CCarManager::Handle17C53(ERROR_DATA judgeInfo)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.Handle17C53(judgeInfo);
	}

	return true;
}

//项目结束
bool CCarManager::Handle17C55(int xmNo, wstring xmName)
{
	if (m_bDrawMap)
	{
		m_mapRefleshClass.Handle17C55(xmNo, xmName);
	}

	return true;
}

//考试结束
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

