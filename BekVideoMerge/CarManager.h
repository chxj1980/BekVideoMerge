#pragma once
#ifndef __CAR_MANAGER_H__
#define  __CAR_MANAGER_H__

#include "StudentInfoReflesh.h"
#include "MapReflesh.h"

class CCarManager
{
public:
	CCarManager();
	~CCarManager();

public:
	void InitCar(int userId, int carNo, BYTE(&decChan)[DISPLAY_CHAN_NUMS]);
	void InitPassiveMode(int studentInfoHandle, int mapHandle);			//初始化被动解码相关参数
	bool StartDynamicDecode(CHANNEL_CONFIG channel, int wnd);	//启动动态解码
	bool StartPassiveDecode(int wnd, LONG &lpHandle);						//启动被动解码
	bool StopDynamicDecode(int wnd);													//停止动态解码

private:
	void InitStudentInfoPic();
	void InitMapPic();

private:
	int m_nUserId;		//设备登录标识
	int m_nCarNo;		//考车号
	int m_nStudentInfoHandle;		//考生信息画面的被动解码句柄
	int m_nMapHandle;		//实时轨迹画面的被动解码句柄
	BYTE m_decChan[DISPLAY_CHAN_NUMS];	//考车所对应的解码通道号
	wstring m_wsProgramPath;

	CStudentInfoReflesh m_studentInfoRefleshClass;
	CMapReflesh m_mapRefleshClass;
};
#endif




