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
	bool InitDB();	//初始化数据库连接
	bool InitVideoChannel();	//从数据库读取视频通道信息

private:
	_ConnectionPtr m_pDB;	//数据库连接指针
	wstring m_wsProgramPath;	//主程序目录
	map<wstring, CHANNEL_CONFIG>	m_mapChannels;		//视频通道配置，用于动态解码
};
#endif


