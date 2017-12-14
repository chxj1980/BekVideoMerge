#pragma once
#ifndef __VIDEO_MERGE_MANAGER_H__
#define __VIDEO_MERGE_MANAGER_H__

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
	bool InitChannel();	//�����ݿ��ȡ��Ƶͨ����Ϣ
	bool InitErrorData();	//�����ݿ��ȡ��Ŀ�����Ϣ���۷ֱ����Ϣ
	bool InitDevice();	//����������������ʼ��

private:
	_ConnectionPtr m_pDB;	//���ݿ�����ָ��
	wstring m_wsProgramPath;	//������Ŀ¼
	map<wstring, CHANNEL_CONFIG>	m_mapChannels;		//��Ƶͨ�����ã����ڶ�̬����
	map<wstring, ERROR_DATA> m_mapErrorDatas;	//��Ŀ�����Ϣ���۷ֱ����Ϣ
};

#endif



