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
	void InitParameter();
	bool InitDB();

private:
	_ConnectionPtr m_pDB;	//���ݿ�����ָ��
	wstring m_sProgramPath;	//������Ŀ¼
	wstring m_sLogPath;	//��־Ŀ¼
	wstring m_sConfPath;	//�����ļ�Ŀ¼
	wstring m_sResPath;	//��Դ�ļ�Ŀ¼
	wstring m_sMapPath;		//��ͼ�ļ�Ŀ¼
	wstring m_sThirdPartyPath;		//����������Ŀ¼
	wstring m_sVideoPath;	//��Ƶת����ʱĿ¼
};

#endif



