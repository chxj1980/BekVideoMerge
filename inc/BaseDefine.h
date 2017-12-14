#pragma once

#ifndef __BASE_DEFINE_H__
#define  __BASE_DEFINE_H__

#define  LISTENING_ADDRESS	_T("0.0.0.0")
//TCP 监听端口
#define  LISTENING_PORT_TCP		6708
//UDP 监听端口
#define  LISTENING_PORT_UDP		6709

//配置文件
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

//数据库类型
typedef enum DatabaseType {
	DB_ORACLE = 0,
	DB_SQL = 1
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
	char szMideaIP[16];		//流媒体IP
	int  nStreamType;	//码流类型 0 主码流 1 子码流
}CHANNEL_CONFIG, *pCHANNEL_CONFIG;

//项目编号信息、扣分编号信息
typedef struct tagERROR_DATA
{
	char errorlx[26];
	int ikcfs;
}ERROR_DATA, *pERROR_DATA;

#endif
