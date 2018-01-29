#pragma once

#ifndef __BASE_DEFINE_H__
#define  __BASE_DEFINE_H__

#include <string>
using namespace std;

#include "gdiplus/inc/GdiPlus.h"
using namespace Gdiplus;

#define  LISTENING_ADDRESS	_T("0.0.0.0")
//TCP �����˿�
#define  LISTENING_PORT_TCP		6708
//UDP �����˿�
#define  LISTENING_PORT_UDP		6709
//����ǽ��Ƶ���ӷ��������TCP�˿�
#define LISTENING_PORT_TCP_WALL	6800

//����
#define FONT_MICROSOFT_YAHEI	_T("΢���ź�")

//���ó��Ļ�����������棬ֻ��ѡ��һ
//#define DISPLAY_4	
//#define DISPLAY_6	

//��ʾͨ�������Ľ���ͨ������
#define DISPLAY_CHAN_NUMS		5

//��������ʾͨ������
#define DISPLAY_CHAN_WIDTH	1920
#define DISPLAY_CHAN_HEIGHT	1920

#define VIDEO_WIDTH	640
#define VIDEO_HEIGHT	360

//��Ŀ��Զ������ͷ���
#define CAMERA_KM2_PUBLIC				_T("10086_1")

//���Ժϸ����
#define  EXAM_TOTAL_SCORE							100
#define  EXAM_PASS_SCORE_KM2					80
#define  EXAM_PASS_SCORE_KM2_BIGCAR		90
#define  EXAM_PASS_SCORE_KM3					90

//���Խ����󣬻����ӳ���ʾһ��ʱ��
#define  DISPLAY_DELAY_SECONDS					30

//�����ļ�
#define LOG_CONF_BEKVIDEOMERGE	_T("BekVideoMerge_logconf.ini")
#define LOG_CONF_BEKVIDEOWALL		_T("BekVideoWall_logconf.ini")
#define CONF_PATH_DB						_T("\\conf\\HS_CONF_DB.ini")
#define CONF_PATH_MAP						_T("\\conf\\HS_CONF_MAP.ini")
#define CONF_PATH_ENV						_T("\\conf\\HS_CONF_ENV.ini")
#define CONF_PATH_CAR 						_T("\\conf\\HS_CONF_CAR.ini")
#define CONF_PATH_CAR_WALL			_T("\\conf\\HS_CONF_CAR_WALL.ini")
#define CONF_PATH_DISPLAY				_T("\\conf\\HS_CONF_DISPLAY.ini")
#define CONF_SECTION_CONFIG			_T("CONFIG")
#define CONF_SECTION_JMQ				_T("JMQ")	
#define CONF_KEY_DBADDRESS			_T("DBADDRESS")
#define CONF_KEY_USERNAME				_T("USERNAME")
#define CONF_KEY_PASSWORD				_T("PASSWORD")
#define CONF_KEY_INSTANCE				_T("INSTANCE")
#define CONF_KEY_SQLORACLE			_T("SQLORACLE")
#define CONF_KEY_NUM						_T("NUM")
#define CONF_KEY_HMQ						_T("HMQ")
#define CONF_KEY_EVEN						_T("EVEN")
#define CONF_KEY_VIDEOWND				_T("VIDEOWND")
#define CONF_KEY_LOADMAP				_T("LOADMAP")
#define CONF_KEY_DRAWCAR				_T("DRAWCAR")
#define CONF_KEY_MAXX						_T("MAXX")
#define CONF_KEY_MINX						_T("MINX")
#define CONF_KEY_MAXY						_T("MAXY")
#define CONF_KEY_MINY						_T("MINY")
#define CONF_KEY_ZOOMIN					_T("ZOOMIN")
#define CONF_KEY_SPLITWIDTH			_T("SPLITWIDTH")
#define CONF_KEY_KSKM						_T("KSKM")
#define CONF_KEY_WND2						_T("WND2")
#define CONF_KEY_BIGCAR					_T("BIGCAR")

