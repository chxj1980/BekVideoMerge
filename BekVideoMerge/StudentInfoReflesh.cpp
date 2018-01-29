#include "stdafx.h"
#include "StudentInfoReflesh.h"

#define MODULE_NAME	_T("CStudentInfoReflesh")

CStudentInfoReflesh::CStudentInfoReflesh()
{
	InitializeCriticalSection(&this->m_carSignalLock);
	m_refleshEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_bStartExam = false;
	m_bEndExam = false;
	m_bPass = false;
	m_nDisplayDelays = 0;
	m_nCurrentScore = EXAM_TOTAL_SCORE;
	m_mapJudgeInfos.clear();

	SetEvent(m_refleshEvent);
}

CStudentInfoReflesh::~CStudentInfoReflesh()
{
	if (NULL != this->m_studentInfoRefleshThread && this->m_studentInfoRefleshThread->IsMainThreadRunning())
	{
		this->m_studentInfoRefleshThread->StopMainThread();
		delete this->m_studentInfoRefleshThread;
	}

	CloseHandle(this->m_refleshEvent);

	//清理锁以及其他
	DeleteCriticalSection(&this->m_carSignalLock);
}

void CStudentInfoReflesh::StartWork()
{
	m_studentInfoRefleshThread = CreateIThreadInstance(StudentInfoRefleshThreadProc, LPVOID(this));
	m_studentInfoRefleshThread->StartMainThread();
}

void CStudentInfoReflesh::SetCarSignal(CarSignal signal)
{
	if (m_bStartExam)
	{
		EnterCriticalSection(&this->m_carSignalLock);
		m_carSignal = signal;
		LeaveCriticalSection(&this->m_carSignalLock);
	}
}

void CStudentInfoReflesh::Handle17C51(StudentInfo studentInfo)
{
	m_studentInfo = studentInfo;

	m_mapJudgeInfos.clear();
	m_bStartExam = true;
	m_bEndExam = false;
	m_nDisplayDelays = 0;
	m_nCurrentScore = EXAM_TOTAL_SCORE;

	SetEvent(m_refleshEvent);
}

void CStudentInfoReflesh::Handle17C53(ERROR_DATA judgeInfo)
{
	int nIndex = m_mapJudgeInfos.size();
	if (nIndex < 5)	//最多展示5条扣分信息
	{
		m_mapJudgeInfos[nIndex] = judgeInfo;
	}

	m_nCurrentScore -= judgeInfo.ikcfs;
	if (m_nCurrentScore < 0)
	{
		m_nCurrentScore = 0;
	}
}

void CStudentInfoReflesh::Handle17C56(bool bPass, int nScore)
{
	m_bPass = bPass;
	m_nCurrentScore = nScore;
	m_bStartExam = false;
	m_bEndExam = true;
	m_nDisplayDelays = DISPLAY_DELAY_SECONDS;
}

BOOL CStudentInfoReflesh::StudentInfoRefleshThreadProc(LPVOID parameter, HANDLE stopEvent)
{
	L_INFO(_T("StudentInfoRefleshThreadProc Start\n"));

	CStudentInfoReflesh *studentInfoRefleshClass = (CStudentInfoReflesh*)parameter;

	Graphics graphics(studentInfoRefleshClass->m_DC.GetSafeHdc());
	CFont fontz;
	fontz.CreateFont(20, 0, 0, 0, FW_BOLD, FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
		DRAFT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"宋体");
	studentInfoRefleshClass->m_DC.SelectObject(&fontz);
	studentInfoRefleshClass->m_DC.SetBkMode(TRANSPARENT);//透明
	studentInfoRefleshClass->m_DC.SetTextColor(RGB(255, 255, 255));

	while (true)
	{
		DWORD dwRet = WaitForSingleObject(studentInfoRefleshClass->m_refleshEvent, INFINITE);
		if (WAIT_OBJECT_0 == dwRet)
		{
			if (studentInfoRefleshClass->m_bStartExam || !studentInfoRefleshClass->m_bEndExam || studentInfoRefleshClass->m_nDisplayDelays > 0)
			{				
				//绘制背景
				studentInfoRefleshClass->DrawBackground(&graphics);
				studentInfoRefleshClass->DrawItemBackground(&graphics);

				if (studentInfoRefleshClass->m_bStartExam || studentInfoRefleshClass->m_nDisplayDelays > 0)	//考试中才展示这些信息
				{
					//绘制考生基本信息
					studentInfoRefleshClass->DrawStudentInfo(&graphics);

					//绘制扣分信息
					studentInfoRefleshClass->DrawJudgement(&graphics);

					//绘制实时信号
					studentInfoRefleshClass->DrawSignal(&graphics);

					//绘制成绩
					studentInfoRefleshClass->DrawScore(&graphics);
				}

				//刷新四合一界面
				studentInfoRefleshClass->Reflesh();

				Sleep(500);

				//考试结束后，继续显示一段时间
				if (studentInfoRefleshClass->m_nDisplayDelays > 0)
				{
					studentInfoRefleshClass->m_nDisplayDelays -= 1;
				}

				SetEvent(studentInfoRefleshClass->m_refleshEvent);
			}
		}

	}

EXIT:
	L_INFO(_T("StudentInfoRefleshThreadProc Exit\n"));
	return TRUE;
}

