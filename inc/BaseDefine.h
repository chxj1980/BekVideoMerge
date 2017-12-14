#pragma once

#ifndef __BASE_DEFINE_H__
#define  __BASE_DEFINE_H__

#define  LISTENING_ADDRESS	_T("0.0.0.0")
//TCP �����˿�
#define  LISTENING_PORT_TCP		6708
//UDP �����˿�
#define  LISTENING_PORT_UDP		6709

//�����ļ�
#define LOG_CONF_BEKVIDEOMERGE	_T("BekVideoMerge_logconf.ini")
#define CONF_PATH_DB						_T("\\conf\\HS_CONF_DB.ini")
#define CONF_PATH_MAP						_T("\\conf\\HS_CONF_MAP.ini")
#define CONF_PATH_ENV						_T("\\conf\\HS_CONF_ENV.ini")
#define CONF_PATH_CAR 						_T("\\conf\\HS_CONF_CAR.ini")
#define CONF_PATH_DISPLAY				_T("\\conf\\HS_CONF_DISPLAY.ini")
#define CONF_SECTION_CONFIG			_T("CONFIG")
#define CONF_KEY_DBADDRESS			_T("DBADDRESS")
#define CONF_KEY_USERNAME				_T("USERNAME")
#define CONF_KEY_PASSWORD				_T("PASSWORD")
#define CONF_KEY_INSTANCE				_T("INSTANCE")
#define CONF_KEY_SQLORACLE			_T("SQLORACLE")

//���ݿ�
#define  DB_TABLE_TBKVIDEO				_T("TBKVideo")
#define  DB_TABLE_ERRORDATA			_T("ErrorData")
#define  DB_TABLE_STUDENTINFO		_T("StudentInfo")
#define  DB_TABLE_STUDENTPHOTO	_T("StudentPhoto")
#define  DB_TABLE_SCHOOLINFO			_T("SchoolInfo")
#define  DB_TABLE_SYSCFG					_T("SysCfg")
#define  DB_FIELD_BH							_T("���")
#define  DB_FIELD_SBIP							_T("�豸IP")
#define  DB_FIELD_YHM							_T("�û���")
#define  DB_FIELD_MM							_T("����")
#define  DB_FIELD_DKH							_T("�˿ں�")
#define  DB_FIELD_TDH							_T("ͨ����")
#define  DB_FIELD_TRANSMODE			_T("TransMode")
#define  DB_FIELD_MEDIAIP					_T("MediaIP")
#define  DB_FIELD_NID							_T("Nid")
#define  DB_FIELD_CWBH						_T("������")
#define  DB_FIELD_KFLX							_T("�۷�����")
#define  DB_FIELD_KCFS							_T("�۳�����")
#define  DB_FIELD_KSCS						_T("���Դ���")
#define  DB_FIELD_DRCS						_T("���մ���")
#define  DB_FIELD_ZKZMBH					_T("׼��֤�����")
#define  DB_FIELD_MJZP						_T("��Ƭ")
#define  DB_FIELD_KCH							_T("������")
#define  DB_FIELD_BZ							_T("��ע")
#define  DB_FIELD_KSCX						_T("���Գ���")
#define  DB_FIELD_XINGMING				_T("����")
#define  DB_FIELD_XB							_T("�Ա�")
#define  DB_FIELD_LSH							_T("��ˮ��")
#define  DB_FIELD_SFZMBH					_T("���֤�����")
#define  DB_FIELD_JXMC						_T("��У����")
#define  DB_FIELD_KSY1							_T("����Ա1")
#define  DB_FIELD_KSYY							_T("����ԭ��")
#define  DB_FIELD_DLR							_T("������")
#define  DB_FIELD_JXBH						_T("��У���")
#define  DB_FIELD_XIANGMU				_T("��Ŀ")
#define  DB_VALUE_A							_T("A")
#define  DB_VALUE_B								_T("B")
#define  DB_VALUE_D							_T("D")
#define  DB_VALUE_F								_T("F")
#define  DB_VALUE_CK							_T("����")
#define  DB_VALUE_ZJ							_T("����")
#define  DB_VALUE_MFXX						_T("����ѧϰ")
#define  DB_VALUE_BK							_T("����")
#define  DB_VALUE_KSYYWZ					_T("����ԭ��δ֪")


#define WM_SOCKET_TCP	   WM_USER + 1001
#define WM_SOCKET_UDP	   WM_USER + 1002

typedef enum PACKTYPE {
	SOCKZREEOR,
	GNSSDATA,
	JGPTDATA,
	JMQStart,
	QueryStart,
	AllCarNum,
	M17C51,
	M17C52,
	M17C53,
	M17C54,
	M17C55,
	M17C56
};

//���ݿ�����
typedef enum DatabaseType {
	DB_ORACLE = 0,
	DB_SQL = 1
};

//ͨ������
typedef struct tagCHANNEL_CONFIG
{
	char szDeviceNo[10];	//���
	char szDeviceIP[16];	//�豸IP
	char szUsername[16];		//�û���
	char szPassword[16];	//����
	WORD dwPort;		//�˿ں�
	WORD dwChannel;		//ͨ����
	char szMideaIP[16];		//��ý��IP
	int  nStreamType;	//�������� 0 ������ 1 ������
}CHANNEL_CONFIG, *pCHANNEL_CONFIG;

//��Ŀ�����Ϣ���۷ֱ����Ϣ
typedef struct tagERROR_DATA
{
	char errorlx[26];
	int ikcfs;
}ERROR_DATA, *pERROR_DATA;

#endif
