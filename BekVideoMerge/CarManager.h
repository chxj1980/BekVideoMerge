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
	void InitPassiveMode(int studentInfoHandle, int mapHandle);			//��ʼ������������ز���
	bool StartDynamicDecode(CHANNEL_CONFIG channel, int wnd);	//������̬����
	bool StartPassiveDecode(int wnd, LONG &lpHandle);						//������������
	bool StopDynamicDecode(int wnd);													//ֹͣ��̬����
	bool HandleCarSignal(CarSignal signal);		//�������ź�
	bool Handle17C51(StudentInfo studentInfo);	//���Կ�ʼ
	bool Handle17C53(ERROR_DATA judgeInfo);	//��Ŀ�۷� 
	bool Handle17C56(bool bPass, int nScore);	//���Խ���

private:
	void InitStudentInfoPic();
	void InitMapPic();

private:
	int m_nUserId;		//�豸��¼��ʶ
	int m_nCarNo;		//������
	int m_nStudentInfoHandle;		//������Ϣ����ı���������
	int m_nMapHandle;		//ʵʱ�켣����ı���������
	BYTE m_decChan[DISPLAY_CHAN_NUMS];	//��������Ӧ�Ľ���ͨ����
	wstring m_wsProgramPath;		//������·��
	bool m_bDrawMap;	//�Ƿ���Ƶ�ͼ

	CStudentInfoReflesh m_studentInfoRefleshClass;
	CMapReflesh m_mapRefleshClass;
};
#endif




