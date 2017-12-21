#pragma once

/**
* @file ThreadUtils.h
* @brief 线程接口类头文件
*/

#ifndef __THREAD_UTILS_H__
#define __THREAD_UTILS_H__

#include "DllExport.h"

/**
 * @brief 注册主线程处理函数使用的结构
 * @param[in] param为自定义参数
 * @param[in] stopevent为线程退出通告，主线程处理函数遇到此消息需要立刻退出
 * @return 返回TRUE表示可以继续下一次执行
 *         返回FALSE表示线程需要结束
 */
typedef BOOL (* ThreadMainProc)(LPVOID param, HANDLE stopevent);

/**
 * @brief 注册主线程停止时处理函数使用的结构
 * @param[in] param为自定义参数
 */
typedef void (* ThreadEndProc)(LPVOID param);

/** 
 * @class IThread
 * @brief 线程接口类
 *
 * 用于创建一个可控的线程对象，可以同步启动与停止线程
 * 在线程处理函数被动退出时也会调用注册的函数进行通知
 */
class DLL_COMMONLIB_API IThread
{
public:
    /**
     * @brief 启动主线程
     * 
     * 返回后保证主线程开始运行
     */
    virtual void WINAPI StartMainThread() = 0;

    /**
     * @brief 结束主线程
     * 
     * 返回后保证主线程已经停止运行
     */
    virtual void WINAPI StopMainThread() = 0;

    /**
     * @brief 异步启动主线程
     * 
     * 返回后不保证主线程开始运行，主线程会在随后自主运行
     */
    virtual void WINAPI StartMainThreadAsync() = 0;

    /**
     * @brief 异步结束主线程
     *
     * 返回后不保证主线程停止，主线程会自主停止运行
     */
    virtual void WINAPI StopMainThreadAsync() = 0;

    /**
     * @brief 主线程是否正在运行
     * @return 返回主线程是否正在运行
     */
    virtual BOOL WINAPI IsMainThreadRunning() = 0;
};

/** 构造函数与析构函数 */
extern "C" 
{
    /**
     * @brief 基础构造函数
     *
     * 以指定函数为主函数创建线程，线程结束不执行任何函数
     *
     * @param[in] Func 主线程执行函数
     * @param[in] Param 主线程执行函数的入参
     */
    DLL_COMMONLIB_API IThread* WINAPI CreateIThreadInstance(ThreadMainProc Func, LPVOID Param);
    /**
     * @brief 完整的构造函数
     *
     * 以指定函数为主函数创建线程，线程结束会调用结束函数
     *
     * @param[in] MainFunc 主线程执行函数
     * @param[in] MainParam 主线程入参
     * @param[in] Endfunc 线程结束时的执行函数
     * @param[in] Endparam 线程结束时的入参
     */
    DLL_COMMONLIB_API IThread* WINAPI CreateIThreadInstanceEx(ThreadMainProc MainFunc, LPVOID MainParam, ThreadEndProc Endfunc, LPVOID Endparam);

    /**
     * @brief 析构函数
     */
    DLL_COMMONLIB_API void WINAPI DestoryIThreadInstance(IThread* Object);
}

#endif
