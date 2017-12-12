#pragma once

#ifndef __SOCKET_UTILS_H__
#define __SOCKET_UTILS_H__

#include <WinSock2.h>
#include "DllExport.h"

/**
* @class CSocketUtils
* @brief Socket操作静态库
*/

/**
* @brief 包结构
*/
typedef struct
{
	/**< 包大小，包括包头 */
	DWORD len;
	/**< 包类型，见下面 RP_XXX_XXX的定义 */
	DWORD  type;
	/**< 数据 */
	BYTE   data[1];
}PACKET_T, *PPACKET_T;
#define GET_PACKET_LEN(pkg)         ((pkg)->len)
#define PACKET_HEADER_LEN           (sizeof(PACKET_T) - 1)
#define GET_PACKET_DATA(pkg)        ((pkg)->data)
#define GET_PACKET_DATA_LEN(pkg)    ((pkg)->len - PACKET_HEADER_LEN)
#define GET_PACKET_TYPE(pkg)        ((pkg)->type)

class CSocketUtils
{
public:
	/**
	* @brief 创建监听socket
	*
	* @param [in] addr    - 监听地址可为NULL
	* @param [in] port    - 监听端口
	* @param [in] backlog - 同时最大连接数
	*
	* @return 返回创建的监听socket对象，失败返回INVALID_SOCKET
	*/
	static DLL_COMMONLIB_API SOCKET WINAPI Socket_CreateListenSock(const TCHAR * /* addr */, USHORT port, int backlog);

	/**
	* @brief 创建连接socket
	*
	* @param [in] addr    - 连接地址
	* @param [in] port    - 连接端口
	*
	* @return 返回创建的socket对象，失败返回INVALID_SOCKET
	*/
	static DLL_COMMONLIB_API SOCKET WINAPI Socket_Connect(const TCHAR *addr, USHORT port);

	/**
	* @brief 读取多个字节
	*/
	static DLL_COMMONLIB_API BOOL WINAPI Socket_ReadNBytes(
		SOCKET so, BYTE *buf, DWORD size, DWORD timeout, HANDLE stopEvent);

	/**
	* @brief 写多个字节
	*/
	static DLL_COMMONLIB_API BOOL WINAPI Socket_WriteNBytes(
		SOCKET so, BYTE *buf, DWORD size, DWORD timeout, HANDLE stopEvent);

	/**
	* @brief 读取一个数据包
	*/
	static DLL_COMMONLIB_API PACKET_T * WINAPI Socket_RecvAPacket(SOCKET so, DWORD timeout, HANDLE stopEvent);

	/**
	* @brief 写一个数据包
	*/
	static DLL_COMMONLIB_API BOOL WINAPI Socket_SendAPacket(SOCKET so, void *data, int datalen,
		DWORD type, DWORD timeout, HANDLE stopEvent);
};


#endif