//�ļ�Ŀ¼
#define FILE_PATH_LOG		_T("\\log")
#define FILE_PATH_VIDEO		_T("\\video")
#define FILE_PATH_PHOTO		_T("\\photo")
#define FILE_PATH_ITEM		_T("\\res\\item\\")

//��Դ�ļ�
#define THIRDPARTY_PATH_MENCODER	_T("\\3rdparty\\mencoder.exe")
#define IMG_PATH_MAP_BACKGROUND		_T("\\res\\MapBackground.skin")
#define IMG_PATH_XM_BACKGROUND		_T("\\res\\XMBackground.skin")
#define IMG_PATH_STUDENT_BACKGROUND		_T("\\res\\StuBackground.png")
#define IMG_PATH_XM_LIST					_T("\\res\\XMList.skin")
#define IMG_PATH_CAR_SKIN		_T("\\res\\Car.skin")
#define IMG_PATH_SIGNAL		_T("\\res\\signal\\signal.png")
#define MAP_FILENAME_FORMAT		_T("\\res\\map\\%d_%d.png")
#define PHOTO_ID_FORMAT		_T("\\%d_id_photo.png")
#define PHOTO_LOGIN_FORMAT		_T("\\%d_login_photo.png")
#define IMG_PATH_GEAR_FORMAT		_T("\\res\\signal\\Gear_%d.png")

//��Ŀ��д
//�ϳ�׼��
#define ITEM_SCZB		_T("SCZB")
//��
#define ITEM_QB			_T("QB")
//ֱ����ʻ
#define ITEM_ZXXS		_T("ZXXS")
//�Ӽ���
#define ITEM_JJD			_T("JJD")
//�������
#define ITEM_BGCD		_T("BGCD")
//����ͣ��
#define ITEM_KBTC		_T("KBTC")
//ֱ��ͨ��·��
#define ITEM_ZX				_T("ZX")
//��ת
#define ITEM_ZZ				_T("ZZ")
//��ת
#define ITEM_YZ				_T("YZ")
//���к��
#define ITEM_RXHD		_T("RXHD")
//ѧУ����
#define ITEM_XXQY		_T("XXQY")
//������վ
#define ITEM_GJCZ			_T("GJCZ")
//�ᳵ
#define ITEM_HC			_T("HC")
//����
#define ITEM_CC			_T("CC")
//��ͷ
#define ITEM_DT			_T("DT")
//ҹ��
#define ITEM_YJ				_T("YJ")

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
#define WM_SOCKET_TCP_WALL WM_USER + 1003

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

//��������
typedef enum PassiveWindowType {
	WINDOW_TYPE_MAP = 0,
	WINDOW_TYPE_STUINFO = 1
};

//���ݿ�����
typedef enum DatabaseType {
	DB_ORACLE = 0,
	DB_SQL = 1
};

//���Կ�Ŀ
typedef enum KSKMType {
	KSKM_2 = 2,
	KSKM_3 = 3
};

//�����豸����
typedef enum HKDeviceType {
	DEVICE_TYPE_JMQ = 0,	//������
	DEVICE_TYPE_HMQ = 1	//������
};

//�Ƿ���н���
typedef enum HKDecodeEven {
	DECODE_EVEN_NO = 0,	//���н���
	DECODE_EVEN_YES = 1	//���н���
};

//��������ͷ���
typedef enum CarCameraNoLocation {
	CAR_CAMERA_NO_MAIN_DRIVING = 1,	//��������ͷ
	CAR_CAMERA_NO_COPILOT = 2,	//��������ͷ
	CAR_CAMERA_NO_CAR_FRONT = 3,	//��ǰ����ͷ
	CAR_CAMERA_NO_MERGE = 10,	//�ϳɺ����Ƶ
};

