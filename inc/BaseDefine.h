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

#endif
