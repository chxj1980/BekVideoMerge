#include "stdafx.h"
#include "BekHikUtil.h"

#define  MODULE_NAME	_T("BekHikUtil")

bool CBekHikUtil::InitSDK()
{
	bool bRet = NET_DVR_Init();
	if (!bRet)
	{
		L_ERROR(_T("NET_DVR_Init Failed\n"));
	}
	else
	{
		L_INFO(_T("NET_DVR_Init Success\n"));

		wstring wstrCurrentPath = _T("");
		wstring wstrSdkLogPath = _T("");
		string strLogPath = "";
		CWinUtils::GetCurrentProcessPath(wstrCurrentPath);
		wstrSdkLogPath = wstrCurrentPath + _T("\\SdkLog");
		CStringUtils::Unicode2ASCII(wstrSdkLogPath, strLogPath);

		NET_DVR_SetLogToFile(3, (char*)strLogPath.c_str(), true);
	}

	return bRet;
}

bool CBekHikUtil::LoginDevice(wstring ip, wstring username, wstring password, int port, int &userId)
{
	L_TRACE_ENTER(_T("\n"));

	if (ip.empty() || username.empty() || password.empty() || port <= 0)
	{
		L_ERROR(_T("Parameter error, ip=%s, username=%s, password=%s, port=%d\n"), ip.c_str(), username.c_str(), password.c_str(), port);
		return false;
	}

	try
	{
		string sIP = "";
		string sUsername = "";
		string sPassword = "";
		CStringUtils::Unicode2ASCII(ip, sIP);
		CStringUtils::Unicode2ASCII(username, sUsername);
		CStringUtils::Unicode2ASCII(password, sPassword);
		
		NET_DVR_DEVICEINFO_V30 struDeviceInfo;
		memset(&struDeviceInfo, 0, sizeof(NET_DVR_DEVICEINFO_V30));
		userId = NET_DVR_Login_V30((char*)sIP.c_str(), port, (char*)sUsername.c_str(), (char*)sPassword.c_str(), &struDeviceInfo);
		if (userId < 0)
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_Login_V30 failed, errorCode=%d\n"), errorCode);
			return false;
		}
	}
	catch (...)
	{
		L_ERROR(_T("LoginDevice catch an error\n"));
		return false;
	}

	L_INFO(_T("LoginDevice success\n"));
	L_TRACE_LEAVE(_T("\n"));
	return true;
}

bool CBekHikUtil::GetDeviceAbility(int userId, NET_DVR_MATRIX_ABILITY_V41 &struDecAbility)
{
	L_TRACE_ENTER(_T("\n"));
	
	if (userId < 0)
	{
		L_ERROR(_T("Parameter error, userId=%d\n"), userId);
		return false;
	}

	try
	{
		memset(&struDecAbility, 0, sizeof(NET_DVR_MATRIX_ABILITY_V41));
		if (!NET_DVR_GetDeviceAbility(userId, MATRIXDECODER_ABILITY_V41, NULL, 0, (char*)&struDecAbility, sizeof(NET_DVR_MATRIX_ABILITY_V41)))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_GetDeviceAbility failed, errorCode=%d\n"), errorCode);
			return false;
		}
	}
	catch (...)
	{
		L_ERROR(_T("GetDeviceAbility catch an error\n"));
		return false;
	}

	L_INFO(_T("GetDeviceAbility success\n"));
	L_TRACE_LEAVE(_T("\n"));
	return true;
}

bool CBekHikUtil::SetAutoReboot(int userId, int byDate, int byHour, int byMinute)
{
	L_TRACE_ENTER(_T("\n"));

	if (userId < 0)
	{
		L_ERROR(_T("Parameter error, userId=%d\n"), userId);
		return false;
	}

	try
	{
		DWORD dwRet = 0;
		NET_DVR_AUTO_REBOOT_CFG struRebootCfg;
		memset(&struRebootCfg, 0, sizeof(NET_DVR_AUTO_REBOOT_CFG));
		if (!NET_DVR_GetDVRConfig(userId, NET_DVR_GET_AUTO_REBOOT_CFG,  0, &struRebootCfg, sizeof(NET_DVR_AUTO_REBOOT_CFG), &dwRet))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_GetDVRConfig failed, errorCode=%d\n"), errorCode);
			return false;
		}

		if (struRebootCfg.struRebootTime.byDate != byDate)
		{
			struRebootCfg.dwSize = sizeof(NET_DVR_AUTO_REBOOT_CFG);
			struRebootCfg.struRebootTime.byDate = byDate;
			struRebootCfg.struRebootTime.byHour = byHour;
			struRebootCfg.struRebootTime.byMinute = byMinute;
			if (!NET_DVR_SetDVRConfig(userId, NET_DVR_SET_AUTO_REBOOT_CFG, 0, &struRebootCfg, sizeof(NET_DVR_AUTO_REBOOT_CFG)))
			{
				int errorCode = NET_DVR_GetLastError();
				L_ERROR(_T("NET_DVR_SetDVRConfig failed, errorCode=%d\n"), errorCode);
				return false;
			}
		}
	}
	catch (...)
	{
		L_ERROR(_T("SetAutoReboot catch an error\n"));
		return false;
	}

	L_INFO(_T("SetAutoReboot success, byDate=%d, byHour=%d, byMinute=%d\n"), byDate, byHour, byMinute);
	L_TRACE_LEAVE(_T("\n"));
	return true;
}