//��Ŀ����ͷ���
typedef enum XmCameraNoLocation {
	XM_CAMERA_NO_1 = 1,		//��Ŀ1������ͷ
};

//���Թ����ź�����
typedef enum ExamSignalType {
	SIGNAL_TYPE_UNKNOWN = 0,		//�����ź�
	SIGNAL_TYPE_17C51 = 1,	//���Կ�ʼ
	SIGNAL_TYPE_17C52 = 2,	//��Ŀ��ʼ
	SIGNAL_TYPE_17C53 = 3,	//��Ŀ�۷�
	SIGNAL_TYPE_17C55 = 5,	//��Ŀ����
	SIGNAL_TYPE_17C56 = 6,	//���Խ��� 
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
	char szMediaIP[16];		//��ý��IP
	int  nStreamType;	//�������� 0 ������ 1 ������
}CHANNEL_CONFIG, *pCHANNEL_CONFIG;

//��Ŀ�����Ϣ���۷ֱ����Ϣ
typedef struct tagERROR_DATA
{
	char errorlx[26];
	int ikcfs;
}ERROR_DATA, *pERROR_DATA;

//�����źţ�����GPS�źź�ԭ���źţ�
typedef struct tagCarSignal
{
	double dX;		//������
	double dY;		//������
	float fDirectionAngle;		//�����
	float fSpeed;	//�ٶ�
	float fMileage;		//���

	int aqd;   //��ȫ��
	int js;	//��ɲ
	int ss;	//��ɲ
	int fs;	//��ɲ
	int lh;	//���
	int lb;	//����
	int zzx;	//��ת��
	int yzx;	//��ת��
	int ygd;	//Զ���
	int jgd;		//�����
	int jsd;		//��ʾ��
	int xh;		//Ϩ��
	int kgm;	//������
	int dw;		//��λ
	int yg;		//���
	int wd;		//���
	int skd;	//ʾ����
	int dh;		//���
	float   fSpeedCar;	//ԭ���ٶ�
	float   fSpeedEngine;	//ת��

	tagCarSignal()
	{
		dX = 0.0;
		dY = 0.0;
		fDirectionAngle = 0.0;
		fSpeed = 0.0;
		fMileage = 0.0;

		aqd = 0;
		js = 0;
		ss = 0;
		fs = 0;
		lh = 0;
		lb = 0;
		zzx = 0;
		yzx = 0;
		ygd = 0;
		jgd = 0;
		jsd = 0;
		xh = 0;
		kgm = 0;
		dw = 0;
		yg = 0;
		wd = 0;
		skd = 0;
		dh = 0;
		fSpeedCar = 0.0;
		fSpeedEngine = 0.0;
	}

}CarSignal, *pCarSignal;

//fix me
//��Ҫ������Ƭ��Ϣ
//������Ϣ 
typedef struct tagStudentInfo
{
	wstring wsCarNo;    //������
	wstring wsRemarks;     //��ע�����ƺţ�
	wstring wsCarType;		//���Գ���
	wstring wsName;		//����
	wstring wsGender;		//�Ա�
	wstring wsDate;		//����
	wstring wsSerialNo;		//��ˮ��
	wstring wsID;		//���֤�����
	wstring wsDrivingSchool;		//��У����
	wstring wsExaminer;   //����Ա1
	wstring wsExamReason;   //����ԭ��
	wstring wsDayCount;    //���մ���
	wstring wsCertificateNo;		//׼��֤�����

	tagStudentInfo()
	{
		wsCarNo = _T("");
		wsRemarks = _T("");
		wsCarType = _T("");
		wsName = _T("");
		wsGender = _T("");
		wsDate = _T("");
		wsSerialNo = _T("");
		wsID = _T("");
		wsDrivingSchool = _T("");
		wsExaminer = _T("");
		wsExamReason = _T("");
		wsDayCount = _T("");
		wsCertificateNo = _T("");
	}
}StudentInfo, *pStudentInfo;

#endif
