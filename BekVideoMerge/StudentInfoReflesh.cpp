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

	//�������Լ�����
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
	if (nIndex < 5)	//���չʾ5���۷���Ϣ
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
		DRAFT_QUALITY, DEFAULT_PITCH | FF_SWISS, L"����");
	studentInfoRefleshClass->m_DC.SelectObject(&fontz);
	studentInfoRefleshClass->m_DC.SetBkMode(TRANSPARENT);//͸��
	studentInfoRefleshClass->m_DC.SetTextColor(RGB(255, 255, 255));

	while (true)
	{
		DWORD dwRet = WaitForSingleObject(studentInfoRefleshClass->m_refleshEvent, INFINITE);
		if (WAIT_OBJECT_0 == dwRet)
		{
			if (studentInfoRefleshClass->m_bStartExam || !studentInfoRefleshClass->m_bEndExam || studentInfoRefleshClass->m_nDisplayDelays > 0)
			{				
				//���Ʊ���
				studentInfoRefleshClass->DrawBackground(&graphics);
				studentInfoRefleshClass->DrawItemBackground(&graphics);

				if (studentInfoRefleshClass->m_bStartExam || studentInfoRefleshClass->m_nDisplayDelays > 0)	//�����в�չʾ��Щ��Ϣ
				{
					//���ƿ���������Ϣ
					studentInfoRefleshClass->DrawStudentInfo(&graphics);

					//���ƿ۷���Ϣ
					studentInfoRefleshClass->DrawJudgement(&graphics);

					//����ʵʱ�ź�
					studentInfoRefleshClass->DrawSignal(&graphics);

					//���Ƴɼ�
					studentInfoRefleshClass->DrawScore(&graphics);
				}

				//ˢ���ĺ�һ����
				studentInfoRefleshClass->Reflesh();

				Sleep(500);

				//���Խ����󣬼�����ʾһ��ʱ��
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

//���Ʊ���
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

//������Ŀ�Ʊ���
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

		//�ϳ�׼��
		DrawNormalItem(graphics, x + 0 * splitWidth, y, imgWidth, imgWidth, ITEM_SCZB);
		
		//��
		DrawNormalItem(graphics, x + 1 * splitWidth, y, imgWidth, imgWidth, ITEM_QB);

		//ֱ����ʻ
		DrawNormalItem(graphics, x + 2 * splitWidth, y, imgWidth, imgWidth, ITEM_ZXXS);

		//�Ӽ���
		DrawNormalItem(graphics, x + 3 * splitWidth, y, imgWidth, imgWidth, ITEM_JJD);

		//�������
		DrawNormalItem(graphics, x + 4 * splitWidth, y, imgWidth, imgWidth, ITEM_BGCD);
		
		//����ͣ��
		DrawNormalItem(graphics, x + 5 * splitWidth, y, imgWidth, imgWidth, ITEM_KBTC);

		//ֱ��ͨ��·��
		DrawNormalItem(graphics, x + 6 * splitWidth, y, imgWidth, imgWidth, ITEM_ZX);

		//��ת
		DrawNormalItem(graphics, x + 7 * splitWidth, y, imgWidth, imgWidth, ITEM_ZZ);

		//��ת
		DrawNormalItem(graphics, x + 8 * splitWidth, y, imgWidth, imgWidth, ITEM_YZ);

		//���к��
		DrawNormalItem(graphics, x + 9 * splitWidth, y, imgWidth, imgWidth, ITEM_RXHD);

		//ѧУ����
		DrawNormalItem(graphics, x + 10 * splitWidth, y, imgWidth, imgWidth, ITEM_XXQY);

		//������վ
		DrawNormalItem(graphics, x + 11 * splitWidth, y, imgWidth, imgWidth, ITEM_GJCZ);

		//�ᳵ
		DrawNormalItem(graphics, x + 12 * splitWidth, y, imgWidth, imgWidth, ITEM_HC);

		//����
		DrawNormalItem(graphics, x + 13 * splitWidth, y, imgWidth, imgWidth, ITEM_CC);

		//��ͷ
		DrawNormalItem(graphics, x + 14 * splitWidth, y, imgWidth, imgWidth, ITEM_DT);

		//ҹ��
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
		//���ƿ�����Ƭ
		DrawPhoto(graphics);

		Gdiplus::Font myFont(FONT_MICROSOFT_YAHEI, 18, FontStyleRegular, UnitPoint);
		StringFormat format;
		format.SetAlignment(StringAlignmentNear);
		SolidBrush blackBrush(Color(0, 0, 0));

		int x = 155;
		int y = 2;
		int splitHeight = 28;
		int width = 500;

		//���ƿ�����Ϣ
		wstring wsInfo = m_studentInfo.wsCarNo + _T("-") + m_studentInfo.wsCarType + _T("-") + m_studentInfo.wsRemarks;
		wstring wsName = _T("������") + m_studentInfo.wsName;
		wstring wsGender = _T("�Ա�") + m_studentInfo.wsGender;
		wstring wsDate = _T("�������ڣ�") + m_studentInfo.wsDate;
		wstring wsSerialNo = _T("ҵ����ˮ��") + m_studentInfo.wsSerialNo;
		wstring wsID = _T("���֤�ţ�") + m_studentInfo.wsID;
		wstring wsDrivingSchool = _T("��ѵ������") + m_studentInfo.wsDrivingSchool;
		wstring wsExamReason = _T("����ԭ��") + m_studentInfo.wsExamReason;
		wstring wsExaminer = _T("����Ա��") + m_studentInfo.wsExaminer;
		wstring wsDayCount = _T("���Դ������� ") + m_studentInfo.wsDayCount + _T(" ��");
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

		//��λ�ź�
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

		//��ȫ��
		xDest = x + 0 * splitDest;
		xSource = 0 * splitSource;
		ySource = carSignal.aqd * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//��ɲ
		xDest = x + 1 * splitDest;
		xSource = 12 * splitSource;
		ySource = carSignal.js * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//��ɲ
		xDest = x + 2 * splitDest;
		xSource = 9 * splitSource;
		ySource = carSignal.ss * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//��ɲ
		xDest = x + 3 * splitDest;
		xSource = 13 * splitSource;
		ySource = carSignal.fs * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//����
		xDest = x + 4 * splitDest;
		xSource = 7 * splitSource;
		ySource = carSignal.lb * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//��ת��
		xDest = x + 5 * splitDest;
		xSource = 1 * splitSource;
		ySource = carSignal.zzx * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//��ת��
		xDest = x + 6 * splitDest;
		xSource = 2 * splitSource;
		ySource = carSignal.yzx * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//Զ���
		xDest = x + 7 * splitDest;
		xSource = 4 * splitSource;
		ySource = carSignal.ygd* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//��һ��
		yDest += splitDest;

		//�����
		xDest = x + 0 * splitDest;
		xSource = 3 * splitSource;
		ySource = carSignal.jgd* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//Ϩ��
		xDest = x + 1 * splitDest;
		xSource = 10 * splitSource;
		ySource = carSignal.xh* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//������
		xDest = x + 2 * splitDest;
		xSource = 6 * splitSource;
		ySource = carSignal.kgm* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//���
		xDest = x + 3 * splitDest;
		xSource = 16 * splitSource;
		ySource = carSignal.kgm* splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//��ʾ��
		xDest = x + 4 * splitDest;
		xSource = 25 * splitSource;
		ySource = carSignal.jsd * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//���
		xDest = x + 5 * splitDest;
		xSource = 26 * splitSource;
		ySource = carSignal.yg * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//���
		xDest = x + 6 * splitDest;
		xSource = 27 * splitSource;
		ySource = carSignal.wd * splitSource;
		graphics->DrawImage(imgSignal, Rect(xDest, yDest, splitSource, splitSource),
			xSource, ySource, splitSource, splitSource, UnitPixel);

		//ʾ����
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
		format.SetAlignment(StringAlignmentFar);	//���Ҷ���
		SolidBrush blackBrush(Color(0, 0, 200));

		wstring wsScore = CStringUtils::Format(_T("%d"), m_nCurrentScore);
		graphics->DrawString(wsScore.c_str(), wsScore.length(), &myFont,
			RectF(440, 5, 150, 150), &format, &blackBrush);

		Gdiplus::Font fontSmall(FONT_MICROSOFT_YAHEI, 25, FontStyleRegular, UnitPoint);
		format.SetAlignment(StringAlignmentNear);	//�������
		wstring wsDes = _T("��");
		graphics->DrawString(wsDes.c_str(), wsDes.length(), &fontSmall,
			RectF(580, 40, 200, 200), &format, &blackBrush);
	}
	catch (...)
	{
		L_ERROR(_T("DrawScore catch an error.\n"));
	}
}