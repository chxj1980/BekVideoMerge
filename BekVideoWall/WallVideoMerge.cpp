#include "stdafx.h"
#include "WallVideoMerge.h"

#define MODULE_NAME	_T("CWallVideoMerge")

CWallVideoMerge::CWallVideoMerge()
{
	m_wsProgramPath = _T("");
	m_mapChannels.clear();
	m_mapCarEncoders.clear();
}

CWallVideoMerge::~CWallVideoMerge()
{
}

bool CWallVideoMerge::StartWork()
{
	L_TRACE_ENTER(_T("\n"));

	//参数初始化
	InitParameter();

	//初始化数据库连接
	if (!InitDB())
	{
		return false;
	}

	//读取视频通道配置信息
	if (!InitVideoChannel())
	{
		return false;
	}

	//初始化合码器
	if (!InitDevice())
	{
		return false;
	}

	if (!Run())
	{
		return false;
	}
	
	L_TRACE_LEAVE(_T("\n"));
	return true;
}

void CWallVideoMerge::InitParameter()
{
	L_TRACE_ENTER(_T("\n"));

	CWinUtils::GetCurrentProcessPath(m_wsProgramPath);
	L_DEBUG(_T("m_sProgramPath=%s\n"), m_wsProgramPath.c_str());

	L_TRACE_LEAVE(_T("\n"));
}