//绘制背景
void CStudentInfoReflesh::DrawBackground(Graphics *graphics)
{
	try
	{
		wstring wsStuBackground = m_wsProgramPath + IMG_PATH_STUDENT_BACKGROUND;
		if (!CWinUtils::FileExists(wsStuBackground))
		{
			L_ERROR(_T("wsStuBackground not exist, file name = %s\n"), wsStuBackground.c_str());
			return;
		}

		Image *imgStuBackground = Image::FromFile(wsStuBackground.c_str());
		int width = imgStuBackground->GetWidth();
		int height = imgStuBackground->GetHeight();
		graphics->DrawImage(imgStuBackground, Rect(0, 0, m_nWidth, m_nHeight), 0, 0, width, height, UnitPixel);

		delete imgStuBackground;
	}
	catch (...)
	{
		L_ERROR(_T("DrawBackground catch an error.\n"));
	}
}

//绘制项目牌背景
void CStudentInfoReflesh::DrawItemBackground(Graphics *graphics)
{
	DrawKM3Background(graphics);
}

void CStudentInfoReflesh::DrawKM3Background(Graphics *graphics)
{
	try
	{
		int x = 20;
		int y = 300;
		int imgWidth = 50;
		int splitWidth = 80;

		//上车准备
		DrawNormalItem(graphics, x + 0 * splitWidth, y, imgWidth, imgWidth, ITEM_SCZB);
		
		//起步
		DrawNormalItem(graphics, x + 1 * splitWidth, y, imgWidth, imgWidth, ITEM_QB);

		//直线行驶
		DrawNormalItem(graphics, x + 2 * splitWidth, y, imgWidth, imgWidth, ITEM_ZXXS);

		//加减档
		DrawNormalItem(graphics, x + 3 * splitWidth, y, imgWidth, imgWidth, ITEM_JJD);

		//变更车道
		DrawNormalItem(graphics, x + 4 * splitWidth, y, imgWidth, imgWidth, ITEM_BGCD);
		
		//靠边停车
		DrawNormalItem(graphics, x + 5 * splitWidth, y, imgWidth, imgWidth, ITEM_KBTC);

		//直行通过路口
		DrawNormalItem(graphics, x + 6 * splitWidth, y, imgWidth, imgWidth, ITEM_ZX);

		//左转
		DrawNormalItem(graphics, x + 7 * splitWidth, y, imgWidth, imgWidth, ITEM_ZZ);

		//右转
		DrawNormalItem(graphics, x + 8 * splitWidth, y, imgWidth, imgWidth, ITEM_YZ);

		//人行横道
		DrawNormalItem(graphics, x + 9 * splitWidth, y, imgWidth, imgWidth, ITEM_RXHD);

		//学校区域
		DrawNormalItem(graphics, x + 10 * splitWidth, y, imgWidth, imgWidth, ITEM_XXQY);

		//公交车站
		DrawNormalItem(graphics, x + 11 * splitWidth, y, imgWidth, imgWidth, ITEM_GJCZ);

		//会车
		DrawNormalItem(graphics, x + 12 * splitWidth, y, imgWidth, imgWidth, ITEM_HC);

		//超车
		DrawNormalItem(graphics, x + 13 * splitWidth, y, imgWidth, imgWidth, ITEM_CC);

		//掉头
		DrawNormalItem(graphics, x + 14 * splitWidth, y, imgWidth, imgWidth, ITEM_DT);

		//夜间
		DrawNormalItem(graphics, x + 15 * splitWidth, y, imgWidth, imgWidth, ITEM_YJ);
	}
	catch (...)
	{
		L_ERROR(_T("DrawItemBackground catch an error.\n"));
	}
}

