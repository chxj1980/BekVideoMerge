#include "stdafx.h"
#include "VideoMergeManager.h"

#include "Function.h"

#define MODULE_NAME	_T("CVideoMergeManager")

CVideoMergeManager::CVideoMergeManager()
{
	m_wsProgramPath = _T("");
	m_mapChannels.clear();
	m_mapErrorDatas.clear();
}

CVideoMergeManager::~CVideoMergeManager()
{
}

bool CVideoMergeManager::StartWork()
{
	L_TRACE_ENTER(_T("\n"));

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

	//读取项目编号信息、扣分编号信息
	if (!InitErrorData())
	{
		return false;
	}

	//初始化合码器、解码器通道配置
	if (!InitDevice())
	{
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));

	return true;
}

void CVideoMergeManager::InitParameter()
{
	L_TRACE_ENTER(_T("\n"));

	CWinUtils::GetCurrentProcessPath(m_wsProgramPath);
	L_DEBUG(_T("m_sProgramPath=%s\n"), m_wsProgramPath.c_str());

	L_TRACE_LEAVE(_T("\n"));
}

//初始化数据库连接
bool CVideoMergeManager::InitDB()
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
bool CVideoMergeManager::InitVideoChannel()
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
						strncpy(m_mapChannels[wsKey].szMideaIP, (LPCSTR)(_bstr_t)vat, 16);
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

