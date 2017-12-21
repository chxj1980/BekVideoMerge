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

bool CBekHikUtil::SetDecChanEnable(int userId, int byChanNo)
{
	L_TRACE_ENTER(_T("\n"));

	if (userId < 0 || byChanNo <= 0)
	{
		L_ERROR(_T("Parameter error, userId=%d, byChanNo=%d\n"), userId, byChanNo);
		return false;
	}

	try
	{
		DWORD dwEnable = -1;
		if (!NET_DVR_MatrixGetDecChanEnable(userId, byChanNo, &dwEnable))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_MatrixGetDecChanEnable failed, errorCode=%d\n"), errorCode);
			return false;
		}

		if (0 == dwEnable)	//0��ʾͨ���ر�
		{
			if (!NET_DVR_MatrixSetDecChanEnable(userId, byChanNo, 1))
			{
				int errorCode = NET_DVR_GetLastError();
				L_ERROR(_T("NET_DVR_MatrixSetDecChanEnable failed, errorCode=%d\n"), errorCode);
				return false;
			}
		}
	}
	catch (...)
	{
		L_ERROR(_T("SetDecChanEnable catch an error\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

bool CBekHikUtil::GetDisplayCfg(int userId, int dispChan, NET_DVR_MATRIX_VOUTCFG vOutCfg)
{
	L_TRACE_ENTER(_T("\n"));

	if (userId < 0 || dispChan <= 0)
	{
		L_ERROR(_T("Parameter error, userId=%d, dispChan=%d\n"), userId, dispChan);
		return false;
	}

	try
	{
		memset(&vOutCfg, 0, sizeof(NET_DVR_MATRIX_VOUTCFG));
		if (!NET_DVR_MatrixGetDisplayCfg_V41(userId, dispChan, &vOutCfg))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_MatrixGetDisplayCfg_V41 failed, errorCode=%d\n"), errorCode);
			return false;
		}
	}
	catch (...)
	{
		L_ERROR(_T("GetDisplayCfg catch an error\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

bool CBekHikUtil::SetDisplayCfg(int userId, int dispChan, NET_DVR_MATRIX_VOUTCFG vOutCfg)
{
	L_TRACE_ENTER(_T("\n"));

	if (userId < 0 || dispChan <= 0)
	{
		L_ERROR(_T("Parameter error, userId=%d, dispChan=%d\n"), userId, dispChan);
		return false;
	}

	try
	{
		if (!NET_DVR_MatrixSetDisplayCfg_V41(userId, dispChan, &vOutCfg))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_MatrixSetDisplayCfg_V41 failed, errorCode=%d\n"), errorCode);
			return false;
		}
	}
	catch (...)
	{
		L_ERROR(_T("SetDisplayCfg catch an error\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

bool CBekHikUtil::StartDynamicDecode(int userId, int decChan, CHANNEL_CONFIG videoChan)
{
	L_TRACE_ENTER(_T("\n"));

	if (decChan <= 0)
	{
		L_ERROR(_T("Parameter error, decChan=%d\n"), decChan);
		return false;
	}

	try
	{
		NET_DVR_PU_STREAM_CFG_V41 struStreamCfgV41;
		memset(&struStreamCfgV41, 0, sizeof(NET_DVR_PU_STREAM_CFG_V41));
		struStreamCfgV41.dwSize = sizeof(NET_DVR_PU_STREAM_CFG_V41);
		struStreamCfgV41.byStreamMode = 1;//ͨ��IP������ȡ��
		struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.byChanType = 0;//ͨ�����ͣ�0-��ͨͨ��,1-��ͨ��,2-��ID,3-��������Դ��4-������������ͨ��
		struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.byChannel = 0;//�ò�����Ч��ͨ���ż�dwChannel 
		struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.byTransProtocol = 0;//����Э������0-TCP��1-UDP	
		struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.byFactoryType = 0;/*ǰ���豸��������,ͨ���ӿڻ�ȡ*/
		
		struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.byTransMode = videoChan.nStreamType;//��������ģʽ 0�������� 1��������    
		struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.wDVRPort = videoChan.dwPort;//�˿ں�
		struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.dwChannel = videoChan.dwChannel;//ͨ����
		sprintf((char *)struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.byAddress, "%s", videoChan.szDeviceIP);//�豸����
		sprintf((char *)struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.sUserName, "%s", videoChan.szUsername);//��½�ʺ�
		sprintf((char *)struStreamCfgV41.uDecStreamMode.struDecStreamDev.struDevChanInfo.sPassword, "%s", videoChan.szPassword);//����	
		
		if (strlen(videoChan.szMediaIP) != 0)	//��ý��
		{
			struStreamCfgV41.uDecStreamMode.struDecStreamDev.struStreamMediaSvrCfg.byValid = 1;//������ý��
			struStreamCfgV41.uDecStreamMode.struDecStreamDev.struStreamMediaSvrCfg.wDevPort = 554;//�˿�
			struStreamCfgV41.uDecStreamMode.struDecStreamDev.struStreamMediaSvrCfg.byTransmitType = 0;//TCP
			sprintf((char *)struStreamCfgV41.uDecStreamMode.struDecStreamDev.struStreamMediaSvrCfg.byAddress, "%s", videoChan.szMediaIP);//��ý��IP
		}

		if (!NET_DVR_MatrixStartDynamic_V41(userId, decChan, &struStreamCfgV41))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_MatrixStartDynamic_V41 failed, errorCode=%d\n"), errorCode);
			return false;
		}
	}
	catch (...)
	{
		L_ERROR(_T("StartDynamicDecode catch an error\n"));
		return false;
	}

	L_INFO(_T("StartDynamicDecode success, decChan=%d\n"), decChan);
	L_TRACE_LEAVE(_T("\n"));
	return true;
}

bool CBekHikUtil::StartPassiveDecode(int userId, int decChan, LONG &lpHandle)
{
	L_TRACE_ENTER(_T("\n"));

	if (decChan <= 0)
	{
		L_ERROR(_T("Parameter error, decChan=%d\n"), decChan);
		return false;
	}

	try
	{
		NET_DVR_MATRIX_PASSIVEMODE  passiveMode;
		passiveMode.wTransProtol = 0;//wTransProtol 	����Э�飺0-TCP��1-UDP��2-MCAST 
		passiveMode.wPassivePort = 8000;//TCP����UDP�˿ڣ�TCPʱ�˿�Ĭ����8000����ͬ�Ľ���ͨ��UDP�˿ں�������Ϊ��ͬ��ֵ
		passiveMode.byStreamType = 2;//��������: 1-ʵʱ��, 2-�ļ���

		lpHandle = NET_DVR_MatrixStartPassiveDecode(userId, decChan, &passiveMode);
		if (lpHandle < 0)
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_MatrixStartPassiveDecode failed, errorCode=%d\n"), errorCode);
			return false;
		}
	}
	catch (...)
	{
		L_ERROR(_T("StartPassiveDecode catch an error\n"));
		return false;
	}

	L_INFO(_T("StartPassiveDecode success, decChan=%d\n"), decChan);
	L_TRACE_LEAVE(_T("\n"));
	return true;
}

bool CBekHikUtil::StopDynamicDecode(int userId, int decChan)
{
	L_TRACE_ENTER(_T("\n"));

	if (decChan <= 0)
	{
		L_ERROR(_T("Parameter error, decChan=%d\n"), decChan);
		return false;
	}

	try
	{
		if (!NET_DVR_MatrixStopDynamic(userId, decChan))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_MatrixStopDynamic failed, errorCode=%d\n"), errorCode);
			return false;
		}
	}
	catch (...)
	{
		L_ERROR(_T("StopDynamicDecode catch an error\n"));
		return false;
	}

	L_INFO(_T("StopDynamicDecode success, decChan=%d\n"), decChan);
	L_TRACE_LEAVE(_T("\n"));
	return true;
}

bool CBekHikUtil::MatrixSendData(int lpHandle, char *sendBuf, DWORD dwSize)
{
	L_TRACE_ENTER(_T("\n"));

	if (lpHandle < 0)
	{
		L_ERROR(_T("Parameter error, lpHandle=%d\n"), lpHandle);
		return false;
	}

	try
	{
		if (!NET_DVR_MatrixSendData(lpHandle, sendBuf, dwSize))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_MatrixStopDynamic failed, errorCode=%d\n"), errorCode);
			return false;
		}
	}
	catch (...)
	{
		L_ERROR(_T("StopDynamicDecode catch an error\n"));
		return false;
	}

	L_DEBUG(_T("MatrixSendData success, lpHandle=%d\n"), lpHandle);
	L_TRACE_LEAVE(_T("\n"));
	return true;
}

