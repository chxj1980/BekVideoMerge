#pragma once
#ifndef __STUDENT_INFO_REFLESH_H__
#define __STUDENT_INFO_REFLESH_H__

#include "TranscodingBase.h"

using namespace Gdiplus;

class CStudentInfoReflesh : public CTranscodingBase
{
public:
	CStudentInfoReflesh();
	~CStudentInfoReflesh();

public:
	void StartWork();
	void SetCarSignal(CarSignal signal);
	void Handle17C51(StudentInfo studentInfo);
	void Handle17C52(int xmNo, wstring xmName);
	void Handle17C53(ERROR_DATA judgeInfo);
	void Handle17C55(int xmNo, wstring xmName);
	void Handle17C56(bool bPass, int nScore);

protected:
	static BOOL StudentInfoRefleshThreadProc(LPVOID parameter, HANDLE stopEvent);

private:
	void DrawBackground(Graphics *graphics);	//���Ʊ���
	void DrawItemBackground(Graphics *graphics);	//������Ŀ�Ʊ���
	void DrawKM3Background(Graphics *graphics);	//��Ŀ����Ŀ�Ʊ���
	void DrawNormalItem(Graphics *graphics, int x, int y, int width, int height, wstring wsItem);	//���Ƶ�����Ŀͼ��
	void DrawEnterItem(Graphics *graphics, int x, int y, int width, int height, wstring wsItem);	//���Ƶ�����Ŀͼ��
	void DrawLeaveItem(Graphics *graphics, int x, int y, int width, int height, wstring wsItem);	//���Ƶ�����Ŀͼ��
	void DrawStudentInfo(Graphics *graphics);		//���ƿ�����Ϣ
	void DrawJudgement(Graphics *graphics);		//���ƿ۷���Ϣ
	void DrawPhoto(Graphics *graphics);		//������Ƭ
	void DrawSignal(Graphics *graphics);	//�����ź�
	void DrawScore(Graphics *graphics);	//���Ƶ÷�
	void DrawCurrentItem(Graphics *graphics);	//����ʵʱ��Ŀ״̬
	void DrawKM3EnterItem(Graphics *graphics);	//�������ڽ��е���Ŀ
	void DrawKM3ELeaveItem(Graphics *graphics);	//�������ڽ��е���Ŀ

private:
	IThread* m_studentInfoRefleshThread;
	CarSignal m_carSignal;		//�����ź�����
	CRITICAL_SECTION m_carSignalLock;	//�����ź���
	StudentInfo m_studentInfo;
	HANDLE	m_refleshEvent;

	map<int, ERROR_DATA> m_mapJudgeInfos;	//�۷���Ϣ
	bool m_bStartExam;	//��ʼ����
	bool m_bEndExam;		//���Խ���
	bool m_bPass;		//�����Ƿ�ͨ��
	int m_nDisplayDelays;		//���Խ������ӳ�һ��ʱ���ٽ�������
	int m_nCurrentScore;	//��ǰ�÷�
	int m_nStartXmStatus;		//��ʶ��Щ��Ŀ�Ѿ���ʼ
	int m_nEndXmStatus;		//��ʶ��Щ��Ŀ�Ѿ�����
	bool m_bBigCar;		//�󳵵���Ŀ����С����һ��
	int m_nKskm;	//���Կ�Ŀ
};

#endif



