#include "stdafx.h"
#include "VideoMergeManager.h"

#define MODULE_NAME	_T("CVideoMergeManager")

CVideoMergeManager::CVideoMergeManager()
{
	m_wsProgramPath = _T("");
	m_mapCarManagers.clear();
	m_mapChannels.clear();
	m_mapItems.clear();

	m_nKskm = 0;
	m_nDBType = 0;
	m_bDynamicVideo = false;
	m_bBigCar = false;
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
	if (!InitItems())
	{
		return false;
	}

	//初始化合码器、解码器通道配置
	if (!InitDevice())
	{
		return false;
	}

	//开始运行
	if (!Run())
	{
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));

	return true;
}

//处理考试过程信号
bool CVideoMergeManager::HandleExamSignal(wstring buf)
{
	vector<wstring> vecWstr;
	CStringUtils::SplitString(buf, _T("*"), vecWstr);
	//if (8 != vecWstr.size() && 9 != vecWstr.size())
	//{
	//	L_ERROR(_T("HandleExamSignal error, exam signal data error, buf=%s\n"), buf.c_str());
	//	return false;
	//}

	try
	{
		int nCarNo = _wtoi(vecWstr[1].c_str());	//考车号
		if (!GetCarManager(nCarNo))
		{
			L_ERROR(_T("HandleExamSignal error, car%d not found\n"), nCarNo);
			return false;
		}

		wstring wsCertificateNo = vecWstr[6];	//准考证明编号
		wstring wsTime = vecWstr[7];	//时间

		//以下几个变量只有特定信号类型才会传入
		wstring wsXmNo = _T("");	//项目编号
		wstring wsJudgeNo = _T("");	//扣分编号
		int nScore = 0;		//考试得分

		int nSignalType = _wtoi(vecWstr[2].c_str());	//考试过程信号类型
		switch (nSignalType)
		{
		case SIGNAL_TYPE_17C51:	//考试开始，没有特殊字段要处理
		{
			StudentInfo studentInfo;
			GetStudentInfo(wsCertificateNo, nCarNo, studentInfo);

			m_mapCarManagers[nCarNo].Handle17C51(studentInfo);
			
			break;
		}

		case SIGNAL_TYPE_17C52:	//项目开始，取项目编号
		{
			wsXmNo = vecWstr[5];

			//动态切换到对应的项目视频
			if (m_bDynamicVideo)
			{
				CHANNEL_CONFIG channel;
				wstring key = wsXmNo + CStringUtils::Format(_T("_%d"), XM_CAMERA_NO_1);
				if (!GetVideoChannel(key, channel))
				{
					L_INFO(_T("Video channel %s not configured\n"), key.c_str());
				}
				else
				{
					m_mapCarManagers[nCarNo].StartDynamicDecode(channel, 5);
				}
			}

			if (0 == m_mapItems.count(wsXmNo))
			{
				L_ERROR(_T("xm NO : %s not found\n"), wsXmNo.c_str());
				break;
			}
			int nXmNo = _wtoi(wsXmNo.c_str());
			wstring wsXmName = _T("");
			CStringUtils::ASCII2Unicode(m_mapItems[wsXmNo].errorlx, wsXmName);
			m_mapCarManagers[nCarNo].Handle17C52(nXmNo, wsXmName);
		
			break;
		}

		case SIGNAL_TYPE_17C53:	//项目扣分，取项目编号和扣分编号
		{
			vector<wstring> vecNos;
			CStringUtils::SplitString(vecWstr[5], _T("@"), vecNos);
			if (2 != vecNos.size())
			{
				L_ERROR(_T("CVideoMergeManager HandleExamSignal catch an error, exam signal data error, buf=%s\n"), buf.c_str());
				return false;
			}
			
			wsXmNo = vecNos[0];
			wsJudgeNo = vecNos[1];

			if (0 == m_mapItems.count(wsJudgeNo))
			{
				L_ERROR(_T("Judgement number : %s not exist."), wsJudgeNo);
			}
			else
			{
				ERROR_DATA judgeInfo = m_mapItems[wsJudgeNo];
				m_mapCarManagers[nCarNo].Handle17C53(judgeInfo);
			}

			break;
		}

		case SIGNAL_TYPE_17C55:	//项目结束，取项目编号
		{
			wsXmNo = vecWstr[5];

			wstring key = _T("");
			if (KSKM_2 == m_nKskm)	
			{
				key = CAMERA_KM2_PUBLIC;	//科目二项目结束时，切换到远景视频
			}
			else
			{
				//fix me
				//旧的软件，这里科目三车前摄像头编号配置为2，后续应该统一调整为3，参考枚举类型CarCameraNoLocation
				key = CStringUtils::Format(_T("考车%d_2"), nCarNo);	//科目三项目结束时切换为车前摄像头
			}

			CHANNEL_CONFIG channel;
			if (!GetVideoChannel(key, channel))
			{
				L_INFO(_T("Video channel %s not configured\n"), key.c_str());
			}
			else
			{
				m_mapCarManagers[nCarNo].StartDynamicDecode(channel, 5);
			}

			if (0 == m_mapItems.count(wsXmNo))
			{
				L_ERROR(_T("xm NO : %s not found\n"), wsXmNo.c_str());
				break;
			}
			int nXmNo = _wtoi(wsXmNo.c_str());
			wstring wsXmName = _T("");
			CStringUtils::ASCII2Unicode(m_mapItems[wsXmNo].errorlx, wsXmName);
			m_mapCarManagers[nCarNo].Handle17C55(nXmNo, wsXmName);

			break;
		}

		case SIGNAL_TYPE_17C56:	//考试结束，取考试得分
		{
			nScore = _wtoi(vecWstr[5].c_str());
			if (nScore < 0)
			{
				nCarNo = 0;
			}

			int nPassScore = 0;	//考试合格分数
			if (KSKM_2 == m_nKskm)
			{
				if (m_bBigCar)
				{
					nPassScore = EXAM_PASS_SCORE_KM2_BIGCAR;
				}
				else
				{
					nPassScore = EXAM_PASS_SCORE_KM2;
				}
			}
			else
			{
				nPassScore = EXAM_PASS_SCORE_KM3;
			}

			bool bPass = (nScore >= nPassScore);
			m_mapCarManagers[nCarNo].Handle17C56(bPass, nScore);

			break;
		}
		
		default:
			break;
		}
	}
	catch (...)
	{
		L_ERROR(_T("CVideoMergeManager HandleExamSignal catch an error, exam signal data error, buf=%s\n"), buf.c_str());
		return false;
	}

	return true;
} 

