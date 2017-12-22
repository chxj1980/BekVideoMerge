#pragma once
#ifndef __ENCODE_MANAGER_H__
#define  __ENCODE_MANAGER_H__

class CEncodeManager
{
public:
	CEncodeManager();
	~CEncodeManager();

public:
	void InitCar(int userId, int carNo, DWORD carWnd, DWORD itemWnd);
	bool StartCarEncode(CHANNEL_CONFIG channel);	//开启副驾视频动态解码
	bool StartItemEncode(CHANNEL_CONFIG channel);	//开启项目视频动态解码

private:
	bool StartDynamicDecode(CHANNEL_CONFIG channel, DWORD wnd);	//启动动态解码
	bool StopDynamicDecode(DWORD wnd);													//停止动态解码

private:
	int m_nUserId;		//设备登录标识
	int m_nCarNo;		//考车号
	DWORD m_dwCarWnd;
	DWORD m_dwItemWnd;
	wstring m_wsProgramPath;

};
#endif




