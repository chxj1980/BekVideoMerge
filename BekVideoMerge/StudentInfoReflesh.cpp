#include "stdafx.h"
#include "StudentInfoReflesh.h"

#define MODULE_NAME	_T("CStudentInfoReflesh")

CStudentInfoReflesh::CStudentInfoReflesh()
{
	m_refleshEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_bStartExam = false;
	m_bEndExam = false;
	m_bPass = false;
	m_nDisplayDelays = 0;

	SetEvent(m_refleshEvent);
}

CStudentInfoReflesh::~CStudentInfoReflesh()
{
	if (NULL != this->m_studentInfoRefleshThread && this->m_studentInfoRefleshThread->IsMainThreadRunning())
	{
		this->m_studentInfoRefleshThread->StopMainThread();
		delete this->m_studentInfoRefleshThread;
	}
}

void CStudentInfoReflesh::StartWork()
{
	m_studentInfoRefleshThread = CreateIThreadInstance(StudentInfoRefleshThreadProc, LPVOID(this));
	m_studentInfoRefleshThread->StartMainThread();
}

void CStudentInfoReflesh::Handle17C51(StudentInfo studentInfo)
{
	m_studentInfo = studentInfo;

	//CLSID pngClsid;
	//GetEncoderClsid(L"image/png", &pngClsid);
	//m_studentInfo.imgPhoto->Save(L"D:\\1d.png", &pngClsid, NULL);

	m_bStartExam = true;
	m_bEndExam = false;
	m_nDisplayDelays = 0;

	SetEvent(m_refleshEvent);
}

void CStudentInfoReflesh::Handle17C56(bool bPass)
{
	m_bPass = bPass;

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
				//graphics.Clear(RGB(255, 255, 255));
				
				//绘制背景
				studentInfoRefleshClass->DrawBackground(&graphics);

				if (studentInfoRefleshClass->m_bStartExam || studentInfoRefleshClass->m_nDisplayDelays > 0)	//考试中才展示这些信息
				{
					//绘制考生信息
					studentInfoRefleshClass->DrawStudentInfo(&graphics);


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

void CStudentInfoReflesh::DrawStudentInfo(Graphics *graphics)
{
	try
	{
		//绘制考生照片
		DrawPhoto(graphics);

		Gdiplus::Font myFont(L"微软雅黑", 18, FontStyleRegular, UnitPoint);
		StringFormat format;
		//format.SetAlignment(StringAlignmentCenter);
		format.SetAlignment(StringAlignmentNear);
		SolidBrush blackBrush(Color(0, 0, 0));
		//SolidBrush blackBrush(Color(255, 255, 255));

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