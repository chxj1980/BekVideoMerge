#pragma once

#ifndef __BASE_DEFINE_H__
#define  __BASE_DEFINE_H__

#include <string>
using namespace std;

#include "gdiplus/inc/GdiPlus.h"
using namespace Gdiplus;

#define  LISTENING_ADDRESS	_T("0.0.0.0")
//TCP 监听端口
#define  LISTENING_PORT_TCP		6708
//UDP 监听端口
#define  LISTENING_PORT_UDP		6709
//电视墙视频叠加服务监听的TCP端口
#define LISTENING_PORT_TCP_WALL	6800

//字体
#define FONT_MICROSOFT_YAHEI	_T("微软雅黑")

//配置成四画面或者六画面，只能选其一
//#define DISPLAY_4	
//#define DISPLAY_6	

//显示通道关联的解码通道个数
#define DISPLAY_CHAN_NUMS		5

//合码器显示通道长宽
#define DISPLAY_CHAN_WIDTH	1920
#define DISPLAY_CHAN_HEIGHT	1920

#define VIDEO_WIDTH	640
#define VIDEO_HEIGHT	360

//科目二远景摄像头编号
#define CAMERA_KM2_PUBLIC				_T("10086_1")

//考试合格分数
#define  EXAM_TOTAL_SCORE							100
#define  EXAM_PASS_SCORE_KM2					80
#define  EXAM_PASS_SCORE_KM2_BIGCAR		90
#define  EXAM_PASS_SCORE_KM3					90

//考试结束后，画面延迟显示一段时间
#define  DISPLAY_DELAY_SECONDS					30

//配置文件
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

//文件目录
#define FILE_PATH_LOG		_T("\\log")
#define FILE_PATH_VIDEO		_T("\\video")
#define FILE_PATH_PHOTO		_T("\\photo")
#define FILE_PATH_ITEM		_T("\\res\\item\\")

//资源文件
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

//项目简写
//上车准备
#define ITEM_SCZB		_T("SCZB")
//起步
#define ITEM_QB			_T("QB")
//直线行驶
#define ITEM_ZXXS		_T("ZXXS")
//加减档
#define ITEM_JJD			_T("JJD")
//变更车道
#define ITEM_BGCD		_T("BGCD")
//靠边停车
#define ITEM_KBTC		_T("KBTC")
//直行通过路口
#define ITEM_ZX				_T("ZX")
//左转
#define ITEM_ZZ				_T("ZZ")
//右转
#define ITEM_YZ				_T("YZ")
//人行横道
#define ITEM_RXHD		_T("RXHD")
//学校区域
#define ITEM_XXQY		_T("XXQY")
//公交车站
#define ITEM_GJCZ			_T("GJCZ")
//会车
#define ITEM_HC			_T("HC")
//超车
#define ITEM_CC			_T("CC")
//掉头
#define ITEM_DT			_T("DT")
//夜间
#define ITEM_YJ				_T("YJ")

//数据库
#define  DB_TABLE_TBKVIDEO				_T("TBKVideo")
#define  DB_TABLE_ERRORDATA			_T("ErrorData")
#define  DB_TABLE_STUDENTINFO		_T("StudentInfo")
#define  DB_TABLE_STUDENTPHOTO	_T("StudentPhoto")
#define  DB_TABLE_SCHOOLINFO			_T("SchoolInfo")
#define  DB_TABLE_SYSCFG					_T("SysCfg")
#define  DB_FIELD_BH							_T("编号")
#define  DB_FIELD_SBIP							_T("设备IP")
#define  DB_FIELD_YHM							_T("用户名")
#define  DB_FIELD_MM							_T("密码")
#define  DB_FIELD_DKH							_T("端口号")
#define  DB_FIELD_TDH							_T("通道号")
#define  DB_FIELD_TRANSMODE			_T("TransMode")
#define  DB_FIELD_MEDIAIP					_T("MediaIP")
#define  DB_FIELD_NID							_T("Nid")
#define  DB_FIELD_CWBH						_T("错误编号")
#define  DB_FIELD_KFLX							_T("扣分类型")
#define  DB_FIELD_KCFS							_T("扣除分数")
#define  DB_FIELD_KSCS						_T("考试次数")
#define  DB_FIELD_DRCS						_T("当日次数")
#define  DB_FIELD_ZKZMBH					_T("准考证明编号")
#define  DB_FIELD_MJZP						_T("照片")
#define  DB_FIELD_KCH							_T("考车号")
#define  DB_FIELD_BZ							_T("备注")
#define  DB_FIELD_KSCX						_T("考试车型")
#define  DB_FIELD_XINGMING				_T("姓名")
#define  DB_FIELD_XB							_T("性别")
#define  DB_FIELD_LSH							_T("流水号")
#define  DB_FIELD_SFZMBH					_T("身份证明编号")
#define  DB_FIELD_JXMC						_T("驾校名称")
#define  DB_FIELD_KSY1							_T("考试员1")
#define  DB_FIELD_KSYY							_T("考试原因")
#define  DB_FIELD_DLR							_T("代理人")
#define  DB_FIELD_JXBH						_T("驾校编号")
#define  DB_FIELD_XIANGMU				_T("项目")
#define  DB_VALUE_A							_T("A")
#define  DB_VALUE_B								_T("B")
#define  DB_VALUE_D							_T("D")
#define  DB_VALUE_F								_T("F")
#define  DB_VALUE_CK							_T("初考")
#define  DB_VALUE_ZJ							_T("增驾")
#define  DB_VALUE_MFXX						_T("满分学习")
#define  DB_VALUE_BK							_T("补考")
#define  DB_VALUE_KSYYWZ					_T("考试原因：未知")



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

