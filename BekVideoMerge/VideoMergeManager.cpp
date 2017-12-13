#include "stdafx.h"
#include "VideoMergeManager.h"

#define MODULE_NAME	_T("CVideoMergeManager")

CVideoMergeManager::CVideoMergeManager()
{
}

CVideoMergeManager::~CVideoMergeManager()
{
}

bool CVideoMergeManager::StartWork()
{
	L_TRACE_ENTER(_T("\n"));

	InitParameter();





	L_TRACE_LEAVE(_T("\n"));

	return true;
}

void CVideoMergeManager::InitParameter()
{
	CWinUtils::GetCurrentProcessPath(m_sProgramPath);
	m_sLogPath = m_sProgramPath + _T("\\log");
	m_sConfPath = m_sProgramPath + _T("\\conf");
	m_sResPath = m_sProgramPath + _T("\\res");
	m_sMapPath = m_sResPath + _T("\\map");
	m_sThirdPartyPath = m_sProgramPath + _T("\\3rd");
	m_sVideoPath = m_sProgramPath + _T("\\video");

	L_DEBUG(_T("m_sProgramPath=%s\n"), m_sProgramPath.c_str());
	L_DEBUG(_T("m_sLogPath=%s\n"), m_sLogPath.c_str());
	L_DEBUG(_T("m_sConfPath=%s\n"), m_sConfPath.c_str());
	L_DEBUG(_T("m_sResPath=%s\n"), m_sResPath.c_str());
	L_DEBUG(_T("m_sMapPath=%s\n"), m_sMapPath.c_str());
	L_DEBUG(_T("m_sThirdPartyPath=%s\n"), m_sThirdPartyPath.c_str());
	L_DEBUG(_T("m_sVideoPath=%s\n"), m_sVideoPath.c_str());
}

bool CVideoMergeManager::InitDB()
{
	return true;
}
