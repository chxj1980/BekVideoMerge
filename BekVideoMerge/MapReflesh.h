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
	void Handle17C56(bool bPass);

protected:
	static BOOL MapRefleshThreadProc(LPVOID parameter, HANDLE stopEvent);

private:
	void LoadMapConfig();
	void DrawBackground(Graphics *graphics);	//���Ʊ���
	bool GetCarRelativeCoordinate(CarSignal signal, int &x, int &y);
	void DrawMap(Graphics *graphics, int carX, int carY);	//���Ƶ�ͼ
	void DrawCar(Graphics *graphics, float angle);	//���Ƴ�ģ��
	void DrawStatus(Graphics *graphics, CarSignal carSignal);	//����״̬��Ϣ

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

	bool m_bStartExam;	//��ʼ����
	bool m_bEndExam;	//��������
	bool m_bPass;		//�����Ƿ�ͨ��
	int m_nDisplayDelays;		//���Խ������ӳ�һ��ʱ���ٽ�������
	wstring m_wsExamStatus;	//����״̬����
	CTime m_startTime;	//���Կ�ʼʱ��
	CTime m_endTime;		//���Խ���ʱ��
};

#endif



