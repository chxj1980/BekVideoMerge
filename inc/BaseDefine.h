#pragma once

#ifndef __BASE_DEFINE_H__
#define  __BASE_DEFINE_H__

#define  LISTENING_ADDRESS	_T("0.0.0.0")
//TCP 监听端口
#define  LISTENING_PORT_TCP		6708
//UDP 监听端口
#define  LISTENING_PORT_UDP		6709

//日志配置文件
#define LOG_CONF_BEKVIDEOMERGE	_T("BekVideoMerge_logconf.ini")


#define WM_SOCKET_TCP	   WM_USER + 1001
#define WM_SOCKET_UDP	   WM_USER + 1002

#endif
