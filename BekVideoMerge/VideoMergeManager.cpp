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

	//��ʼ�����ݿ�����
	if (!InitDB())
	{
		return false;
	}

	//��ȡ��Ƶͨ��������Ϣ
	if (!InitVideoChannel())
	{
		return false;
	}

	//��ȡ��Ŀ�����Ϣ���۷ֱ����Ϣ
	if (!InitItems())
	{
		return false;
	}

	//��ʼ����������������ͨ������
	if (!InitDevice())
	{
		return false;
	}

	//��ʼ����
	if (!Run())
	{
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));

	return true;
}

//�����Թ����ź�
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
		int nCarNo = _wtoi(vecWstr[1].c_str());	//������
		if (!GetCarManager(nCarNo))
		{
			L_ERROR(_T("HandleExamSignal error, car%d not found\n"), nCarNo);
			return false;
		}

		wstring wsCertificateNo = vecWstr[6];	//׼��֤�����
		wstring wsTime = vecWstr[7];	//ʱ��

		//���¼�������ֻ���ض��ź����ͲŻᴫ��
		wstring wsXmNo = _T("");	//��Ŀ���
		wstring wsJudgeNo = _T("");	//�۷ֱ��
		int nScore = 0;		//���Ե÷�

		int nSignalType = _wtoi(vecWstr[2].c_str());	//���Թ����ź�����
		switch (nSignalType)
		{
		case SIGNAL_TYPE_17C51:	//���Կ�ʼ��û�������ֶ�Ҫ����
		{
			StudentInfo studentInfo;
			GetStudentInfo(wsCertificateNo, studentInfo);

			m_mapCarManagers[nCarNo].Handle17C51(studentInfo);
			
			break;
		}

		case SIGNAL_TYPE_17C52:	//��Ŀ��ʼ��ȡ��Ŀ���
		{
			wsXmNo = vecWstr[5];

			//��̬�л�����Ӧ����Ŀ��Ƶ
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
					m_mapCarManagers[nCarNo].StartDynamicDecode(channel, 1);
				}
			}



			break;
		}

		case SIGNAL_TYPE_17C53:	//��Ŀ�۷֣�ȡ��Ŀ��źͿ۷ֱ��
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

		case SIGNAL_TYPE_17C55:	//��Ŀ������ȡ��Ŀ���
		{
			wsXmNo = vecWstr[5];

			wstring key = _T("");
			if (KSKM_2 == m_nKskm)	
			{
				key = CAMERA_KM2_PUBLIC;	//��Ŀ����Ŀ����ʱ���л���Զ����Ƶ
			}
			else
			{
				//fix me
				//�ɵ�����������Ŀ����ǰ����ͷ�������Ϊ2������Ӧ��ͳһ����Ϊ3���ο�ö������CarCameraNoLocation
				key = CStringUtils::Format(_T("����%d_2"), nCarNo);	//��Ŀ����Ŀ����ʱ�л�Ϊ��ǰ����ͷ
			}

			CHANNEL_CONFIG channel;
			if (!GetVideoChannel(key, channel))
			{
				L_INFO(_T("Video channel %s not configured\n"), key.c_str());
			}
			else
			{
				m_mapCarManagers[nCarNo].StartDynamicDecode(channel, 1);
			}

			break;
		}

		case SIGNAL_TYPE_17C56:	//���Խ�����ȡ���Ե÷�
		{
			nScore = _wtoi(vecWstr[5].c_str());
			if (nScore < 0)
			{
				nCarNo = 0;
			}

			int nPassScore = 0;	//���Ժϸ����
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

//�������ź�
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

	//���Կ�Ŀ
	m_nKskm = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_KSKM, 2, wsEnvConfPath.c_str());
	L_INFO(_T("m_nKskm=%d\n"), m_nKskm);

	//���ݿ�����
	m_nDBType = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_SQLORACLE, 0, wsEnvConfPath.c_str());
	L_INFO(_T("m_nDBType=%d\n"), m_nDBType);

	//�����Ƿ��Ǵ�
	int nBigCar = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_BIGCAR, 0, wsEnvConfPath.c_str());
	if (1 == nBigCar)
	{
		m_bBigCar = true;
	}

	//������Ƿ�����̬�л�
	int nWnd2 = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_WND2, 0, wsDisplayConfPath.c_str());
	if (1 == nWnd2)
	{
		m_bDynamicVideo = true;
	}

	L_TRACE_LEAVE(_T("\n"));
}

