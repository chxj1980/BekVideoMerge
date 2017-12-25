#pragma once

#ifndef __WALL_VIDEO_MERGE_H__
#define __WALL_VIDEO_MERGE_H__

#include "Function.h"
#include "EncodeManager.h"

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
	bool InitDevice();	//��������ʼ��
	bool InitDVIChannel(int userId, int deviceNo, NET_DVR_MATRIX_ABILITY_V41 struDecAbility);	//������DVIͨ����⼰��ʼ��
	bool Run();
	bool GetVideoChannel(wstring key, CHANNEL_CONFIG &videoChannel);	//���ݱ�Ų�����Ƶͨ����Ϣ

private:
	_ConnectionPtr m_pDB;	//���ݿ�����ָ��
	wstring m_wsProgramPath;	//������Ŀ¼
	map<wstring, CHANNEL_CONFIG>	m_mapChannels;		//��Ƶͨ�����ã����ڶ�̬����
	map<int, CEncodeManager>	m_mapCarEncoders;		
};
#endif


