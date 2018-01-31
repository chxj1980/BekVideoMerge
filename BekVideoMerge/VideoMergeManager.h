#pragma once
#ifndef __VIDEO_MERGE_MANAGER_H__
#define __VIDEO_MERGE_MANAGER_H__

#include "Function.h"
#include "CarManager.h"

class CVideoMergeManager
{
public:
	CVideoMergeManager();
	~CVideoMergeManager();

public:
	bool StartWork();
	bool HandleExamSignal(wstring buf);	//�����Թ�����Ϣ����
	bool HandleCarSignal(int carNo, char* buf);		//�������ź�����

private:
	void InitParameter();	//������ʼ��
	bool InitDB();	//��ʼ�����ݿ�����
	bool InitVideoChannel();	//�����ݿ��ȡ��Ƶͨ����Ϣ
	bool InitItems();	//�����ݿ��ȡ��Ŀ�����Ϣ���۷ֱ����Ϣ
	bool InitDevice();	//����������������ʼ��
	bool InitDVIChannel(int userId, int deviceNo, NET_DVR_MATRIX_ABILITY_V41 struDecAbility);	//������DVIͨ����⼰��ʼ��
	bool InitBNCChannel(int userId, int deviceNo, NET_DVR_MATRIX_ABILITY_V41 struDecAbility);	//������BNCͨ����⼰��ʼ��
	bool Run();
	bool GetVideoChannel(wstring key, CHANNEL_CONFIG &videoChannel);	//���ݱ�Ų�����Ƶͨ����Ϣ
	bool GetCarManager(int carNo);	//���ݿ����Ų��ҿ���������
	bool GetStudentInfo(wstring cetificateNo, int carNo, StudentInfo &studentInfo);	//�����ݿ��ѯ������Ϣ
	wstring GetExamReason(wstring code);	//���ݿ���ԭ���ţ���ȡ����ԭ��

private:
	_ConnectionPtr m_pDB;	//���ݿ�����ָ��
	wstring m_wsProgramPath;	//������Ŀ¼
	map<int, CCarManager> m_mapCarManagers;		//��������
	map<wstring, CHANNEL_CONFIG>	m_mapChannels;		//��Ƶͨ�����ã����ڶ�̬����
	map<wstring, ERROR_DATA> m_mapItems;	//��Ŀ�����Ϣ���۷ֱ����Ϣ
	UINT m_displayChan[DISPLAY_CHAN_NUMS];		//�Ӵ��ںͽ���ͨ���Ķ�Ӧ��ϵ
	UINT m_audioWindow;	//��Ƶ����

	int m_nKskm;	//���Կ�Ŀ
	int m_nDBType;	//���ݿ�����
	bool m_bDynamicVideo;	//������Ƿ�����̬�л�
	bool m_bBigCar;		//�󳵿�Ŀ���ϸ������һ��

	int m_nWndIndexMap;	//��ͼ���洰�������ţ�Ĭ�������Ͻ�
	int m_nWndIndexStuInfo;	//������Ϣ���������ţ�Ĭ�������½�
	int m_nWndIndexMainDriving;	//������Ƶ���������ţ�Ĭ�������½�
	int m_nWndIndexCopilot;	//������Ƶ���������ţ�Ĭ�����ұ��в�
	int m_nWndIndexItem;	//��Ŀ��Ƶ���������ţ�Ĭ�������Ͻ�
};

#endif



