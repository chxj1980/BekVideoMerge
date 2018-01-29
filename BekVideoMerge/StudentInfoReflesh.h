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
	void Handle17C51(StudentInfo studentInfo);
	void Handle17C53(ERROR_DATA judgeInfo);
	void Handle17C56(bool bPass, int nScore);

protected:
	static BOOL StudentInfoRefleshThreadProc(LPVOID parameter, HANDLE stopEvent);

private:
	void DrawBackground(Graphics *graphics);	//���Ʊ���
	void DrawStudentInfo(Graphics *graphics);		//���ƿ�����Ϣ
	void DrawJudgement(Graphics *graphics);		//���ƿ۷���Ϣ
	void DrawPhoto(Graphics *graphics);		//������Ƭ

private:
	IThread* m_studentInfoRefleshThread;
	StudentInfo m_studentInfo;
	HANDLE	m_refleshEvent;

	map<int, ERROR_DATA> m_mapJudgeInfos;	//�۷���Ϣ
	bool m_bStartExam;	//��ʼ����
	bool m_bEndExam;		//���Խ���
	bool m_bPass;		//�����Ƿ�ͨ��
	int m_nDisplayDelays;		//���Խ������ӳ�һ��ʱ���ٽ�������
	int m_nCurrentScore;	//��ǰ�÷�
	
};

#endif