//处理车载信号
bool CVideoMergeManager::HandleCarSignal(int carNo, char* buf)
{
	if (!GetCarManager(carNo))
	{
		L_ERROR(_T("HandleCarSignal error, car%d not found\n"), carNo);
		return false;
	}

	CarSignal signal;
	memcpy((char *)&signal, buf, sizeof(CarSignal));
	L_DEBUG(_T("CVideoMergeManager HandleCarSignal, carNo=%d, x=%lf, y=%lf, angle=%lf, speed=%lf, mileage=%lf\n"),
		carNo, signal.dX, signal.dY, signal.fDirectionAngle, signal.fSpeed, signal.fMileage);

	m_mapCarManagers[carNo].HandleCarSignal(signal);

	return true;
}

void CVideoMergeManager::InitParameter()
{
	L_TRACE_ENTER(_T("\n"));

	CWinUtils::GetCurrentProcessPath(m_wsProgramPath);
	L_DEBUG(_T("m_sProgramPath=%s\n"), m_wsProgramPath.c_str());

	wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
	wstring wsDisplayConfPath = m_wsProgramPath + CONF_PATH_DISPLAY;

	//考试科目
	m_nKskm = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_KSKM, 2, wsEnvConfPath.c_str());
	L_INFO(_T("m_nKskm=%d\n"), m_nKskm);

	//数据库类型
	m_nDBType = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_SQLORACLE, 0, wsEnvConfPath.c_str());
	L_INFO(_T("m_nDBType=%d\n"), m_nDBType);

	//车型是否是大车
	int nBigCar = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_BIGCAR, 0, wsEnvConfPath.c_str());
	if (1 == nBigCar)
	{
		m_bBigCar = true;
	}

	//画面二是否做动态切换
	int nWnd2 = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_WND2, 0, wsDisplayConfPath.c_str());
	if (1 == nWnd2)
	{
		m_bDynamicVideo = true;
	}

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
		
		L_DEBUG(_T("wsDBAddress=%s, wsDBUsername=%s, wsDBPassword=%s, wsDBInstance=%s\n"), 
			wsDBAddress.c_str(), wsDBUsername.c_str(), wsDBPassword.c_str(), wsDBInstance.c_str());
		if (wsDBAddress.empty() || wsDBUsername.empty() || wsDBPassword.empty() || wsDBInstance.empty())
		{
			L_ERROR(_T("conf error, please check conf file : %s\n"), CONF_PATH_DB);
			return false;
		}

		//根据数据库类型编写数据库连接字符串
		wstring wsConnection = _T("");
		if (DB_ORACLE == m_nDBType)
		{
			L_INFO(_T("Database Type : Oracle\n"));
			wsConnection = CStringUtils::Format(
				_T("Provider=MSDAORA.1;Data Source=(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)(HOST=%s)(PORT=%s))(CONNECT_DATA=(SERVICE_NAME=%s)));User ID=%s;Password=%s"),
				wsDBAddress.c_str(), _T("1521"), wsDBInstance.c_str(), wsDBUsername.c_str(), wsDBPassword.c_str());
		}
		else if (DB_SQL == m_nDBType)
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

