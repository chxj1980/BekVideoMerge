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

protected:
	static BOOL MapRefleshThreadProc(LPVOID parameter, HANDLE stopEvent);

private:
	void LoadMapConfig();
	void DrawBackground(Graphics *graphics);	//绘制背景
	bool GetCarRelativeCoordinate(CarSignal signal, int &x, int &y);
	void DrawMap(Graphics *graphics, int carX, int carY);	//绘制地图

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
};

#endif



