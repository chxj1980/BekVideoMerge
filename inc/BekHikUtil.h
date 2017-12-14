#pragma once

#ifndef __BEK_HIK_UTIL_H__
#define __BEK_HIK_UTIL_H__



#ifndef DLL_HIKUTIL_API
#define DLL_HIKUTIL_API __declspec(dllexport)
#endif


/**
* @class CBekHikUtil
* @brief 海康设备操作封装类
*
* 用于操作海康设备（合码器、解码器、NVR、DVR 等）
*/

class CBekHikUtil
{
public:
	
	/** @brief 海康 sdk 初始化
	*	@return  返回操作结果
	*	 - false 初始化失败
	*	 - true	初始化成功
	*/
	static DLL_HIKUTIL_API bool WINAPI InitSDK();

	/** @brief 登录海康设备
	*	@param[in]	ip		设备IP
	*	@param[in]	username 	用户名
	*	@param[in]	password 	密码
	*	@param[in]	port	 	端口
	*	@param[out] userId	输出登录id
	*	@return  返回操作结果
	*	 - false 登录失败
	*	 - true	登录成功
	*/
	static DLL_HIKUTIL_API bool WINAPI LoginDevice(wstring ip, wstring username, wstring password, int port, int &userId);
};


#endif