//初始化数据库连接
bool CWallVideoMerge::InitDB()
{
	L_TRACE_ENTER(_T("\n"));

	try
	{
		//从配置文件读取数据库相关配置
		TCHAR buf[MAX_PATH];
		wstring wsDBConfPath = m_wsProgramPath + CONF_PATH_DB;
		wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
		wstring wsDBAddress = _T("");	//数据库地址
		wstring wsDBUsername = _T("");	//数据库用户名
		wstring wsDBPassword = _T("");		//数据库密码
		wstring wsDBInstance = _T("");		//数据库实例
		int nDBType = 0;		//数据库类型
		if (GetPrivateProfileString(CONF_SECTION_CONFIG, CONF_KEY_DBADDRESS, _T(""),
			buf, sizeof(buf) / sizeof(TCHAR), wsDBConfPath.c_str()))
		{
			wsDBAddress = Base64Decode(buf);
		}
		if (GetPrivateProfileString(CONF_SECTION_CONFIG, CONF_KEY_USERNAME, _T(""),
			buf, sizeof(buf) / sizeof(TCHAR), wsDBConfPath.c_str()))
		{
			wsDBUsername = Base64Decode(buf);
		}
		if (GetPrivateProfileString(CONF_SECTION_CONFIG, CONF_KEY_PASSWORD, _T(""),
			buf, sizeof(buf) / sizeof(TCHAR), wsDBConfPath.c_str()))
		{
			wsDBPassword = Base64Decode(buf);
		}
		if (GetPrivateProfileString(CONF_SECTION_CONFIG, CONF_KEY_INSTANCE, _T(""),
			buf, sizeof(buf) / sizeof(TCHAR), wsDBConfPath.c_str()))
		{
			wsDBInstance = Base64Decode(buf);
		}
		nDBType = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_SQLORACLE, 0, wsEnvConfPath.c_str());

		L_DEBUG(_T("wsDBAddress=%s, wsDBUsername=%s, wsDBPassword=%s, wsDBInstance=%s\n"),
			wsDBAddress.c_str(), wsDBUsername.c_str(), wsDBPassword.c_str(), wsDBInstance.c_str());
		if (wsDBAddress.empty() || wsDBUsername.empty() || wsDBPassword.empty() || wsDBInstance.empty())
		{
			L_ERROR(_T("conf error, please check conf file : %s\n"), CONF_PATH_DB);
			return false;
		}

		//根据数据库类型编写数据库连接字符串
		wstring wsConnection = _T("");
		if (DB_ORACLE == nDBType)
		{
			L_INFO(_T("Database Type : Oracle\n"));
			wsConnection = CStringUtils::Format(
				_T("Provider=MSDAORA.1;Data Source=(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=%s)(PORT=%s))(CONNECT_DATA=(SERVICE_NAME=%s)));User ID=%s;Password=%s"),
				wsDBAddress.c_str(), _T("1521"), wsDBInstance.c_str(), wsDBUsername.c_str(), wsDBPassword.c_str());
		}
		else if (DB_SQL == nDBType)
		{
			L_INFO(_T("Database Type : Sql Server\n"));
			wsConnection = CStringUtils::Format(
				_T("Driver={SQL Server};Server=%s;Trusted_Connection=no;Database=%s;Uid=%s;Pwd=%s;"),
				wsDBAddress.c_str(), wsDBInstance.c_str(), wsDBUsername.c_str(), wsDBPassword.c_str());
		}
		else
		{
			L_ERROR(_T("Unknown database type, Please check conf file\n"));
			return false;
		}
		L_DEBUG(_T("Database connection string : %s\n"), wsConnection.c_str());

		//初始化数据库连接
		HRESULT hr = m_pDB.CreateInstance("ADODB.Connection");
		if (NULL == m_pDB)
		{
			L_ERROR(_T("m_pDB.CreateInstance failed\n"));
			return false;
		}
		hr = m_pDB->Open((_bstr_t)wsConnection.c_str(), "", "", 0);
		if (S_OK == hr)
		{
			L_INFO(_T("Connect Database Success\n"));
		}
		else
		{
			L_ERROR(_T("Connect Database Failed\n"));
			return false;
		}
	}
	catch (_com_error e)
	{
		L_ERROR(_T("InitDB catch an error : %s"), e.ErrorMessage());
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

//从数据库读取视频通道配置
bool CWallVideoMerge::InitVideoChannel()
{
	L_TRACE_ENTER(_T("\n"));
	m_mapChannels.clear();

	try
	{
		wstring wsSql = CStringUtils::Format(_T("select %s,%s,%s,%s,%s,%s,%s,%s,%s from %s order by %s"),
			DB_FIELD_BH,
			DB_FIELD_SBIP,
			DB_FIELD_YHM,
			DB_FIELD_MM,
			DB_FIELD_DKH,
			DB_FIELD_TDH,
			DB_FIELD_TRANSMODE,
			DB_FIELD_MEDIAIP,
			DB_FIELD_NID,
			DB_TABLE_TBKVIDEO,
			DB_FIELD_BH);

		VARIANT cnt;
		cnt.vt = VT_INT;
		_RecordsetPtr pSet = m_pDB->Execute((_bstr_t)wsSql.c_str(), &cnt, adCmdUnknown);
		_variant_t vat;
		if (pSet != NULL && !pSet->adoEOF)
		{
			while (!pSet->adoEOF)
			{
				wstring wsBh = _T("");
				wstring wsNid = _T("");

				vat = pSet->GetCollect(DB_FIELD_BH);
				if (vat.vt != NULL)
				{
					wsBh = (_bstr_t)vat;
				}
				vat = pSet->GetCollect(DB_FIELD_NID);
				if (vat.vt != NULL)
				{
					wsNid = (_bstr_t)vat;
				}

				if (!wsBh.empty() && !wsNid.empty())
				{
					wstring wsKey = wsBh + _T("_") + wsNid;
					vat = pSet->GetCollect(DB_FIELD_BH);
					if (vat.vt != NULL)
					{
						strncpy(m_mapChannels[wsKey].szDeviceNo, (LPCSTR)(_bstr_t)vat, 10);
					}
					vat = pSet->GetCollect(DB_FIELD_SBIP);
					if (vat.vt != NULL)
					{
						strncpy(m_mapChannels[wsKey].szDeviceIP, (LPCSTR)(_bstr_t)vat, 16);
					}
					vat = pSet->GetCollect(DB_FIELD_YHM);
					if (vat.vt != NULL)
					{
						strncpy(m_mapChannels[wsKey].szUsername, (LPCSTR)(_bstr_t)vat, 16);
					}
					vat = pSet->GetCollect(DB_FIELD_MM);
					if (vat.vt != NULL)
					{
						strncpy(m_mapChannels[wsKey].szPassword, (LPCSTR)(_bstr_t)vat, 16);
					}
					vat = pSet->GetCollect(DB_FIELD_DKH);
					if (vat.vt != NULL)
					{
						m_mapChannels[wsKey].dwPort = atoi((_bstr_t)vat);
					}
					vat = pSet->GetCollect(DB_FIELD_TDH);
					if (vat.vt != NULL)
					{
						m_mapChannels[wsKey].dwChannel = atoi((_bstr_t)vat);
					}
					vat = pSet->GetCollect(DB_FIELD_TRANSMODE);
					if (vat.vt != NULL)
					{
						m_mapChannels[wsKey].nStreamType = atoi((_bstr_t)vat);
					}
					vat = pSet->GetCollect(DB_FIELD_MEDIAIP);
					if (vat.vt != NULL)
					{
						strncpy(m_mapChannels[wsKey].szMediaIP, (LPCSTR)(_bstr_t)vat, 16);
					}
				}
				pSet->MoveNext();
			}
		}
	}
	catch (...)
	{
		L_ERROR(_T("InitVideoChannel catch an error\n"));
		return false;
	}

	L_INFO(_T("Read channel infos from database end, count = %d\n"), m_mapChannels.size());

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

//合码器初始化
bool CWallVideoMerge::InitDevice()
{
	L_TRACE_ENTER(_T("\n"));

	try
	{
		//海康sdk初始化
		if (!CBekHikUtil::InitSDK())
		{
			return false;
		}

		//从配置文件读取相关配置
		wstring wsCarConfPath = m_wsProgramPath + CONF_PATH_CAR_WALL;
		int nNum = GetPrivateProfileInt(CONF_SECTION_JMQ, CONF_KEY_NUM, 0, wsCarConfPath.c_str());	//解码器数量
		for (int nDeviceNo = 1; nDeviceNo <= nNum; nDeviceNo++)
		{
			TCHAR buf[MAX_PATH];
			wstring key = CStringUtils::Format(_T("%d"), nDeviceNo);
			if (GetPrivateProfileString(CONF_SECTION_JMQ, key.c_str(), _T(""), buf, sizeof(buf) / sizeof(TCHAR), wsCarConfPath.c_str()))
			{
				//解析设备IP、用户名、密码、端口
				wstring wsSource = buf;
				wstring separator = _T(",");
				vector<wstring> vecWstr;
				CStringUtils::SplitString(wsSource, separator, vecWstr);
				if (vecWstr.size() != 4)
				{
					L_ERROR(_T("JMQ Config error, No=%d, value=%s\n"), nDeviceNo, wsSource.c_str());
					return false;
				}
				wstring wsIP = vecWstr[0];
				wstring wsUsername = vecWstr[1];
				wstring wsPassword = vecWstr[2];
				wstring wsPort = vecWstr[3];
				string sPort = "";
				CStringUtils::Unicode2ASCII(wsPort, sPort);
				int nPort = atoi(sPort.c_str());
				if (wsIP.empty() || wsUsername.empty() || wsPassword.empty() || nPort <= 0)
				{
					L_ERROR(_T("JMQ Config error, No=%d, value=%s\n"), nDeviceNo, wsSource.c_str());
					return false;
				}

				//登录设备
				int nUserId = -1;
				if (!CBekHikUtil::LoginDevice(wsIP, wsUsername, wsPassword, nPort, nUserId))
				{
					L_ERROR(_T("LoginDevice failed, IP=%s\n"), wsIP.c_str());
					return false;
				}

				//获取设备能力集
				NET_DVR_MATRIX_ABILITY_V41 struDecAbility;
				memset(&struDecAbility, 0, sizeof(NET_DVR_MATRIX_ABILITY_V41));
				if (!CBekHikUtil::GetDeviceAbility(nUserId, struDecAbility))
				{
					L_ERROR(_T("GetDeviceAbility failed, IP=%s\n"), wsIP.c_str());
					return false;
				}

				//设置定时重启
				CBekHikUtil::SetAutoReboot(nUserId, 7, 5, 9);

				//合码器通道对应考车初始化
				if (!InitDVIChannel(nUserId, nDeviceNo, struDecAbility))
				{
					L_ERROR(_T("InitDVIChannel failed\n"));
					return false;
				}
				
			}
		}

	}
	catch (...)
	{
		L_ERROR(_T("InitDevice catch an error\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

//合码器通道检测及初始化
bool CWallVideoMerge::InitDVIChannel(int userId, int deviceNo, NET_DVR_MATRIX_ABILITY_V41 struDecAbility)
{
	L_TRACE_ENTER(_T("\n"));

	if (userId < 0 || deviceNo <= 0)
	{
		L_ERROR(_T("InitDVIChannel parameter error, userId=%d, deviceNo=%d\n"), userId, deviceNo);
		return false;
	}

	try
	{
		//从配置文件读取相关配置
		map<int, int> mapDviCar;
		map<int, int> mapIndexCar;
		wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
		wstring wsCarConfPath = m_wsProgramPath + CONF_PATH_CAR_WALL;
		wstring wsSection = CStringUtils::Format(_T("JMQ%d"), deviceNo);
		int nCarIndex = 0;
		for (int nDviChanNo = 0; nDviChanNo < struDecAbility.struDviInfo.byChanNums; nDviChanNo++)	//DVI通道循环
		{
			wstring wsKey = CStringUtils::Format(_T("BNC%d"), nDviChanNo + 1);
			int nCarNo = GetPrivateProfileInt(wsSection.c_str(), wsKey.c_str(), 0, wsCarConfPath.c_str());
			if (nCarNo <= 0)
			{
				L_INFO(_T("%s of %s not configured\n"), wsKey.c_str(), wsSection.c_str());
				continue;
			}

			if (0 == mapDviCar.count(nDviChanNo))
			{
				mapDviCar[nDviChanNo] = nCarNo;
			}
			if (0 == mapIndexCar.count(nCarIndex))
			{
				mapIndexCar[nCarIndex++] = nCarNo;
			}
		}

		//关闭电视墙全部窗口
		if (!NET_DVR_RemoteControl(userId, NET_DVR_CLOSE_ALL_WND, NULL, NULL))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_RemoteControl NET_DVR_CLOSE_ALL_WND fail, errorCode=%d\n"), errorCode);
			return false;
		}

		//取显示输出号
		DWORD dwRet = -1;
		NET_DVR_DISPLAYCFG struDisplayCfg = { 0 };
		struDisplayCfg.dwSize = sizeof(NET_DVR_DISPLAYCFG);
		if (!NET_DVR_GetDVRConfig(userId, NET_DVR_GET_VIDEOWALLDISPLAYNO, 0, &struDisplayCfg,
			sizeof(struDisplayCfg), &dwRet))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_GetDVRConfig NET_DVR_GET_VIDEOWALLDISPLAYNO fail, errorCode=%d\n"), errorCode);
			return false;
		}

		//关联电视墙显示窗口
		DWORD dwWallNo = 1 << 24;	//墙号
		DWORD dwDispNum = mapDviCar.size();
		DWORD *pDispChan = new DWORD[dwDispNum];
		memset(pDispChan, 0, dwDispNum * sizeof(DWORD));
		int nDispIndex = 0;
		for (map<int, int>::iterator it = mapDviCar.begin(); it != mapDviCar.end(); it++)
		{
			try
			{
				int nChan = (it->first);
				DWORD dwDisplayNo = struDisplayCfg.struDisplayParam[nChan].dwDisplayNo;
				pDispChan[nDispIndex++] = dwDisplayNo;
			}
			catch (...)
			{
				L_ERROR(_T("chan %d not exist"), it->first);
				delete[] pDispChan;
				return false;
			}
		}

		DWORD *pDispStatus = new DWORD[dwDispNum];
		NET_DVR_VIDEOWALLDISPLAYPOSITION *pStruWallDispPos = new NET_DVR_VIDEOWALLDISPLAYPOSITION[dwDispNum];
		memset(pDispStatus, 0, dwDispNum * sizeof(DWORD));
		memset(pStruWallDispPos, 0, dwDispNum * sizeof(NET_DVR_VIDEOWALLDISPLAYPOSITION));
		if (!NET_DVR_GetDeviceConfig(userId, NET_DVR_GET_VIDEOWALLDISPLAYPOSITION, dwDispNum, pDispChan,
			sizeof(DWORD) * dwDispNum, pDispStatus, pStruWallDispPos, dwDispNum * sizeof(NET_DVR_VIDEOWALLDISPLAYPOSITION)))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_GetDeviceConfig NET_DVR_GET_VIDEOWALLDISPLAYPOSITION fail, errorCode=%d\n"), errorCode);
			delete[] pStruWallDispPos;
			delete[] pDispStatus;
			delete[] pDispChan;
			return false;
		}

		for (int i = 0; i < dwDispNum; i++)
		{
			pStruWallDispPos[i].byEnable = 1;
			pStruWallDispPos[i].dwVideoWallNo = dwWallNo;
			pStruWallDispPos[i].struRectCfg.dwXCoordinate = (i % 4) * 1920;	//电视墙每行放4个显示通道
			pStruWallDispPos[i].struRectCfg.dwYCoordinate = (i / 4) * 1920;
			pStruWallDispPos[i].struRectCfg.dwWidth = 1920;
			pStruWallDispPos[i].struRectCfg.dwHeight = 1920;
		}
		if (!NET_DVR_SetDeviceConfig(userId, NET_DVR_SET_VIDEOWALLDISPLAYPOSITION, dwDispNum, pDispChan,
			sizeof(DWORD) * dwDispNum, pDispStatus, pStruWallDispPos,
			dwDispNum * sizeof(NET_DVR_VIDEOWALLDISPLAYPOSITION)))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_SetDeviceConfig NET_DVR_SET_VIDEOWALLDISPLAYPOSITION fail, errorCode=%d\n"), errorCode);
			delete[] pStruWallDispPos;
			delete[] pDispStatus;
			delete[] pDispChan;
			return false;
		}
	
		//开窗 ，每个显示通道开两个窗
		DWORD *pWinNo = new DWORD[2 * dwDispNum];		
		DWORD *pRetWinNo = new DWORD[2 * dwDispNum];
		DWORD *pWinStatus = new DWORD[2 * dwDispNum];
		NET_DVR_VIDEOWALLWINDOWPOSITION *pStruWinPos = new NET_DVR_VIDEOWALLWINDOWPOSITION[2 * dwDispNum];
		memset(pWinNo, 0, 2 * dwDispNum * sizeof(DWORD));
		memset(pRetWinNo, 0, 2 * dwDispNum * sizeof(DWORD));
		memset(pWinStatus, 0, 2 * dwDispNum * sizeof(DWORD));
		memset(pStruWinPos, 0, 2 * dwDispNum * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION));
		for (int i = 0; i < (2 * dwDispNum); i++)
		{
			pWinNo[i] = 1 << 24;		//开窗时不用传入窗口号，只需要传入墙号即可
			pStruWinPos[i].dwSize = sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
			pStruWinPos[i].byEnable = 1;
			if (0 == i % 2)	//每辆考车的第一个窗口，用于展示副驾视频
			{
				pStruWinPos[i].struRect.dwXCoordinate = ((i / 2) % 4) * 1920 + 1000;
				pStruWinPos[i].struRect.dwYCoordinate = ((i / 2) / 4) * 1920 + 200;
				pStruWinPos[i].struRect.dwWidth = 800;
				pStruWinPos[i].struRect.dwHeight = 800;
			}
			else
			{
				pStruWinPos[i].struRect.dwXCoordinate = ((i / 2) % 4) * 1920;
				pStruWinPos[i].struRect.dwYCoordinate = ((i / 2) / 4) * 1920;
				pStruWinPos[i].struRect.dwWidth = 1920;
				pStruWinPos[i].struRect.dwHeight = 1920;
			}
		}
		NET_DVR_IN_PARAM struInputParam = { 0 };
		NET_DVR_OUT_PARAM struOutputParam = { 0 };
		struInputParam.struCondBuf.pBuf = pWinNo;
		struInputParam.struCondBuf.nLen = 2 * dwDispNum * sizeof(DWORD);
		struInputParam.dwRecvTimeout = 5000;
		struInputParam.struInParamBuf.pBuf = pStruWinPos;
		struInputParam.struInParamBuf.nLen = 2 * dwDispNum * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
		struOutputParam.lpStatusList = pWinStatus;
		struOutputParam.struOutBuf.pBuf = pRetWinNo;
		struOutputParam.struOutBuf.nLen = 2 * dwDispNum * sizeof(DWORD);
		if (!NET_DVR_SetDeviceConfigEx(userId, NET_DVR_SET_VIDEOWALLWINDOWPOSITION, 2 * dwDispNum, &struInputParam,
			&struOutputParam))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_SetDeviceConfigEx NET_DVR_SET_VIDEOWALLWINDOWPOSITION fail, errorCode=%d\n"), errorCode);
			delete[] pWinNo;
			delete[] pRetWinNo;
			delete[] pWinStatus;
			delete[] pStruWinPos;
			delete[] pStruWallDispPos;
			delete[] pDispStatus;
			delete[] pDispChan;
			return false;
		}

		//初始化考车对应通道信息
		for (int i = 0; i < dwDispNum; i++)
		{
			int nCarNo = mapIndexCar[i];		//考车号
			DWORD carChan = pRetWinNo[2 * i];	//副驾视频窗口
			DWORD itemChan = pRetWinNo[2 * i + 1];		//项目视频窗口

			L_DEBUG(_T("carNo=%d, carChan=%d, itemChan=%d\n"), nCarNo, carChan, itemChan);

			//窗口置底
			DWORD dwCarChan[1];
			dwCarChan[0] = itemChan;
			if (!NET_DVR_RemoteControl(userId, NET_DVR_SWITCH_WIN_BOTTOM, dwCarChan, sizeof(DWORD)))
			{
				int errorCode = NET_DVR_GetLastError();
				L_ERROR(_T("NET_DVR_RemoteControl NET_DVR_SWITCH_WIN_BOTTOM fail, carNo=%d,errorCode=%d\n"), 
					nCarNo, errorCode);
			}

			if (0 == m_mapCarEncoders.count(nCarNo))
			{
				m_mapCarEncoders[nCarNo].InitCar(userId, nCarNo, carChan, itemChan);
			}
		}

		delete[] pWinNo;
		delete[] pRetWinNo;
		delete[] pWinStatus;
		delete[] pStruWinPos;
		delete[] pStruWallDispPos;
		delete[] pDispStatus;
		delete[] pDispChan;
	}
	catch (...)
	{
		L_ERROR(_T("InitDVIChannel catch an error\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

bool CWallVideoMerge::Run()
{
	CHANNEL_CONFIG channel;
	map<int, CEncodeManager>::iterator it;
	for (it = m_mapCarEncoders.begin(); it != m_mapCarEncoders.end(); it++)
	{
		wstring key = _T("");
		key = CStringUtils::Format(_T("考车%d_2"), it->first);
		if (!GetVideoChannel(key, channel))
		{
			L_ERROR(_T("Video channel 2 of car %d not configured\n"), it->first);
		}
		else
		{
			it->second.StartCarEncode(channel);
		}

		key = CStringUtils::Format(_T("考车%d_1"), it->first);
		if (!GetVideoChannel(key, channel))
		{
			L_ERROR(_T("Video channel 1 of car %d not configured\n"), it->first);
		}
		else
		{
			it->second.StartItemEncode(channel);
		}
	}
	
	return true;
}

bool CWallVideoMerge::GetVideoChannel(wstring key, CHANNEL_CONFIG &videoChannel)
{
	map<wstring, CHANNEL_CONFIG>::iterator it = m_mapChannels.find(key);
	if (m_mapChannels.end() == it)
	{
		return false;
	}

	videoChannel = it->second;
	return true;
}
