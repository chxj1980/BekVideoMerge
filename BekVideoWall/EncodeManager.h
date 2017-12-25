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
	bool StartCarEncode(CHANNEL_CONFIG channel);	//����������Ƶ��̬����
	bool StartItemEncode(CHANNEL_CONFIG channel);	//������Ŀ��Ƶ��̬����

private:
	bool StartDynamicDecode(CHANNEL_CONFIG channel, DWORD wnd);	//������̬����
	bool StopDynamicDecode(DWORD wnd);													//ֹͣ��̬����

private:
	int m_nUserId;		//�豸��¼��ʶ
	int m_nCarNo;		//������
	DWORD m_dwCarWnd;
	DWORD m_dwItemWnd;
	wstring m_wsProgramPath;

};
#endif




