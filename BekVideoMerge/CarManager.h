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
	int m_nUserId;		//�豸��¼��ʶ
	int m_nCarNo;		//������
	BYTE m_decChan[DISPLAY_CHAN_NUMS];	//��������Ӧ�Ľ���ͨ����
};
#endif




