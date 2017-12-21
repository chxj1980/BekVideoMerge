#include "stdafx.h"
#include "MapReflesh.h"

#define MODULE_NAME	_T("CStudentInfoReflesh")

CMapReflesh::CMapReflesh()
{
}

CMapReflesh::~CMapReflesh()
{
	if (NULL != this->m_mapRefleshThread && this->m_mapRefleshThread->IsMainThreadRunning())
	{
		this->m_mapRefleshThread->StopMainThread();
		delete this->m_mapRefleshThread;
	}
}

void CMapReflesh::StartWork()
{
	m_mapRefleshThread = CreateIThreadInstance(MapRefleshThreadProc, LPVOID(this));
	m_mapRefleshThread->StartMainThread();
}

BOOL CMapReflesh::MapRefleshThreadProc(LPVOID parameter, HANDLE stopEvent)
{
	L_INFO(_T("MapRefleshThreadProc Start\n"));

	CMapReflesh *mapRefleshClass = (CMapReflesh*)parameter;

	Graphics graphics(mapRefleshClass->m_DC.GetSafeHdc());
	CFont font;
	Image *imgMap;
	font.CreateFontW(20, 0, 0, 0, FW_MEDIUM, FALSE, FALSE, 0, ANSI_CHARSET,
		OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS,
		DRAFT_QUALITY, VARIABLE_PITCH | FF_SWISS, _T("ËÎÌå"));
	mapRefleshClass->m_DC.SetBkMode(TRANSPARENT);	//Í¸Ã÷

	wstring wsImgPathMap = mapRefleshClass->m_wsProgramPath + IMG_PATH_XMP_MARK;
	imgMap = Image::FromFile(wsImgPathMap.c_str());

	while (true)
	{
		DWORD dwRet = WaitForSingleObject(stopEvent, 1000);
		switch (dwRet)
		{
		case WAIT_TIMEOUT:
		{
			graphics.DrawImage(imgMap, Rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT));	//ÕÚÕÖ

#ifdef __DEBUG_SAVE_PNG__
			mapRefleshClass->SavePngFile();
#else
			mapRefleshClass->SendDataToHikDevice();
#endif

			Sleep(1000);

			break;
		}

		default:
			goto EXIT;
		}
	}

EXIT:
	L_INFO(_T("MapRefleshThreadProc Exit\n"));
	return TRUE;
}
