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
		L_ERROR(_T("LoginDevice catch an error"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

