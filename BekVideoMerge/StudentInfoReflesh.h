#pragma once
#ifndef __STUDENT_INFO_REFLESH_H__
#define __STUDENT_INFO_REFLESH_H__

#include "TranscodingBase.h"

class CStudentInfoReflesh : public CTranscodingBase
{
public:
	CStudentInfoReflesh();
	~CStudentInfoReflesh();

public:
	void StartWork();
	void Handle17C51(StudentInfo studentInfo);
	void Handle17C56(bool bPass);

protected:
	static BOOL StudentInfoRefleshThreadProc(LPVOID parameter, HANDLE stopEvent);

private:
	void DrawBackground(Graphics *graphics);	//���Ʊ���

private:
	IThread* m_studentInfoRefleshThread;
	StudentInfo m_studentInfo;
	HANDLE	m_refleshEvent;

	bool m_bStartExam;	//��ʼ����
	bool m_bEndExam;		//���Խ���
	bool m_bPass;		//�����Ƿ�ͨ��
	int m_nDisplayDelays;		//���Խ������ӳ�һ��ʱ���ٽ�������
	
};

#endif



