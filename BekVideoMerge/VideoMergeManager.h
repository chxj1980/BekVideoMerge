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
	_ConnectionPtr m_pDB;	//数据库连接指针
	wstring m_wsProgramPath;	//主程序目录
	//wstring m_wsLogPath;	//日志目录
	//wstring m_wsConfPath;	//配置文件目录
	//wstring m_wsResPath;	//资源文件目录
	//wstring m_wsMapPath;		//地图文件目录
	//wstring m_wsThirdPartyPath;		//第三方工具目录
	//wstring m_wsVideoPath;	//视频转换临时目录
};

#endif



