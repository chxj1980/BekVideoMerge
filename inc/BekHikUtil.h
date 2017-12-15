#pragma once

#ifndef __BEK_HIK_UTIL_H__
#define __BEK_HIK_UTIL_H__

#include "BaseDefine.h"

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

	/** @brief 开启解码通道
	*	@param[in]	userId		登录id
	*	@param[in]	byChanNo		解码通道号
	*	@return  返回操作结果
	*	 - false 操作失败
	*	 - true	操作成功
	*/
	static DLL_HIKUTIL_API bool WINAPI  SetDecChanEnable(int userId, int byChanNo);

	/** @brief 获取显示通道配置
	*	@param[in]	userId		登录id
	*	@param[in]	dispChan		显示通道号
	*	@param[in]	vOutCfg		显示通道配置
	*	@return  返回操作结果
	*	 - false 操作失败
	*	 - true	操作成功
	*/
	static DLL_HIKUTIL_API bool WINAPI  GetDisplayCfg(int userId, int dispChan, NET_DVR_MATRIX_VOUTCFG vOutCfg);

	/** @brief 配置显示通道
	*	@param[in]	userId		登录id
	*	@param[in]	dispChan		显示通道号
	*	@param[in]	vOutCfg		显示通道配置
	*	@return  返回操作结果
	*	 - false 操作失败
	*	 - true	操作成功
	*/
	static DLL_HIKUTIL_API bool WINAPI  SetDisplayCfg(int userId, int dispChan, NET_DVR_MATRIX_VOUTCFG vOutCfg);

	/** @brief 配置动态解码
	*	@param[in]	userId		登录id
	*	@param[in]	decChan		解码通道号
	*	@param[in]	videoChan	NVR/DVR通道信息
	*	@return  返回操作结果
	*	 - false 操作失败
	*	 - true	操作成功
	*/
	static DLL_HIKUTIL_API bool WINAPI  StartDynamicDecode(int userId, int decChan, CHANNEL_CONFIG videoChan);

	/** @brief 配置被动解码
	*	@param[in]	userId		登录id
	*	@param[in]	decChan		解码通道号
	*	@param[out]	lpHandle	被动解码操作句柄
	*	@return  返回操作结果
	*	 - false 操作失败
	*	 - true	操作成功
	*/
	static DLL_HIKUTIL_API bool WINAPI  StartPassiveDecode(int userId, int decChan, LONG &lpHandle);

	/** @brief 停止动态解码
	*	@param[in]	userId		登录id
	*	@param[in]	decChan		解码通道号
	*	@return  返回操作结果
	*	 - false 操作失败
	*	 - true	操作成功
	*/
	static DLL_HIKUTIL_API bool WINAPI  StopDynamicDecode(int userId, int decChan);
};


#endif