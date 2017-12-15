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
	void InitParameter();	//参数初始化
	bool InitDB();	//初始化数据库连接
	bool InitVideoChannel();	//从数据库读取视频通道信息
	bool InitItems();	//从数据库读取项目编号信息，扣分编号信息
	bool InitDevice();	//合码器、解码器初始化
	bool InitDVIChannel(int userId, int deviceNo, NET_DVR_MATRIX_ABILITY_V41 struDecAbility);	//合码器DVI通道检测及初始化
	bool InitBNCChannel(int userId, int deviceNo, NET_DVR_MATRIX_ABILITY_V41 struDecAbility);	//解码器BNC通道检测及初始化
	bool Run();
	bool GetVideoChannel(wstring key, CHANNEL_CONFIG &videoChannel);	//根据编号查找视频通道信息

private:
	_ConnectionPtr m_pDB;	//数据库连接指针
	wstring m_wsProgramPath;	//主程序目录
	map<int, CCarManager> m_mapCarManagers;		//考车管理
	map<wstring, CHANNEL_CONFIG>	m_mapChannels;		//视频通道配置，用于动态解码
	map<wstring, ERROR_DATA> m_mapItems;	//项目编号信息、扣分编号信息
	UINT m_displayChan[DISPLAY_CHAN_NUMS];		//子窗口和解码通道的对应关系
	UINT m_audioWindow;	//音频窗口
};

#endif