void CStudentInfoReflesh::DrawNormalItem(Graphics *graphics, int x, int y, int width, int height, wstring wsItem)
{
	wstring wsPath = m_wsProgramPath + FILE_PATH_ITEM + wsItem + _T("_Normal.jpg");
	if (!CWinUtils::FileExists(wsPath))
	{
		L_ERROR(_T("file not exist : %s\n"), wsPath.c_str());
		return;
	}

	Image *imgItem = Image::FromFile(wsPath.c_str());
	graphics->DrawImage(imgItem, Rect(x, y, width, height));
	delete imgItem;
}

void CStudentInfoReflesh::DrawStudentInfo(Graphics *graphics)
{
	try
	{
		//绘制考生照片
		DrawPhoto(graphics);

		Gdiplus::Font myFont(FONT_MICROSOFT_YAHEI, 18, FontStyleRegular, UnitPoint);
		StringFormat format;
		format.SetAlignment(StringAlignmentNear);
		SolidBrush blackBrush(Color(0, 0, 0));

		int x = 155;
		int y = 2;
		int splitHeight = 28;
		int width = 500;

		//绘制考生信息
		wstring wsInfo = m_studentInfo.wsCarNo + _T("-") + m_studentInfo.wsCarType + _T("-") + m_studentInfo.wsRemarks;
		wstring wsName = _T("姓名：") + m_studentInfo.wsName;
		wstring wsGender = _T("性别：") + m_studentInfo.wsGender;
		wstring wsDate = _T("考试日期：") + m_studentInfo.wsDate;
		wstring wsSerialNo = _T("业务流水：") + m_studentInfo.wsSerialNo;
		wstring wsID = _T("身份证号：") + m_studentInfo.wsID;
		wstring wsDrivingSchool = _T("培训机构：") + m_studentInfo.wsDrivingSchool;
		wstring wsExamReason = _T("考试原因：") + m_studentInfo.wsExamReason;
		wstring wsExaminer = _T("考试员：") + m_studentInfo.wsExaminer;
		wstring wsDayCount = _T("考试次数：第 ") + m_studentInfo.wsDayCount + _T(" 次");
		graphics->DrawString(wsInfo.c_str(), wsInfo.length(), &myFont,
			RectF(x, y + 0 * splitHeight, width, splitHeight), &format, &blackBrush);
		graphics->DrawString(wsName.c_str(), wsName.length(), &myFont,
			RectF(x, y + 1 * splitHeight, width, splitHeight), &format, &blackBrush);
		graphics->DrawString(wsGender.c_str(), wsGender.length(), &myFont,
			RectF(x, y + 2 * splitHeight, width, splitHeight), &format, &blackBrush);
		graphics->DrawString(wsDate.c_str(), wsDate.length(), &myFont,
			RectF(x, y + 3 * splitHeight, width, splitHeight), &format, &blackBrush);
		graphics->DrawString(wsSerialNo.c_str(), wsSerialNo.length(), &myFont,
			RectF(x, y + 4 * splitHeight, width, splitHeight), &format, &blackBrush);
		graphics->DrawString(wsID.c_str(), wsID.length(), &myFont,
			RectF(x, y + 5 * splitHeight, width, splitHeight), &format, &blackBrush);
		graphics->DrawString(wsDrivingSchool.c_str(), wsDrivingSchool.length(), &myFont,
			RectF(x, y + 6 * splitHeight, width, splitHeight), &format, &blackBrush);
		graphics->DrawString(wsExamReason.c_str(), wsExamReason.length(), &myFont,
			RectF(x, y + 7 * splitHeight, width, splitHeight), &format, &blackBrush);
		graphics->DrawString(wsExaminer.c_str(), wsExaminer.length(), &myFont,
			RectF(x, y + 8 * splitHeight, width, splitHeight), &format, &blackBrush);
		graphics->DrawString(wsDayCount.c_str(), wsDayCount.length(), &myFont,
			RectF(x, y + 9 * splitHeight, width, splitHeight), &format, &blackBrush);
		


		
		//RectF layoutRect(200.0f, 220.0f, 200.0f, 50.0f);
		//
		//graphics->DrawString(m_studentInfo.wsName.c_str(), m_studentInfo.wsName.length(), &myFont,
		//	layoutRect, &format, &blackBrush);


		//m_DC.DrawText(m_studentInfo.wsName.c_str(), CRect(200, 220, 200, 50), DT_RIGHT | DT_SINGLELINE | DT_VCENTER);
	}
	catch (...)
	{
		L_ERROR(_T("DrawStudentInfo catch an error.\n"));
	}
}

