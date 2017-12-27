#include "stdafx.h"
#include "StudentInfoReflesh.h"

#define MODULE_NAME	_T("CStudentInfoReflesh")

CStudentInfoReflesh::CStudentInfoReflesh()
{
	
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



BOOL CStudentInfoReflesh::StudentInfoRefleshThreadProc(LPVOID parameter, HANDLE stopEvent)
{
	L_INFO(_T("StudentInfoRefleshThreadProc Start\n"));

	CStudentInfoReflesh *studentInfoRefleshClass = (CStudentInfoReflesh*)parameter;

	Graphics graphics(studentInfoRefleshClass->m_DC.GetSafeHdc());
	CFont font;
	Image *imgBk;
	font.CreateFontW(20, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
		DRAFT_QUALITY, VARIABLE_PITCH | FF_SWISS, _T("宋体"));
	studentInfoRefleshClass->m_DC.SetBkMode(TRANSPARENT);	//透明

	wstring wsImgPathTbk = studentInfoRefleshClass->m_wsProgramPath + IMG_PATH_TBK;
	imgBk = Image::FromFile(wsImgPathTbk.c_str());

	while (true)
	{
		DWORD dwRet = WaitForSingleObject(stopEvent, 1000);
		switch (dwRet)
		{
		case WAIT_TIMEOUT:
		{
			graphics.DrawImage(imgBk, Rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT));	//遮罩

			//刷新四合一界面
			studentInfoRefleshClass->Reflesh();

			Sleep(1000);

			break;
		}

		default:
			goto EXIT;
		}
	}

EXIT:
	L_INFO(_T("StudentInfoRefleshThreadProc Exit\n"));
	return TRUE;
}