//从数据库读取项目编号信息和扣分编号信息
bool CVideoMergeManager::InitErrorData()
{
	L_TRACE_ENTER(_T("\n"));
	m_mapErrorDatas.clear();

	try
	{
		wstring wsSql = CStringUtils::Format(_T("select %s,%s,%s from %s"),
			DB_FIELD_CWBH,
			DB_FIELD_KFLX,
			DB_FIELD_KCFS,
			DB_TABLE_ERRORDATA);

		VARIANT cnt;
		cnt.vt = VT_INT;
		_RecordsetPtr pSet = m_pDB->Execute((_bstr_t)wsSql.c_str(), &cnt, adCmdUnknown);
		_variant_t vat;
		if (pSet != NULL && !pSet->adoEOF)
		{
			while (!pSet->adoEOF)
			{
				vat = pSet->GetCollect(DB_FIELD_CWBH);
				if (vat.vt != NULL)
				{
					wstring wsNo = (_bstr_t)vat;
					vat = pSet->GetCollect(DB_FIELD_KFLX);
					if (vat.vt != NULL)
					{
						strncpy(m_mapErrorDatas[wsNo].errorlx, (LPCSTR)(_bstr_t)vat, 10);
					}
					vat = pSet->GetCollect(DB_FIELD_KCFS);
					if (vat.vt != NULL)
					{
						m_mapErrorDatas[wsNo].ikcfs = atoi((_bstr_t)vat);
					}
				}

				pSet->MoveNext();
			}
		}
	}
	catch (...)
	{
		L_ERROR(_T("InitErrorData catch an error\n"));
		return false;
	}

	L_INFO(_T("Read ErrorData infos from database end, count = %d\n"), m_mapErrorDatas.size());

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

//合码器、解码器初始化
bool CVideoMergeManager::InitDevice()
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
		wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
		wstring wsCarConfPath = m_wsProgramPath + CONF_PATH_CAR;
		wstring wsDisplayConfPath = m_wsProgramPath + CONF_PATH_DISPLAY;
		int nHKDeviceType = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_HMQ, 0, wsEnvConfPath.c_str());	//解码设备类型
		int nNum = GetPrivateProfileInt(CONF_SECTION_JMQ, CONF_KEY_NUM, 0, wsCarConfPath.c_str());	//解码器数量

		//取子窗口和解码通道的对应关系
		for (int nDisplayNo = 0; nDisplayNo < DISPLAY_CHAN_NUMS; nDisplayNo++)
		{
			wstring wsKey = CStringUtils::Format(_T("DISPLAY%d"), nDisplayNo + 1);
			m_displayChan[nDisplayNo] = GetPrivateProfileInt(CONF_SECTION_CONFIG, wsKey.c_str(), nDisplayNo, wsDisplayConfPath.c_str());
		}
		m_audioWindow = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_VIDEOWND, 1, wsDisplayConfPath.c_str());

		for (int nDeviceNo = 1; nDeviceNo <= nNum; nDeviceNo++)
		{
			TCHAR buf[MAX_PATH];
			wstring key = CStringUtils::Format(_T("%d"), nDeviceNo);
			if (GetPrivateProfileString(CONF_SECTION_JMQ, key.c_str(), _T(""), buf, sizeof(buf)/sizeof(TCHAR), wsCarConfPath.c_str()))
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

				//初始化显示通道和解码通道
				if (DEVICE_TYPE_HMQ == nHKDeviceType)
				{
					if (!InitDVIChannel(nUserId, nDeviceNo, struDecAbility))
					{
						L_ERROR(_T("InitDVIChannel failed\n"));
						return false;
					}
				}
				else if (DEVICE_TYPE_JMQ == nHKDeviceType)
				{
					if (!InitBNCChannel(nUserId, nDeviceNo, struDecAbility))
					{
						L_ERROR(_T("InitDVIChannel failed\n"));
						return false;
					}
				}
				else
				{
					L_ERROR(_T("Unknown Hikvison device type, HMQ = %d, Please check config file : %s\n"), nHKDeviceType, wsEnvConfPath);
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
bool CVideoMergeManager::InitDVIChannel(int userId, int deviceNo, NET_DVR_MATRIX_ABILITY_V41 struDecAbility)
{
	L_TRACE_ENTER(_T("\n"));

	if (userId <0 || deviceNo <= 0)
	{
		L_ERROR(_T("InitDVIChannel parameter error, userId=%d, deviceNo=%d\n"), userId, deviceNo);
		return false;
	}

	try
	{
		//从配置文件读取相关配置
		wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
		wstring wsCarConfPath = m_wsProgramPath + CONF_PATH_CAR;
		wstring wsDisplayConfPath = m_wsProgramPath + CONF_PATH_DISPLAY;
		int nEven = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_EVEN, 0, wsDisplayConfPath.c_str());	//是否隔行解码

		wstring wsSection = CStringUtils::Format(_T("JMQ%d"), deviceNo);
		for (int nDviChanNo = 0; nDviChanNo < struDecAbility.struDviInfo.byChanNums; nDviChanNo++)	//DVI通道循环
		{
			if ((DECODE_EVEN_YES == nEven) && (1 == nDviChanNo % 2))	//隔行解码
			{
				continue;
			}

			wstring wsKey = CStringUtils::Format(_T("BNC%d"), nDviChanNo + 1);
			int nCarNo = GetPrivateProfileInt(wsSection.c_str(), wsKey.c_str(), 0, wsCarConfPath.c_str());
			if (nCarNo <= 0)
			{
				L_INFO(_T("%s of %s not configured\n"), wsKey.c_str(), wsSection.c_str());
				continue;
			}

			//开启解码通道
			BYTE byDecChan[DISPLAY_CHAN_NUMS];	//显示通道对应的解码通道
			for (int j = 0; j < DISPLAY_CHAN_NUMS; j++)
			{
				byDecChan[j] = struDecAbility.byStartChan + nDviChanNo * DISPLAY_CHAN_NUMS + j;
				int chanNo = byDecChan[j];
				if (!CBekHikUtil::SetDecChanEnable(userId, chanNo))
				{
					L_ERROR(_T("Enable %s of %s failed\n"), wsKey.c_str(), wsSection.c_str());
					return false;
				}
			}

			//配置显示通道
			NET_DVR_MATRIX_VOUTCFG vOutCfg;
			memset(&vOutCfg, 0, sizeof(NET_DVR_MATRIX_VOUTCFG));
			DWORD dwDispChan = struDecAbility.struDviInfo.byStartChan + nDviChanNo;	//显示通道
			if (!CBekHikUtil::GetDisplayCfg(userId, dwDispChan, vOutCfg))
			{
				return false;
			}
			if (DISPLAY_CHAN_NUMS != vOutCfg.dwWindowMode)	//画面分割模式
			{
				vOutCfg.dwWindowMode = DISPLAY_CHAN_NUMS;
			}
			for (int nDecChanNo = 0; nDecChanNo < DISPLAY_CHAN_NUMS; nDecChanNo++)	//子窗口关联的解码通道
			{
				if (vOutCfg.byJoinDecChan[nDecChanNo] != byDecChan[m_displayChan[nDecChanNo]])
				{
					vOutCfg.byJoinDecChan[nDecChanNo] = byDecChan[m_displayChan[nDecChanNo]];
				}
			}
			vOutCfg.byAudio = 1;	//音频
			vOutCfg.byAudioWindowIdx = m_audioWindow;	//音频窗口
			vOutCfg.byVedioFormat = 1;	//视频制式：1- NTSC，2- PAL				//这项配置与解码器不同
			vOutCfg.dwResolution = 67207228;													//这项配置与解码器不同
			vOutCfg.byScale = 0;	//真实
			if (!CBekHikUtil::SetDisplayCfg(userId, dwDispChan, vOutCfg))
			{
				return false;
			}

			//考车初始化

		}
	}
	catch (...)
	{
		L_ERROR(_T("InitDVIChannel catch an error\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

//解码器通道检测及初始化
bool CVideoMergeManager::InitBNCChannel(int userId, int deviceNo, NET_DVR_MATRIX_ABILITY_V41 struDecAbility)
{
	L_TRACE_ENTER(_T("\n"));

	if (userId < 0 || deviceNo <= 0)
	{
		L_ERROR(_T("InitBNCChannel parameter error, userId=%d, deviceNo=%d\n"), userId, deviceNo);
		return false;
	}

	try
	{
		//从配置文件读取相关配置
		wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
		wstring wsCarConfPath = m_wsProgramPath + CONF_PATH_CAR;
		wstring wsDisplayConfPath = m_wsProgramPath + CONF_PATH_DISPLAY;
		int nEven = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_EVEN, 0, wsDisplayConfPath.c_str());	//是否隔行解码

		wstring wsSection = CStringUtils::Format(_T("JMQ%d"), deviceNo);
		for (int nBncChanNo = 0; nBncChanNo < struDecAbility.struBncInfo.byChanNums; nBncChanNo++)	//BNC通道循环
		{
			if ((DECODE_EVEN_YES == nEven) && (1 == nBncChanNo % 2))	//隔行解码
			{
				continue;
			}

			wstring wsKey = CStringUtils::Format(_T("BNC%d"), nBncChanNo + 1);
			int nCarNo = GetPrivateProfileInt(wsSection.c_str(), wsKey.c_str(), 0, wsCarConfPath.c_str());
			if (nCarNo <= 0)
			{
				L_INFO(_T("%s of %s not configured\n"), wsKey.c_str(), wsSection.c_str());
				continue;
			}

			//开启解码通道
			BYTE byDecChan[DISPLAY_CHAN_NUMS];	//显示通道对应的解码通道
			for (int j = 0; j < DISPLAY_CHAN_NUMS; j++)
			{
				byDecChan[j] = (BYTE)(struDecAbility.byStartChan + nBncChanNo * DISPLAY_CHAN_NUMS + j);
				if (!CBekHikUtil::SetDecChanEnable(userId, byDecChan[j]))
				{
					L_ERROR(_T("Enable %s of %s failed\n"), wsKey.c_str(), wsSection.c_str());
					return false;
				}
			}

			//配置显示通道
			NET_DVR_MATRIX_VOUTCFG vOutCfg;
			memset(&vOutCfg, 0, sizeof(NET_DVR_MATRIX_VOUTCFG));
			DWORD dwDispChan = struDecAbility.struBncInfo.byStartChan + nBncChanNo;	//显示通道
			if (!CBekHikUtil::GetDisplayCfg(userId, dwDispChan, vOutCfg))
			{
				return false;
			}
			if (DISPLAY_CHAN_NUMS != vOutCfg.dwWindowMode)	//画面分割模式
			{
				vOutCfg.dwWindowMode = DISPLAY_CHAN_NUMS;
			}
			for (int nDecChanNo = 0; nDecChanNo < DISPLAY_CHAN_NUMS; nDecChanNo++)	//子窗口关联的解码通道
			{
				if (vOutCfg.byJoinDecChan[nDecChanNo] != byDecChan[m_displayChan[nDecChanNo]])
				{
					vOutCfg.byJoinDecChan[nDecChanNo] = byDecChan[m_displayChan[nDecChanNo]];
				}
			}
			vOutCfg.byAudio = 1;	//音频
			vOutCfg.byAudioWindowIdx = m_audioWindow;	//音频窗口
			vOutCfg.byVedioFormat = 2;	//视频制式：1- NTSC，2- PAL			//这项配置与合码器不同
			vOutCfg.dwResolution = 0;																//这项配置与合码器不同
			vOutCfg.byScale = 0;	//真实
			if (!CBekHikUtil::SetDisplayCfg(userId, dwDispChan, vOutCfg))
			{
				return false;
			}

			//考车初始化

		}
	}
	catch (...)
	{
		L_ERROR(_T("InitBNCChannel catch an error\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}