void CStudentInfoReflesh::DrawJudgement(Graphics *graphics)
{
	try
	{
		Gdiplus::Font myFont(FONT_MICROSOFT_YAHEI, 15, FontStyleRegular, UnitPoint);
		StringFormat format;
		format.SetAlignment(StringAlignmentNear);
		SolidBrush blackBrush(Color(0, 0, 0));

		int x = 605;
		int y = 150;
		int splitHeight = 26;
		int width = 600;

		for (int i = 0; i < m_mapJudgeInfos.size(); i++)
		{
			ERROR_DATA judgeInfo = m_mapJudgeInfos[i];
			
			int yDisplay = y + i * splitHeight;
			wstring wsNo = CStringUtils::Format(_T("%d"), i + 1);
			wstring wsDeduction = CStringUtils::Format(_T("%d"), judgeInfo.ikcfs);
			string sDes = CStringUtils::Format("%s", judgeInfo.errorlx);
			wstring wsDes = _T("");
			CStringUtils::ASCII2Unicode(sDes, wsDes);

			if (!wsNo.empty() && !wsDes.empty() && !wsDeduction.empty())
			{
				graphics->DrawString(wsNo.c_str(), wsNo.length(), &myFont,
					RectF(x, yDisplay, width, splitHeight), &format, &blackBrush);
				graphics->DrawString(wsDes.c_str(), wsDes.length(), &myFont,
					RectF(x + 55, yDisplay, width, splitHeight), &format, &blackBrush);
				graphics->DrawString(wsDeduction.c_str(), wsDeduction.length(), &myFont,
					RectF(x + 590, yDisplay, width, splitHeight), &format, &blackBrush);
			}
		}
	}
	catch (...)
	{
		L_ERROR(_T("DrawJudgement catch an error.\n"));
	}
}

void CStudentInfoReflesh::DrawPhoto(Graphics *graphics)
{
	try
	{
		wstring wsPhotoName = m_wsProgramPath + FILE_PATH_PHOTO + CStringUtils::Format(PHOTO_ID_FORMAT, m_nCarNo);
		if (CWinUtils::FileExists(wsPhotoName))
		{
			Image *imgPhoto = Image::FromFile(wsPhotoName.c_str());
			int width = imgPhoto->GetWidth();
			int height = imgPhoto->GetHeight();
			graphics->DrawImage(imgPhoto, Rect(5, 5, 140, 140), 0, 0, width, height, UnitPixel);

			delete imgPhoto;
		}
		else
		{
			L_TRACE(_T("file not exist : %s\n"), wsPhotoName.c_str());
		}

		wstring wsLoginPhotoName = m_wsProgramPath + FILE_PATH_PHOTO + 
			CStringUtils::Format(PHOTO_LOGIN_FORMAT, m_nCarNo);
		if (CWinUtils::FileExists(wsLoginPhotoName))
		{
			Image *imgLoginPhoto = Image::FromFile(wsLoginPhotoName.c_str());
			int width = imgLoginPhoto->GetWidth();
			int height = imgLoginPhoto->GetHeight();
			graphics->DrawImage(imgLoginPhoto, Rect(5, 148, 140, 140), 0, 0, width, height, UnitPixel);

			delete imgLoginPhoto;
		}
		else
		{
			L_TRACE(_T("file not exist : %s\n"), wsPhotoName.c_str());
		}
	}
	catch (...)
	{
		L_ERROR(_T("DrawPhoto catch an error.\n"));
	}
}

