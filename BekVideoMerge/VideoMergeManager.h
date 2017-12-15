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

private:
	_ConnectionPtr m_pDB;	//���ݿ�����ָ��
	wstring m_wsProgramPath;	//������Ŀ¼
	map<int, CCarManager> m_mapCarManagers;		//��������
	map<wstring, CHANNEL_CONFIG>	m_mapChannels;		//��Ƶͨ�����ã����ڶ�̬����
	map<wstring, ERROR_DATA> m_mapItems;	//��Ŀ�����Ϣ���۷ֱ����Ϣ
	UINT m_displayChan[DISPLAY_CHAN_NUMS];		//�Ӵ��ںͽ���ͨ���Ķ�Ӧ��ϵ
	UINT m_audioWindow;	//��Ƶ����
};

#endif



