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
	void InitParameter();	//参数初始化
	bool InitDB();	//初始化数据库连接
	bool InitChannel();	//从数据库读取视频通道信息
	bool InitErrorData();	//从数据库读取项目编号信息，扣分编号信息
	bool InitDevice();	//合码器、解码器初始化

private:
	_ConnectionPtr m_pDB;	//数据库连接指针
	wstring m_wsProgramPath;	//主程序目录
	map<wstring, CHANNEL_CONFIG>	m_mapChannels;		//视频通道配置，用于动态解码
	map<wstring, ERROR_DATA> m_mapErrorDatas;	//项目编号信息、扣分编号信息
};

#endif