//画面类型
typedef enum PassiveWindowType {
	WINDOW_TYPE_MAP = 0,
	WINDOW_TYPE_STUINFO = 1
};

//数据库类型
typedef enum DatabaseType {
	DB_ORACLE = 0,
	DB_SQL = 1
};

//考试科目
typedef enum KSKMType {
	KSKM_2 = 2,
	KSKM_3 = 3
};

//解码设备类型
typedef enum HKDeviceType {
	DEVICE_TYPE_JMQ = 0,	//解码器
	DEVICE_TYPE_HMQ = 1	//合码器
};

//是否隔行解码
typedef enum HKDecodeEven {
	DECODE_EVEN_NO = 0,	//逐行解码
	DECODE_EVEN_YES = 1	//隔行解码
};

//考车摄像头编号
typedef enum CarCameraNoLocation {
	CAR_CAMERA_NO_MAIN_DRIVING = 1,	//主驾摄像头
	CAR_CAMERA_NO_COPILOT = 2,	//副驾摄像头
	CAR_CAMERA_NO_CAR_FRONT = 3,	//车前摄像头
	CAR_CAMERA_NO_MERGE = 10,	//合成后的视频
};

//项目摄像头编号
typedef enum XmCameraNoLocation {
	XM_CAMERA_NO_1 = 1,		//项目1号摄像头
};

//考试过程信号类型
typedef enum ExamSignalType {
	SIGNAL_TYPE_UNKNOWN = 0,		//错误信号
	SIGNAL_TYPE_17C51 = 1,	//考试开始
	SIGNAL_TYPE_17C52 = 2,	//项目开始
	SIGNAL_TYPE_17C53 = 3,	//项目扣分
	SIGNAL_TYPE_17C55 = 5,	//项目结束
	SIGNAL_TYPE_17C56 = 6,	//考试结束 
};

//通道配置
typedef struct tagCHANNEL_CONFIG
{
	char szDeviceNo[10];	//编号
	char szDeviceIP[16];	//设备IP
	char szUsername[16];		//用户名
	char szPassword[16];	//密码
	WORD dwPort;		//端口号
	WORD dwChannel;		//通道号
	char szMediaIP[16];		//流媒体IP
	int  nStreamType;	//码流类型 0 主码流 1 子码流
}CHANNEL_CONFIG, *pCHANNEL_CONFIG;

//项目编号信息、扣分编号信息
typedef struct tagERROR_DATA
{
	char errorlx[26];
	int ikcfs;
}ERROR_DATA, *pERROR_DATA;

//车载信号（包含GPS信号和原车信号）
typedef struct tagCarSignal
{
	double dX;		//横坐标
	double dY;		//纵坐标
	float fDirectionAngle;		//方向角
	float fSpeed;	//速度
	float fMileage;		//里程

	int aqd;   //安全带
	int js;	//脚刹
	int ss;	//手刹
	int fs;	//副刹
	int lh;	//离合
	int lb;	//喇叭
	int zzx;	//左转向
	int yzx;	//右转向
	int ygd;	//远光灯
	int jgd;		//近光灯
	int jsd;		//警示灯
	int xh;		//熄火
	int kgm;	//开关门
	int dw;		//档位
	int yg;		//雨刮
	int wd;		//雾灯
	int skd;	//示廓灯
	int dh;		//点火
	float   fSpeedCar;	//原车速度
	float   fSpeedEngine;	//转速

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
//需要增加照片信息
//考生信息 
typedef struct tagStudentInfo
{
	wstring wsCarNo;    //考车号
	wstring wsRemarks;     //备注（车牌号）
	wstring wsCarType;		//考试车型
	wstring wsName;		//姓名
	wstring wsGender;		//性别
	wstring wsDate;		//日期
	wstring wsSerialNo;		//流水号
	wstring wsID;		//身份证明编号
	wstring wsDrivingSchool;		//驾校名称
	wstring wsExaminer;   //考试员1
	wstring wsExamReason;   //考试原因
	wstring wsDayCount;    //当日次数
	wstring wsCertificateNo;		//准考证明编号

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