//从数据库读取项目编号信息和扣分编号信息
bool CVideoMergeManager::InitItems()
{
	L_TRACE_ENTER(_T("\n"));
	m_mapItems.clear();

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
						strncpy(m_mapItems[wsNo].errorlx, (LPCSTR)(_bstr_t)vat, 10);
					}
					vat = pSet->GetCollect(DB_FIELD_KCFS);
					if (vat.vt != NULL)
					{
						m_mapItems[wsNo].ikcfs = atoi((_bstr_t)vat);
					}
				}

				pSet->MoveNext();
			}
		}
	}
	catch (...)
	{
		L_ERROR(_T("InitItems catch an error\n"));
		return false;
	}

	L_INFO(_T("Read m_mapItems from database end, count = %d\n"), m_mapItems.size());

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

/*//合码器通道检测及初始化
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
			if (0 == m_mapCarManagers.count(nCarNo))
			{
				m_mapCarManagers[nCarNo].InitCar(userId, nCarNo, byDecChan);
			}
		}
	}
	catch (...)
	{
		L_ERROR(_T("InitDVIChannel catch an error\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}*/

//合码器通道检测及初始化
bool CVideoMergeManager::InitDVIChannel(int userId, int deviceNo, NET_DVR_MATRIX_ABILITY_V41 struDecAbility)
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
		wstring wsCarConfPath = m_wsProgramPath + CONF_PATH_CAR;
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

		//获取电视墙的一些显示参数
		DWORD *pStatus = new DWORD[dwDispNum];
		NET_DVR_WALLOUTPUTPARAM *pStruWallOutput = new NET_DVR_WALLOUTPUTPARAM[dwDispNum];
		if (!NET_DVR_GetDeviceConfig(userId, NET_DVR_WALLOUTPUT_GET, dwDispNum, pDispChan, sizeof(DWORD) * dwDispNum,
			pStatus, pStruWallOutput, dwDispNum * sizeof(NET_DVR_WALLOUTPUTPARAM)))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_GetDeviceConfig NET_DVR_WALLOUTPUT_GET fail, errorCode=%d\n"), errorCode);
		}
		for (int i = 0; i < dwDispNum; i++)
		{
			pStruWallOutput[i].dwResolution = _1080P_60HZ;
		}
		if (!NET_DVR_SetDeviceConfig(userId, NET_DVR_WALLOUTPUT_SET, dwDispNum, pDispChan,
			sizeof(DWORD) * dwDispNum, pStatus, pStruWallOutput, dwDispNum * sizeof(NET_DVR_WALLOUTPUTPARAM)))
		{
			int errorCode = NET_DVR_GetLastError();
			L_ERROR(_T("NET_DVR_GetDeviceConfig NET_DVR_WALLOUTPUT_SET fail, errorCode=%d\n"), errorCode);
		}
		delete[]pStruWallOutput;
		delete[]pStatus;

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
			pStruWallDispPos[i].struRectCfg.dwXCoordinate = (i % 4) * DISPLAY_CHAN_WIDTH;	//电视墙每行放4个显示通道
			pStruWallDispPos[i].struRectCfg.dwYCoordinate = (i / 4) * DISPLAY_CHAN_HEIGHT;
			pStruWallDispPos[i].struRectCfg.dwWidth = DISPLAY_CHAN_WIDTH;
			pStruWallDispPos[i].struRectCfg.dwHeight = DISPLAY_CHAN_HEIGHT;
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

		//开窗 ，每个显示通道开5个窗
		int nWinCount = DISPLAY_CHAN_NUMS;
		DWORD *pWinNo = new DWORD[nWinCount * dwDispNum];
		DWORD *pRetWinNo = new DWORD[nWinCount * dwDispNum];
		DWORD *pWinStatus = new DWORD[nWinCount * dwDispNum];
		NET_DVR_VIDEOWALLWINDOWPOSITION *pStruWinPos = new NET_DVR_VIDEOWALLWINDOWPOSITION[nWinCount * dwDispNum];
		memset(pWinNo, 0, nWinCount * dwDispNum * sizeof(DWORD));
		memset(pRetWinNo, 0, nWinCount * dwDispNum * sizeof(DWORD));
		memset(pWinStatus, 0, nWinCount * dwDispNum * sizeof(DWORD));
		memset(pStruWinPos, 0, nWinCount * dwDispNum * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION));
		for (int i = 0; i < (nWinCount * dwDispNum); i++)
		{
			pWinNo[i] = 1 << 24;		//开窗时不用传入窗口号，只需要传入墙号即可
			pStruWinPos[i].dwSize = sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
			pStruWinPos[i].byEnable = 1;
			if (0 == i % nWinCount)
			{
				pStruWinPos[i].struRect.dwXCoordinate = ((i / nWinCount) % 4) * DISPLAY_CHAN_WIDTH;
				pStruWinPos[i].struRect.dwYCoordinate = ((i / nWinCount) / 4) * DISPLAY_CHAN_HEIGHT;
				pStruWinPos[i].struRect.dwWidth = DISPLAY_CHAN_WIDTH / 3 * 2;
				pStruWinPos[i].struRect.dwHeight = DISPLAY_CHAN_WIDTH / 3 * 2;
			}
			else if (1 == i % nWinCount)
			{
				pStruWinPos[i].struRect.dwXCoordinate = ((i / nWinCount) % 4) * DISPLAY_CHAN_WIDTH;
				pStruWinPos[i].struRect.dwYCoordinate = ((i / nWinCount) / 4) * DISPLAY_CHAN_HEIGHT + DISPLAY_CHAN_HEIGHT / 3 * 2;
				pStruWinPos[i].struRect.dwWidth = DISPLAY_CHAN_WIDTH / 3 * 2;
				pStruWinPos[i].struRect.dwHeight = DISPLAY_CHAN_HEIGHT / 3;
			}
			else if (2 == i % nWinCount)
			{
				pStruWinPos[i].struRect.dwXCoordinate = ((i / nWinCount) % 4) * DISPLAY_CHAN_WIDTH + DISPLAY_CHAN_WIDTH / 3 * 2;
				pStruWinPos[i].struRect.dwYCoordinate = ((i / nWinCount) / 4) * DISPLAY_CHAN_HEIGHT + DISPLAY_CHAN_HEIGHT / 3 * 2;
				pStruWinPos[i].struRect.dwWidth = DISPLAY_CHAN_WIDTH / 3;
				pStruWinPos[i].struRect.dwHeight = DISPLAY_CHAN_HEIGHT / 3;
			}
			else if (3 == i % nWinCount)
			{
				pStruWinPos[i].struRect.dwXCoordinate = ((i / nWinCount) % 4) * DISPLAY_CHAN_WIDTH + DISPLAY_CHAN_WIDTH / 3 * 2;
				pStruWinPos[i].struRect.dwYCoordinate = ((i / nWinCount) / 4) * DISPLAY_CHAN_HEIGHT + DISPLAY_CHAN_HEIGHT / 3;
				pStruWinPos[i].struRect.dwWidth = DISPLAY_CHAN_WIDTH / 3;
				pStruWinPos[i].struRect.dwHeight = DISPLAY_CHAN_HEIGHT / 3;
			}
			else if (4 == i % nWinCount)
			{
				pStruWinPos[i].struRect.dwXCoordinate = ((i / nWinCount) % 4) * DISPLAY_CHAN_WIDTH + DISPLAY_CHAN_WIDTH / 3 * 2;
				pStruWinPos[i].struRect.dwYCoordinate = ((i / nWinCount) / 4) * DISPLAY_CHAN_HEIGHT;
				pStruWinPos[i].struRect.dwWidth = DISPLAY_CHAN_WIDTH / 3;
				pStruWinPos[i].struRect.dwHeight = DISPLAY_CHAN_HEIGHT / 3;
			}
		}
		NET_DVR_IN_PARAM struInputParam = { 0 };
		NET_DVR_OUT_PARAM struOutputParam = { 0 };
		struInputParam.struCondBuf.pBuf = pWinNo;
		struInputParam.struCondBuf.nLen = nWinCount * dwDispNum * sizeof(DWORD);
		struInputParam.dwRecvTimeout = 5000;
		struInputParam.struInParamBuf.pBuf = pStruWinPos;
		struInputParam.struInParamBuf.nLen = nWinCount * dwDispNum * sizeof(NET_DVR_VIDEOWALLWINDOWPOSITION);
		struOutputParam.lpStatusList = pWinStatus;
		struOutputParam.struOutBuf.pBuf = pRetWinNo;
		struOutputParam.struOutBuf.nLen = nWinCount * dwDispNum * sizeof(DWORD);
		if (!NET_DVR_SetDeviceConfigEx(userId, NET_DVR_SET_VIDEOWALLWINDOWPOSITION, nWinCount * dwDispNum, &struInputParam,
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

			BYTE byDecChan[DISPLAY_CHAN_NUMS];	//显示通道对应的解码通道
			for (int j = 0; j < DISPLAY_CHAN_NUMS; j++)
			{
				byDecChan[j] = pRetWinNo[nWinCount * i + j];
			}

			if (0 == m_mapCarManagers.count(nCarNo))
			{
				m_mapCarManagers[nCarNo].InitCar(userId, nCarNo, byDecChan);
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
			if (0 == m_mapCarManagers.count(nCarNo))
			{
				//CCarManager carManager;
				//carManager.InitCar(userId, nCarNo, byDecChan);
				//m_mapCarManagers[nCarNo] = carManager;
				m_mapCarManagers[nCarNo].InitCar(userId, nCarNo, byDecChan);
			}
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

bool CVideoMergeManager::Run()
{
	CHANNEL_CONFIG channel;
	map<int, CCarManager>::iterator it;
	for (it = m_mapCarManagers.begin(); it != m_mapCarManagers.end(); it++)
	{
		//fix me 
		//这里摄像头编号和窗口号索引是写死的
		//车载视频动态解码
		wstring key = CStringUtils::Format(_T("考车%d_1"), it->first);
		if (!GetVideoChannel(key, channel))
		{
			L_INFO(_T("Video channel 1 of car %d not configured\n"), it->first);
		}
		else
		{
			it->second.StartDynamicDecode(channel, 2);
		}
		key = CStringUtils::Format(_T("考车%d_2"), it->first);
		if (!GetVideoChannel(key, channel))
		{
			L_INFO(_T("Video channel 2 of car %d not configured\n"), it->first);
		}
		else
		{
			it->second.StartDynamicDecode(channel, 3);
		}
		key = CStringUtils::Format(_T("考车%d_3"), it->first);
		if (!GetVideoChannel(key, channel))
		{
			L_INFO(_T("Video channel 3 of car %d not configured\n"), it->first);
		}
		else
		{
			it->second.StartDynamicDecode(channel, 4);
		}
		
		//被动解码
		LONG lStudentInfoHandle = -1;
		LONG lMapHandle = -1;
		if (it->second.StartPassiveDecode(0, lMapHandle) && it->second.StartPassiveDecode(1, lStudentInfoHandle))
		{
			it->second.InitPassiveMode(lStudentInfoHandle, lMapHandle);
		}
	}

	return true;
}

//根据编号查找视频通道信息
bool CVideoMergeManager::GetVideoChannel(wstring key, CHANNEL_CONFIG &videoChannel)
{
	map<wstring, CHANNEL_CONFIG>::iterator it = m_mapChannels.find(key);
	if (m_mapChannels.end() == it)
	{
		return false;
	}

	videoChannel = it->second;
	return true;
}

//根据考车号查找考车管理类
bool CVideoMergeManager::GetCarManager(int carNo)
{
	if (0 == m_mapCarManagers.count(carNo))
	{
		return false;
	}
	return true;
}

//从数据库查询考生信息
bool CVideoMergeManager::GetStudentInfo(wstring certificateNo, int carNo, StudentInfo &studentInfo)
{
	L_TRACE_ENTER(_T("\n"));
	
	try
	{
		wstring wsSql = _T("");

		//删除上个考生照片
		wstring wsPhotoName = m_wsProgramPath + FILE_PATH_PHOTO + CStringUtils::Format(PHOTO_ID_FORMAT, carNo);
		if (CWinUtils::FileExists(wsPhotoName))
		{
			DeleteFile(wsPhotoName.c_str());
		}
		wstring wsLoginPhotoName = m_wsProgramPath + FILE_PATH_PHOTO + CStringUtils::Format(PHOTO_LOGIN_FORMAT, carNo);
		if (CWinUtils::FileExists(wsLoginPhotoName))
		{
			DeleteFile(wsLoginPhotoName.c_str());
		}
		
		if (DB_ORACLE == m_nDBType)
		{
			wsSql = _T("select 考车号,SysCfg.备注,考试车型,姓名,性别,to_char(sysdate,'yyyy-mm-dd') as MYDATE, \
					流水号,身份证明编号,驾校名称,考试员1,考试原因,准考证明编号,当日次数 from StudentInfo left join SchoolInfo \
					on 代理人=驾校编号 left join SysCfg on 考车号=项目 where 准考证明编号='") + certificateNo + _T("'");
		}
		else if (DB_SQL == m_nDBType)
		{
			wsSql = _T("select 考车号,SysCfg.备注,考试车型,姓名,性别,(Select CONVERT(varchar(100), GETDATE(), 23)) as MYDATE, \
					流水号,身份证明编号,驾校名称,考试员1,考试原因,准考证明编号,当日次数 from StudentInfo left join SchoolInfo \
					on 代理人=驾校编号 left join SysCfg on 考车号=项目 where 准考证明编号='") + certificateNo + _T("'");
		}

		L_DEBUG(_T("GetStudentInfo sql : %s\n"), wsSql.c_str());

		//取考生信息
		VARIANT cnt;
		cnt.vt = VT_INT;
		_RecordsetPtr pSet = m_pDB->Execute((_bstr_t)wsSql.c_str(), &cnt, adCmdUnknown);
		_variant_t vat;
		if (pSet != NULL && !pSet->adoEOF)
		{
			while (!pSet->adoEOF)
			{
				vat = pSet->GetCollect(DB_FIELD_KCH);	//考车号
				if (vat.vt != NULL)
				{
					studentInfo.wsCarNo = (_bstr_t)vat;
				}
				
				vat = pSet->GetCollect(DB_FIELD_BZ);	//备注，车牌号
				if (vat.vt != NULL)
				{
					studentInfo.wsRemarks = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_KSCX);	//考试车型
				if (vat.vt != NULL)
				{
					studentInfo.wsCarType = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_XINGMING);	//姓名
				if (vat.vt != NULL)
				{
					studentInfo.wsName = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_XB);	//性别
				if (vat.vt != NULL)
				{
					studentInfo.wsGender = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(_T("MYDATE"));	//日期
				if (vat.vt != NULL)
				{
					studentInfo.wsDate = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_LSH);	//流水号
				if (vat.vt != NULL)
				{
					studentInfo.wsSerialNo = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_SFZMBH);	//身份证明编号
				if (vat.vt != NULL)
				{
					studentInfo.wsID = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_JXMC);	//驾校名称
				if (vat.vt != NULL)
				{
					studentInfo.wsDrivingSchool = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_KSY1);	//考试员1
				if (vat.vt != NULL)
				{
					studentInfo.wsExaminer = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_DRCS);	//当日次数
				if (vat.vt != NULL)
				{
					studentInfo.wsDayCount = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_KSYY);	//考试原因
				if (vat.vt != NULL)
				{
					wstring examReasonNo = (_bstr_t)vat;
					studentInfo.wsExamReason = GetExamReason(examReasonNo);
				}

				studentInfo.wsCertificateNo = certificateNo;

				pSet->MoveNext();
			}

			pSet->Close();
			pSet.Release();
		}

		//取考生照片
		_RecordsetPtr pSetPhoto;
		pSetPhoto.CreateInstance("ADODB.Recordset");
		pSetPhoto->CursorLocation = adUseClient;
		wsSql = _T("select 照片,门禁照片 from StudentPhoto where 准考证明编号='") + certificateNo + _T("'");
		pSetPhoto->Open((_variant_t)_bstr_t(wsSql.c_str()), _variant_t((IDispatch*)m_pDB, true),
			adOpenDynamic, adLockOptimistic, adCmdText);
		if (1 != pSetPhoto->RecordCount)
		{
			L_ERROR(_T("student photo not exist, certificateNo = %s\n"), certificateNo.c_str());
			return false;
		}
		long lSizePhoto = pSetPhoto->GetFields()->GetItem("照片")->ActualSize; 
		long lSizeLoginPhoto = pSetPhoto->GetFields()->GetItem("门禁照片")->ActualSize;
		_variant_t vatPhoto;
		_variant_t vatLoginPhoto;
		if (lSizePhoto > 0)
		{
			vatPhoto = pSetPhoto->GetFields()->GetItem("照片")->GetChunk(lSizePhoto);
		}
		if (lSizeLoginPhoto > 0)
		{
			vatLoginPhoto = pSetPhoto->GetFields()->GetItem("门禁照片")->GetChunk(lSizeLoginPhoto);
		}

		pSetPhoto->Close();
		pSetPhoto.Release();

		CLSID pngClsid;
		GetEncoderClsid(L"image/png", &pngClsid);

		if ((VT_ARRAY | VT_UI1) == vatPhoto.vt)
		{
			LPVOID lpPhoto = new char[lSizePhoto + 1];

			char *pBuf = NULL;
			SafeArrayAccessData(vatPhoto.parray, (void **)&pBuf);
			memcpy(lpPhoto, pBuf, lSizePhoto);
			SafeArrayUnaccessData(vatPhoto.parray);

			IStream *pStream = NULL;
			LONGLONG lSize = lSizePhoto;
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, lSize);
			if (NULL == hGlobal)
			{
				L_ERROR(_T("GetStudentInfo fail.\n"));
				return false;
			}

			LPVOID pvData = GlobalLock(hGlobal);
			memcpy(pvData, lpPhoto, lSize);
			GlobalUnlock(hGlobal);
			CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);

			Image *imgPhoto = Image::FromStream(pStream, FALSE);
			imgPhoto->Save(wsPhotoName.c_str(), &pngClsid, NULL);
			delete imgPhoto;

			pStream->Release();
			GlobalFree(hGlobal);
			if (lpPhoto)
			{
				delete lpPhoto;
				lpPhoto = NULL;
			}
		}

		if ((VT_ARRAY | VT_UI1) == vatLoginPhoto.vt)
		{
			LPVOID lpLoginPhoto = new char[lSizeLoginPhoto + 1];

			char *pBuf = NULL;
			SafeArrayAccessData(vatLoginPhoto.parray, (void **)&pBuf);
			memcpy(lpLoginPhoto, pBuf, lSizeLoginPhoto);
			SafeArrayUnaccessData(vatLoginPhoto.parray);

			IStream *pStream = NULL;
			LONGLONG lSize = lSizeLoginPhoto;
			HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, lSize);
			if (NULL == hGlobal)
			{
				L_ERROR(_T("GetStudentInfo fail.\n"));
				return false;
			}

			LPVOID pvData = GlobalLock(hGlobal);
			memcpy(pvData, lpLoginPhoto, lSize);
			GlobalUnlock(hGlobal);
			CreateStreamOnHGlobal(hGlobal, TRUE, &pStream);

			Image *imgLoginPhoto = Image::FromStream(pStream, FALSE);
			imgLoginPhoto->Save(wsLoginPhotoName.c_str(), &pngClsid, NULL);
			delete imgLoginPhoto;

			pStream->Release();
			GlobalFree(hGlobal);
			if (lpLoginPhoto)
			{
				delete lpLoginPhoto;
				lpLoginPhoto = NULL;
			}
		}
	}
	catch (...)
	{
		L_ERROR(_T("GetStudentInfo catch an error, return false\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

//根据考试原因编号，获取考试原因
wstring CVideoMergeManager::GetExamReason(wstring code)
{
	wstring wsRet = _T("");

	if (DB_VALUE_A == code)
	{
		wsRet = code + _T("-") + DB_VALUE_CK;    //A-初考
	}
	else if (DB_VALUE_B == code)
	{
		wsRet = code + _T("-") + DB_VALUE_ZJ;    //B-增驾
	}
	else if (DB_VALUE_F == code)
	{
		wsRet = code + _T("-") + DB_VALUE_MFXX;    //F-满分学习
	}
	else if (DB_VALUE_D == code)
	{
		wsRet = code + _T("-") + DB_VALUE_BK;    //D-补考
	}
	else
	{
		wsRet = DB_VALUE_KSYYWZ;    //考试原因:未知
	}

	return wsRet;
}