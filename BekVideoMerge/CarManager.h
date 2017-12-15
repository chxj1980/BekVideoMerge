#pragma once
#ifndef __CAR_MANAGER_H__
#define  __CAR_MANAGER_H__

class CCarManager
{
public:
	CCarManager();
	~CCarManager();

public:
	void InitCar(int userId, int carNo, BYTE(&decChan)[DISPLAY_CHAN_NUMS]);
	bool StartDynamicDecode(CHANNEL_CONFIG channel, int wnd);
	bool StartPassiveDecode(int wnd, LONG &lpHandle);
	bool StopDynamicDecode(int wnd);

private:
	int m_nUserId;		//设备登录标识
	int m_nCarNo;		//考车号
	BYTE m_decChan[DISPLAY_CHAN_NUMS];	//考车所对应的解码通道号
};
#endif




