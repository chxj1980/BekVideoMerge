#pragma once

/**
* @file ServiceUtils.h
* @brief 服务接口类头文件
*/

#ifndef __SERVICE_UTILS_H__
#define __SERVICE_UTILS_H__

#include "DllExport.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(1)

typedef BOOL (WINAPI *IServiceFunc)();

/**
 * @brief 服务基本信息
 */
typedef struct
{
    BOOL         ConsoloMode;    /** < 控制台模式      */
    WCHAR*       ServiceName;    /** < 服务名称        */
    WCHAR*       LogConfigPath;  /** < Log配置文件路径 */
    WCHAR*       LogModuleName;  /** < Log的模块名称   */
    IServiceFunc StartFun;       /** < 服务启动函数    */
    IServiceFunc StopFun;        /** < 服务启动函数    */
} ServiceBaseInfo;

#pragma pack()

/** @brief Windows 服务框架函数
*	@param[in]	ServiceBaseInfo		服务基本信息
*	@par 示例
*	@code
*		ServiceBaseInfo info;
*		info.ConsoloMode   = FALSE;
*		info.LogConfigPath = _T("CommonTest_logconf.ini");
*		info.LogModuleName = _T("CommonTestService");
*		info.ServiceName   = _T("CommonTestService");
*		info.StartFun = CommonTestServiceStart;
*		info.StopFun       = CommonTestServiceStop;
*		ServiceMain(&info);
*	 @endcode
*/
DLL_COMMONLIB_API int WINAPI ServiceMain(ServiceBaseInfo *ServiceInfo);

/** @brief 获取服务状态
*	@param[in]	serviceName	服务名称
*	@param[out] dwState 服务状态
*	@dwState 返回值列表
*		SERVICE_STOPPED
*		SERVICE_START_PENDING
*		SERVICE_STOP_PENDING
*		SERVICE_RUNNING
*		SERVICE_CONTINUE_PENDING
*		SERVICE_PAUSE_PENDING
*		SERVICE_PAUSED
*	@return  返回操作结果
*	 - false 失败
*	 - true 成功
*/
DLL_COMMONLIB_API BOOL WINAPI GetServiceState(const WCHAR* serviceName, DWORD* dwState);

/** @brief 启动服务
*	@param[in]	serviceName	服务名称
*	@return  返回操作结果
*	 - false 失败
*	 - true 成功
*/
DLL_COMMONLIB_API BOOL WINAPI ServiceStart(const WCHAR* serviceName);

/** @brief 停止服务
*	@param[in]	serviceName	服务名称
*	@return  返回操作结果
*	 - false 失败
*	 - true 成功
*/
DLL_COMMONLIB_API BOOL WINAPI ServiceStop(const WCHAR* serviceName);

/** @brief 安装服务，服务类型为自动运行
*	@param[in]	binPath 服务主程序完整路径
*	@param[in]	serviceName 服务名称
*	@param[in]	description 服务描述
*	@return  返回操作结果
*	 - false 失败
*	 - true 成功
*/
DLL_COMMONLIB_API BOOL WINAPI ServiceInstall(const WCHAR* binPath, const WCHAR* serviceName, WCHAR* description);

/** @brief 卸载服务
*	@param[in]	serviceName 服务名称
*	@return  返回操作结果
*	 - false 失败
*	 - true 成功
*/
DLL_COMMONLIB_API BOOL WINAPI ServiceUninstall(const WCHAR* serviceName);

#ifdef __cplusplus
}
#endif

#endif
