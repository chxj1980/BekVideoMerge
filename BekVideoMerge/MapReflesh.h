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
	void DrawBackground(Graphics *graphics);	//���Ʊ���
	bool GetCarRelativeCoordinate(CarSignal signal, int &x, int &y);
	void DrawMap(Graphics *graphics, int carX, int carY);	//���Ƶ�ͼ
	void DrawCar(Graphics *graphics, float angle);	//���Ƴ�ģ��
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
	bool m_bEndExam;	//��������
	bool m_bPass;		//�����Ƿ�ͨ��
	int m_nDisplayDelays;		//���Խ������ӳ�һ��ʱ���ٽ�������
	int m_nCurrentScore;	//��ǰ�÷�
	wstring m_wsExamStatus;	//����״̬����
	CTime m_startTime;	//���Կ�ʼʱ��
	CTime m_endTime;		//���Խ���ʱ��
	map<int, ERROR_DATA> m_mapJudgeInfos;	//�۷���Ϣ
	int m_nStartXmStatus;		//��ʶ��Щ��Ŀ�Ѿ���ʼ
	int m_nEndXmStatus;		//��ʶ��Щ��Ŀ�Ѿ�����
};

#endif



