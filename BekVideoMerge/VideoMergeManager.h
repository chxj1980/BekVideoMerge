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
	wstring m_wsProgramPath;	//������Ŀ¼
	//wstring m_wsLogPath;	//��־Ŀ¼
	//wstring m_wsConfPath;	//�����ļ�Ŀ¼
	//wstring m_wsResPath;	//��Դ�ļ�Ŀ¼
	//wstring m_wsMapPath;		//��ͼ�ļ�Ŀ¼
	//wstring m_wsThirdPartyPath;		//����������Ŀ¼
	//wstring m_wsVideoPath;	//��Ƶת����ʱĿ¼
};

#endif



