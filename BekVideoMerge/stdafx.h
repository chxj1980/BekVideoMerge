
// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // Exclude rarely-used stuff from Windows headers
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions


#include <afxdisp.h>        // MFC Automation classes



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC support for Internet Explorer 4 Common Controls
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // MFC support for ribbons and control bars



#include <process.h>
using namespace std;

//如果在没有定义WIN32_LEAN_AND_MEAN宏的大前提下windows.h有可能包含winsock.h 头文件
#define WIN32_LEAN_AND_MEAN

#include "BaseDefine.h"
#include "Function.h"

//CommonDll
#include "Common/ServiceUtils.h"
#include "Common/LogBase.h"
#include "Common/SocketUtils.h"
#include "Common/WinUtils.h"
#include "Common/StringUtils.h"
#include "Common/Base64Utils.h"
#include "Common/ThreadUtils.h"
#pragma comment(lib, "CommonDll.lib")

//Hikvision
#include "hikvision/inc/HCNetSDK.h"
#include "BekHikUtil.h"
#pragma comment(lib, "BekHikUtil.lib")
#pragma comment(lib, "hikvision/lib/HCNetSDK.lib")

//gdiplus
#include "gdiplus/inc/GdiPlus.h"
#pragma comment(lib, "gdiplus/lib/GdiPlus.lib")
using namespace Gdiplus;

//Socket
#include "JmqUDP.h"
#include "TCPSockz.h"
#pragma comment(lib, "CUdpForJMQ.lib")

//ADO
#pragma warning(disable: 4146)
#pragma warning(disable: 4244)
#import "ado/msado15.dll" no_namespace rename("EOF", "adoEOF")

//临时保存为PNG，不发送到合码器
//#define __DEBUG_SAVE_PNG__

#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


