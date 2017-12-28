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

protected:
	static BOOL MapRefleshThreadProc(LPVOID parameter, HANDLE stopEvent);

private:
	void LoadMapConfig();
	void DrawBackground(Graphics *graphics);	//���Ʊ���
	bool GetCarRelativeCoordinate(CarSignal signal, int &x, int &y);
	void DrawMap(Graphics *graphics, int carX, int carY);	//���Ƶ�ͼ
	void DrawStatus(CarSignal carSignal);	//����״̬��Ϣ

private:
	IThread* m_mapRefleshThread;
	CarSignal m_carSignal;		//�����ź�����
	CRITICAL_SECTION m_carSignalLock;	//�����ź���
	HANDLE	m_refleshEvent;
	bool m_bDrawCar;	//�Ƿ���ƿ���
	int m_mapMaxX;
	int m_mapMinX;		
	int m_mapMaxY;
	int m_mapMinY;
	int m_mapZoomIn;
	int m_mapWidth;
	int m_mapHeight;
	int m_mapSplitWidth;

	bool m_bBigCar;		//�󳵵���Ŀ����С����һ��
	int m_nKskm;	//���Կ�Ŀ

	bool m_bStartExam;	//��ʼ����
	wstring m_wsExamStatus;	//����״̬����
	int m_nDisplayDelays;		//���Խ������ӳ�һ��ʱ���ٽ�������
	CTime m_startTime;	//���Կ�ʼʱ��
};

#endif



