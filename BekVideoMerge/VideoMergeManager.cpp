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
	if (!InitChannel())
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

//从数据库读取通道配置
bool CVideoMergeManager::InitChannel()
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
		L_ERROR(_T("InitChannel catch an error\n"));
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
		int nHKDeviceType = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_HMQ, 0, wsEnvConfPath.c_str());	//解码设备类型
		int nNum = GetPrivateProfileInt(CONF_SECTION_JMQ, CONF_KEY_NUM, 0, wsCarConfPath.c_str());	//解码器数量
		if (DEVICE_TYPE_JMQ == nHKDeviceType)
		{
			L_INFO(_T("Hikvison device type : JMQ, count=%d\n"), nNum);
		}
		else if (DEVICE_TYPE_HMQ == nHKDeviceType)
		{
			L_INFO(_T("Hikvison device type : HMQ, count=%d\n"), nNum);
		}
		else
		{
			L_ERROR(_T("Unknown Hikvison device type, HMQ = %d, Please check config file : %s\n"), nHKDeviceType, wsEnvConfPath);
			return false;
		}

		for (int i = 1; i <= nNum; i++)
		{
			TCHAR buf[MAX_PATH];
			wstring key = CStringUtils::Format(_T("%d"), i);
			if (GetPrivateProfileString(CONF_SECTION_JMQ, key.c_str(), _T(""), buf, sizeof(buf)/sizeof(TCHAR), wsCarConfPath.c_str()))
			{
				//解析设备IP、用户名、密码、端口
				wstring wsSource = buf;
				wstring separator = _T(",");
				vector<wstring> vecWstr;
				CStringUtils::SplitString(wsSource, separator, vecWstr);
				if (vecWstr.size() != 4)
				{
					L_ERROR(_T("JMQ Config error, No=%d, value=%s\n"), i, wsSource);
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
					L_ERROR(_T("JMQ Config error, No=%d, value=%s\n"), i, wsSource);
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