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
	bool InitChannel();
	bool InitErrorData();

private:
	_ConnectionPtr m_pDB;	//数据库连接指针
	wstring m_wsProgramPath;	//主程序目录
	map<wstring, CHANNEL_CONFIG>	m_mapChannels;		//视频通道配置
	map<wstring, ERROR_DATA> m_mapErrorDatas;	//项目编号信息、扣分编号信息
};

#endif