void CStudentInfoReflesh::DrawSignal(Graphics *graphics)
{
	try
	{
		EnterCriticalSection(&m_carSignalLock);
		CarSignal carSignal = m_carSignal;
		LeaveCriticalSection(&m_carSignalLock);

		//档位信号
		wstring wsGearPath = m_wsProgramPath + CStringUtils::Format(IMG_PATH_GEAR_FORMAT, m_carSignal.dw);
		if (!CWinUtils::FileExists(wsGearPath))
		{	
			L_ERROR(_T("file not exist : %s\n"), wsGearPath.c_str());
		}
		Image *imgGear = Image::FromFile(wsGearPath.c_str());
		graphics->DrawImage(imgGear, Rect(1130, 0, 156, 132));
		delete imgGear;

		wstring wsSignalPath = m_wsProgramPath + IMG_PATH_SIGNAL;
		if (!CWinUtils::FileExists(wsSignalPath))
		{
			L_ERROR(_T("file not exist : %s\n"), wsSignalPath.c_str());
			return;
		}
		Image *imgSignal = Image::FromFile(wsSignalPath.c_str());

		int x = 700;
		int y = 13;
		int splitSource = 40;
		int splitDest = 55;

		int xDest = 0;
		int yDest = y;
		int xSource = 0;
		int ySource = 0;

		//安全带
		xDest = x + 0 * splitDest;
		xSource = 0 * splitSource;
		ySource = carSignal.aqd * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//脚刹
		xDest = x + 1 * splitDest;
		xSource = 12 * splitSource;
		ySource = carSignal.js * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//手刹
		xDest = x + 2 * splitDest;
		xSource = 9 * splitSource;
		ySource = carSignal.ss * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//副刹
		xDest = x + 3 * splitDest;
		xSource = 13 * splitSource;
		ySource = carSignal.fs * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//喇叭
		xDest = x + 4 * splitDest;
		xSource = 7 * splitSource;
		ySource = carSignal.lb * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//左转向
		xDest = x + 5 * splitDest;
		xSource = 1 * splitSource;
		ySource = carSignal.zzx * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//右转向
		xDest = x + 6 * splitDest;
		xSource = 2 * splitSource;
		ySource = carSignal.yzx * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//远光灯
		xDest = x + 7 * splitDest;
		xSource = 4 * splitSource;
		ySource = carSignal.ygd* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//换一行
		yDest += splitDest;

		//近光灯
		xDest = x + 0 * splitDest;
		xSource = 3 * splitSource;
		ySource = carSignal.jgd* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//熄火
		xDest = x + 1 * splitDest;
		xSource = 10 * splitSource;
		ySource = carSignal.xh* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//开关门
		xDest = x + 2 * splitDest;
		xSource = 6 * splitSource;
		ySource = carSignal.kgm* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//离合
		xDest = x + 3 * splitDest;
		xSource = 16 * splitSource;
		ySource = carSignal.kgm* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//警示灯
		xDest = x + 4 * splitDest;
		xSource = 25 * splitSource;
		ySource = carSignal.jsd * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//雨刮
		xDest = x + 5 * splitDest;
		xSource = 26 * splitSource;
		ySource = carSignal.yg * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//雾灯
		xDest = x + 6 * splitDest;
		xSource = 27 * splitSource;
		ySource = carSignal.wd * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//示廓灯
		xDest = x + 7 * splitDest;
		xSource = 14 * splitSource;
		ySource = carSignal.skd * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		delete imgSignal;
	}
	catch (...)
	{
		L_ERROR(_T("DrawSignal catch an error.\n"));
	}
}

void CStudentInfoReflesh::DrawScore(Graphics *graphics)
{
	try
	{
		Gdiplus::Font myFont(FONT_MICROSOFT_YAHEI, 50, FontStyleRegular, UnitPoint);
		StringFormat format;
		format.SetAlignment(StringAlignmentFar);	//靠右对齐
		SolidBrush blackBrush(Color(0, 0, 200));

		wstring wsScore = CStringUtils::Format(_T("%d"), m_nCurrentScore);
		graphics->DrawString(wsScore.c_str(), wsScore.length(), &myFont,
			RectF(440, 5, 150, 150), &format, &blackBrush);

		Gdiplus::Font fontSmall(FONT_MICROSOFT_YAHEI, 25, FontStyleRegular, UnitPoint);
		format.SetAlignment(StringAlignmentNear);	//靠左对齐
		wstring wsDes = _T("分");
		graphics->DrawString(wsDes.c_str(), wsDes.length(), &fontSmall,
			RectF(580, 40, 200, 200), &format, &blackBrush);
	}
	catch (...)
	{
		L_ERROR(_T("DrawScore catch an error.\n"));
	}
}