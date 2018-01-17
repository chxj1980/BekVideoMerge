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
	void DrawBackground(Graphics *graphics);	//绘制背景

private:
	IThread* m_studentInfoRefleshThread;
	StudentInfo m_studentInfo;
	HANDLE	m_refleshEvent;

	bool m_bStartExam;	//开始考试
	bool m_bEndExam;		//考试结束
	bool m_bPass;		//考试是否通过
	int m_nDisplayDelays;		//考试结束后延迟一段时间再结束画面
	
};

#endif



