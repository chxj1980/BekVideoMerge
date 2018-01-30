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
	void DrawBackground(Graphics *graphics);	//绘制背景
	void DrawItemBackground(Graphics *graphics);	//绘制项目牌背景
	void DrawKM3Background(Graphics *graphics);	//科目三项目牌背景
	void DrawNormalItem(Graphics *graphics, int x, int y, int width, int height, wstring wsItem);	//绘制单个项目图标
	void DrawEnterItem(Graphics *graphics, int x, int y, int width, int height, wstring wsItem);	//绘制单个项目图标
	void DrawLeaveItem(Graphics *graphics, int x, int y, int width, int height, wstring wsItem);	//绘制单个项目图标
	void DrawStudentInfo(Graphics *graphics);		//绘制考生信息
	void DrawJudgement(Graphics *graphics);		//绘制扣分信息
	void DrawPhoto(Graphics *graphics);		//绘制照片
	void DrawSignal(Graphics *graphics);	//绘制信号
	void DrawScore(Graphics *graphics);	//绘制得分
	void DrawCurrentItem(Graphics *graphics);	//绘制实时项目状态
	void DrawKM3EnterItem(Graphics *graphics);	//绘制正在进行的项目
	void DrawKM3ELeaveItem(Graphics *graphics);	//绘制正在进行的项目

private:
	IThread* m_studentInfoRefleshThread;
	CarSignal m_carSignal;		//车载信号数据
	CRITICAL_SECTION m_carSignalLock;	//车载信号锁
	StudentInfo m_studentInfo;
	HANDLE	m_refleshEvent;

	map<int, ERROR_DATA> m_mapJudgeInfos;	//扣分信息
	bool m_bStartExam;	//开始考试
	bool m_bEndExam;		//考试结束
	bool m_bPass;		//考试是否通过
	int m_nDisplayDelays;		//考试结束后延迟一段时间再结束画面
	int m_nCurrentScore;	//当前得分
	int m_nStartXmStatus;		//标识哪些项目已经开始
	int m_nEndXmStatus;		//标识哪些项目已经结束
	bool m_bBigCar;		//大车的项目牌与小车不一样
	int m_nKskm;	//考试科目
};

#endif