//��ʼ�����ݿ�����
bool CVideoMergeManager::InitDB()
{
	L_TRACE_ENTER(_T("\n"));

	try
	{
		//�������ļ���ȡ���ݿ��������
		TCHAR buf[MAX_PATH];
		wstring wsDBConfPath = m_wsProgramPath + CONF_PATH_DB;
		wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
		wstring wsDBAddress = _T("");	//���ݿ��ַ
		wstring wsDBUsername = _T("");	//���ݿ��û���
		wstring wsDBPassword = _T("");		//���ݿ�����
		wstring wsDBInstance = _T("");		//���ݿ�ʵ��
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

		//�������ݿ����ͱ�д���ݿ������ַ���
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

		//��ʼ�����ݿ�����
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

//�����ݿ��ȡ��Ƶͨ������
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

//�����ݿ��ȡ��Ŀ�����Ϣ�Ϳ۷ֱ����Ϣ
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

//����������������ʼ��
bool CVideoMergeManager::InitDevice()
{
	L_TRACE_ENTER(_T("\n"));

	try
	{
		//����sdk��ʼ��
		if (!CBekHikUtil::InitSDK())
		{
			return false;
		}
		
		//�������ļ���ȡ�������
		wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
		wstring wsCarConfPath = m_wsProgramPath + CONF_PATH_CAR;
		wstring wsDisplayConfPath = m_wsProgramPath + CONF_PATH_DISPLAY;
		int nHKDeviceType = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_HMQ, 0, wsEnvConfPath.c_str());	//�����豸����
		int nNum = GetPrivateProfileInt(CONF_SECTION_JMQ, CONF_KEY_NUM, 0, wsCarConfPath.c_str());	//����������

		//ȡ�Ӵ��ںͽ���ͨ���Ķ�Ӧ��ϵ
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
				//�����豸IP���û��������롢�˿�
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

				//��¼�豸
				int nUserId = -1;
				if (!CBekHikUtil::LoginDevice(wsIP, wsUsername, wsPassword, nPort, nUserId))
				{
					L_ERROR(_T("LoginDevice failed, IP=%s\n"), wsIP.c_str());
					return false;
				}

				//��ȡ�豸������
				NET_DVR_MATRIX_ABILITY_V41 struDecAbility;
				memset(&struDecAbility, 0, sizeof(NET_DVR_MATRIX_ABILITY_V41));
				if (!CBekHikUtil::GetDeviceAbility(nUserId, struDecAbility))
				{
					L_ERROR(_T("GetDeviceAbility failed, IP=%s\n"), wsIP.c_str());
					return false;
				}

				//���ö�ʱ����
				CBekHikUtil::SetAutoReboot(nUserId, 7, 5, 9);

				//��ʼ����ʾͨ���ͽ���ͨ��
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

//������ͨ����⼰��ʼ��
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
		//�������ļ���ȡ�������
		wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
		wstring wsCarConfPath = m_wsProgramPath + CONF_PATH_CAR;
		wstring wsDisplayConfPath = m_wsProgramPath + CONF_PATH_DISPLAY;
		int nEven = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_EVEN, 0, wsDisplayConfPath.c_str());	//�Ƿ���н���

		wstring wsSection = CStringUtils::Format(_T("JMQ%d"), deviceNo);
		for (int nDviChanNo = 0; nDviChanNo < struDecAbility.struDviInfo.byChanNums; nDviChanNo++)	//DVIͨ��ѭ��
		{
			if ((DECODE_EVEN_YES == nEven) && (1 == nDviChanNo % 2))	//���н���
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

			//��������ͨ��
			BYTE byDecChan[DISPLAY_CHAN_NUMS];	//��ʾͨ����Ӧ�Ľ���ͨ��
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

			//������ʾͨ��
			NET_DVR_MATRIX_VOUTCFG vOutCfg;
			memset(&vOutCfg, 0, sizeof(NET_DVR_MATRIX_VOUTCFG));
			DWORD dwDispChan = struDecAbility.struDviInfo.byStartChan + nDviChanNo;	//��ʾͨ��
			if (!CBekHikUtil::GetDisplayCfg(userId, dwDispChan, vOutCfg))
			{
				return false;
			}
			if (DISPLAY_CHAN_NUMS != vOutCfg.dwWindowMode)	//����ָ�ģʽ
			{
				vOutCfg.dwWindowMode = DISPLAY_CHAN_NUMS;
			}
			for (int nDecChanNo = 0; nDecChanNo < DISPLAY_CHAN_NUMS; nDecChanNo++)	//�Ӵ��ڹ����Ľ���ͨ��
			{
				if (vOutCfg.byJoinDecChan[nDecChanNo] != byDecChan[m_displayChan[nDecChanNo]])
				{
					vOutCfg.byJoinDecChan[nDecChanNo] = byDecChan[m_displayChan[nDecChanNo]];
				}
			}
			vOutCfg.byAudio = 1;	//��Ƶ
			vOutCfg.byAudioWindowIdx = m_audioWindow;	//��Ƶ����
			vOutCfg.byVedioFormat = 1;	//��Ƶ��ʽ��1- NTSC��2- PAL				//�����������������ͬ
			vOutCfg.dwResolution = 67207228;													//�����������������ͬ
			vOutCfg.byScale = 0;	//��ʵ
			if (!CBekHikUtil::SetDisplayCfg(userId, dwDispChan, vOutCfg))
			{
				return false;
			}

			//������ʼ��
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
		L_ERROR(_T("InitDVIChannel catch an error\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

//������ͨ����⼰��ʼ��
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
		//�������ļ���ȡ�������
		wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
		wstring wsCarConfPath = m_wsProgramPath + CONF_PATH_CAR;
		wstring wsDisplayConfPath = m_wsProgramPath + CONF_PATH_DISPLAY;
		int nEven = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_EVEN, 0, wsDisplayConfPath.c_str());	//�Ƿ���н���

		wstring wsSection = CStringUtils::Format(_T("JMQ%d"), deviceNo);
		for (int nBncChanNo = 0; nBncChanNo < struDecAbility.struBncInfo.byChanNums; nBncChanNo++)	//BNCͨ��ѭ��
		{
			if ((DECODE_EVEN_YES == nEven) && (1 == nBncChanNo % 2))	//���н���
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

			//��������ͨ��
			BYTE byDecChan[DISPLAY_CHAN_NUMS];	//��ʾͨ����Ӧ�Ľ���ͨ��
			for (int j = 0; j < DISPLAY_CHAN_NUMS; j++)
			{
				byDecChan[j] = (BYTE)(struDecAbility.byStartChan + nBncChanNo * DISPLAY_CHAN_NUMS + j);
				if (!CBekHikUtil::SetDecChanEnable(userId, byDecChan[j]))
				{
					L_ERROR(_T("Enable %s of %s failed\n"), wsKey.c_str(), wsSection.c_str());
					return false;
				}
			}

			//������ʾͨ��
			NET_DVR_MATRIX_VOUTCFG vOutCfg;
			memset(&vOutCfg, 0, sizeof(NET_DVR_MATRIX_VOUTCFG));
			DWORD dwDispChan = struDecAbility.struBncInfo.byStartChan + nBncChanNo;	//��ʾͨ��
			if (!CBekHikUtil::GetDisplayCfg(userId, dwDispChan, vOutCfg))
			{
				return false;
			}
			if (DISPLAY_CHAN_NUMS != vOutCfg.dwWindowMode)	//����ָ�ģʽ
			{
				vOutCfg.dwWindowMode = DISPLAY_CHAN_NUMS;
			}
			for (int nDecChanNo = 0; nDecChanNo < DISPLAY_CHAN_NUMS; nDecChanNo++)	//�Ӵ��ڹ����Ľ���ͨ��
			{
				if (vOutCfg.byJoinDecChan[nDecChanNo] != byDecChan[m_displayChan[nDecChanNo]])
				{
					vOutCfg.byJoinDecChan[nDecChanNo] = byDecChan[m_displayChan[nDecChanNo]];
				}
			}
			vOutCfg.byAudio = 1;	//��Ƶ
			vOutCfg.byAudioWindowIdx = m_audioWindow;	//��Ƶ����
			vOutCfg.byVedioFormat = 2;	//��Ƶ��ʽ��1- NTSC��2- PAL			//�����������������ͬ
			vOutCfg.dwResolution = 0;																//�����������������ͬ
			vOutCfg.byScale = 0;	//��ʵ
			if (!CBekHikUtil::SetDisplayCfg(userId, dwDispChan, vOutCfg))
			{
				return false;
			}

			//������ʼ��
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
		//������Ƶ��̬����
		wstring key = CStringUtils::Format(_T("����%d_1"), it->first);
		if (!GetVideoChannel(key, channel))
		{
			L_INFO(_T("Video channel 1 of car %d not configured\n"), it->first);
		}
		else
		{
			it->second.StartDynamicDecode(channel, 0);
		}
		key = CStringUtils::Format(_T("����%d_2"), it->first);
		if (!GetVideoChannel(key, channel))
		{
			L_INFO(_T("Video channel 2 of car %d not configured\n"), it->first);
		}
		else
		{
			it->second.StartDynamicDecode(channel, 1);
		}
		
		//��������
		LONG lStudentInfoHandle = -1;
		LONG lMapHandle = -1;
		if (it->second.StartPassiveDecode(2, lStudentInfoHandle) && it->second.StartPassiveDecode(3, lMapHandle))
		{
			it->second.InitPassiveMode(lStudentInfoHandle, lMapHandle);
		}
	}

	return true;
}

//���ݱ�Ų�����Ƶͨ����Ϣ
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

//���ݿ����Ų��ҿ���������
bool CVideoMergeManager::GetCarManager(int carNo)
{
	if (0 == m_mapCarManagers.count(carNo))
	{
		return false;
	}
	return true;
}

//�����ݿ��ѯ������Ϣ
bool CVideoMergeManager::GetStudentInfo(wstring certificateNo, StudentInfo &studentInfo)
{
	L_TRACE_ENTER(_T("\n"));
	
	try
	{
		wstring wsSql = _T("");
		
		if (DB_ORACLE == m_nDBType)
		{
			wsSql = _T("select ������,SysCfg.��ע,���Գ���,����,�Ա�,to_char(sysdate,'yyyy-mm-dd') as MYDATE, \
					��ˮ��,���֤�����,��У����,����Ա1,����ԭ��,׼��֤�����,���մ��� from StudentInfo left join SchoolInfo \
					on ������=��У��� left join SysCfg on ������=��Ŀ where ׼��֤�����='") + certificateNo + _T("'");
		}
		else if (DB_SQL == m_nDBType)
		{
			wsSql = _T("select ������,SysCfg.��ע,���Գ���,����,�Ա�,(Select CONVERT(varchar(100), GETDATE(), 23)) as MYDATE, \
					��ˮ��,���֤�����,��У����,����Ա1,����ԭ��,׼��֤�����,���մ��� from StudentInfo left join SchoolInfo \
					on ������=��У��� left join SysCfg on ������=��Ŀ where ׼��֤�����='") + certificateNo + _T("'");
		}

		L_DEBUG(_T("GetStudentInfo sql : %s\n"), wsSql.c_str());

		VARIANT cnt;
		cnt.vt = VT_INT;
		_RecordsetPtr pSet = m_pDB->Execute((_bstr_t)wsSql.c_str(), &cnt, adCmdUnknown);
		_variant_t vat;
		if (pSet != NULL && !pSet->adoEOF)
		{
			while (!pSet->adoEOF)
			{
				vat = pSet->GetCollect(DB_FIELD_KCH);	//������
				if (vat.vt != NULL)
				{
					studentInfo.wsCarNo = (_bstr_t)vat;
				}
				
				vat = pSet->GetCollect(DB_FIELD_BZ);	//��ע�����ƺ�
				if (vat.vt != NULL)
				{
					studentInfo.wsRemarks = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_KSCX);	//���Գ���
				if (vat.vt != NULL)
				{
					studentInfo.wsCarType = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_XINGMING);	//����
				if (vat.vt != NULL)
				{
					studentInfo.wsName = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_XB);	//�Ա�
				if (vat.vt != NULL)
				{
					studentInfo.wsGender = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(_T("MYDATE"));	//����
				if (vat.vt != NULL)
				{
					studentInfo.wsDate = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_LSH);	//��ˮ��
				if (vat.vt != NULL)
				{
					studentInfo.wsSerialNo = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_SFZMBH);	//���֤�����
				if (vat.vt != NULL)
				{
					studentInfo.wsID = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_JXMC);	//��У����
				if (vat.vt != NULL)
				{
					studentInfo.wsDrivingSchool = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_KSY1);	//����Ա1
				if (vat.vt != NULL)
				{
					studentInfo.wsExaminer = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_DRCS);	//���մ���
				if (vat.vt != NULL)
				{
					studentInfo.wsDayCount = (_bstr_t)vat;
				}

				vat = pSet->GetCollect(DB_FIELD_KSYY);	//����ԭ��
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
	
	}
	catch (...)
	{
		L_ERROR(_T("GetStudentInfo catch an error, return false\n"));
		return false;
	}

	L_TRACE_LEAVE(_T("\n"));
	return true;
}

//���ݿ���ԭ���ţ���ȡ����ԭ��
wstring CVideoMergeManager::GetExamReason(wstring code)
{
	wstring wsRet = _T("");

	if (DB_VALUE_A == code)
	{
		wsRet = code + _T("-") + DB_VALUE_CK;    //A-����
	}
	else if (DB_VALUE_B == code)
	{
		wsRet = code + _T("-") + DB_VALUE_ZJ;    //B-����
	}
	else if (DB_VALUE_F == code)
	{
		wsRet = code + _T("-") + DB_VALUE_MFXX;    //F-����ѧϰ
	}
	else if (DB_VALUE_D == code)
	{
		wsRet = code + _T("-") + DB_VALUE_BK;    //D-����
	}
	else
	{
		wsRet = DB_VALUE_KSYYWZ;    //����ԭ��:δ֪
	}

	return wsRet;
}