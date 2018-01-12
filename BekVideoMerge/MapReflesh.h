#pragma once
#ifndef __MAP_REFLESH_H__
#define __MAP_REFLESH_H__

#include "TranscodingBase.h"

class CMapReflesh : public CTranscodingBase
{
public:
	CMapReflesh();
	~CMapReflesh();

public:
	void StartWork();
	void SetCarSignal(CarSignal signal);
	void Handle17C51();
	void Handle17C52(int xmNo, wstring xmName);
	void Handle17C53(ERROR_DATA judgeInfo);
	void Handle17C55(int xmNo, wstring xmName);
	void Handle17C56(bool bPass, int nScore);

protected:
	static BOOL MapRefleshThreadProc(LPVOID parameter, HANDLE stopEvent);

private:
	void LoadMapConfig();
	void DrawBackground(Graphics *graphics);	//绘制背景
	bool GetCarRelativeCoordinate(CarSignal signal, int &x, int &y);
	void DrawMap(Graphics *graphics, int carX, int carY);	//绘制地图
	void DrawCar(Graphics *graphics, float angle);	//绘制车模型
	void DrawStatus(CarSignal carSignal);	//绘制状态信息

private:
	IThread* m_mapRefleshThread;
	CarSignal m_carSignal;		//车载信号数据
	CRITICAL_SECTION m_carSignalLock;	//车载信号锁
	HANDLE	m_refleshEvent;
	bool m_bDrawCar;	//是否绘制考车
	int m_mapMaxX;
	int m_mapMinX;		
	int m_mapMaxY;
	int m_mapMinY;
	int m_mapZoomIn;
	int m_mapWidth;
	int m_mapHeight;
	int m_mapSplitWidth;

	bool m_bBigCar;		//大车的项目牌与小车不一样
	int m_nKskm;	//考试科目

	bool m_bStartExam;	//开始考试
	bool m_bEndExam;	//结束考试
	bool m_bPass;		//考试是否通过
	int m_nDisplayDelays;		//考试结束后延迟一段时间再结束画面
	int m_nCurrentScore;	//当前得分
	wstring m_wsExamStatus;	//考试状态描述
	CTime m_startTime;	//考试开始时间
	CTime m_endTime;		//考试结束时间
	map<int, ERROR_DATA> m_mapJudgeInfos;	//扣分信息
	int m_nStartXmStatus;		//标识哪些项目已经开始
	int m_nEndXmStatus;		//标识哪些项目已经结束
};

#endif



