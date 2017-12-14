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

	/** @brief 获取设备能力集
	*	@param[in]	userId		登录id
	*	@param[out] struDecAbility	设备能力集结构体
	*	@return  返回操作结果
	*	 - false 操作失败
	*	 - true	操作成功
	*/
	static DLL_HIKUTIL_API bool WINAPI  GetDeviceAbility(int userId, NET_DVR_MATRIX_ABILITY_V41 &struDecAbility);

	/** @brief 设置设备定时重启
	*	@param[in]	userId		登录id
	*	@param[in]	byDate		星期几，1-7代表星期一到星期日
	*	@param[in]	byHour		时
	*	@param[in]	byMinute		分	
	*	@return  返回操作结果
	*	 - false 操作失败
	*	 - true	操作成功
	*/
	static DLL_HIKUTIL_API bool WINAPI  SetAutoReboot(int userId, int byDate, int byHour, int byMinute);
};


#endif