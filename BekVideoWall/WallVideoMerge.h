#pragma once

#ifndef __WALL_VIDEO_MERGE_H__
#define __WALL_VIDEO_MERGE_H__

#include "Function.h"

class CWallVideoMerge
{
public:
	CWallVideoMerge();
	~CWallVideoMerge();

public:
	bool StartWork();

private:
	void InitParameter();
	bool InitDB();	//��ʼ�����ݿ�����
	bool InitVideoChannel();	//�����ݿ��ȡ��Ƶͨ����Ϣ

private:
	_ConnectionPtr m_pDB;	//���ݿ�����ָ��
	wstring m_wsProgramPath;	//������Ŀ¼
	map<wstring, CHANNEL_CONFIG>	m_mapChannels;		//��Ƶͨ�����ã����ڶ�̬����
};
#endif


