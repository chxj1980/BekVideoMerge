#include "stdafx.h"
#include "MapReflesh.h"

#define MODULE_NAME	_T("CStudentInfoReflesh")

CMapReflesh::CMapReflesh()
{
	InitializeCriticalSection(&this->m_carSignalLock);
	m_refleshEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

	m_bDrawCar = false;
	m_mapMaxX = 0;
	m_mapMinX = 0;
	m_mapMaxY = 0;
	m_mapMinY = 0;
	m_mapZoomIn = 0;
	m_mapWidth = 0;
	m_mapHeight = 0;
	m_mapSplitWidth = 0;

	m_bBigCar = false;
	m_nKskm = 2;

	m_bStartExam = true;
	m_wsExamStatus = _T("");
	m_nDisplayDelays = 0;
	m_nCurrentScore = EXAM_TOTAL_SCORE;
}

CMapReflesh::~CMapReflesh()
{
	if (NULL != this->m_mapRefleshThread && this->m_mapRefleshThread->IsMainThreadRunning())
	{
		this->m_mapRefleshThread->StopMainThread();
		delete this->m_mapRefleshThread;
	}

	CloseHandle(this->m_refleshEvent);

	//清理锁以及其他
	DeleteCriticalSection(&this->m_carSignalLock);
}

void CMapReflesh::StartWork()
{
	LoadMapConfig();
	
	m_mapRefleshThread = CreateIThreadInstance(MapRefleshThreadProc, LPVOID(this));
	m_mapRefleshThread->StartMainThread();
}

void CMapReflesh::LoadMapConfig()
{
	wstring wsEnvConfPath = m_wsProgramPath + CONF_PATH_ENV;
	wstring wsMapConfPath = m_wsProgramPath + CONF_PATH_MAP;

	//是否绘制考车
	int nDrawCar = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_DRAWCAR, 0, wsEnvConfPath.c_str());
	if (1 == nDrawCar)
	{
		m_bDrawCar = true;
	}

	//考试科目
	m_nKskm = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_KSKM, 2, wsEnvConfPath.c_str());

	//是否是大车程序
	int nBigCar = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_BIGCAR, 0, wsEnvConfPath.c_str());
	if (1 == nBigCar)
	{
		m_bBigCar = true;
	}

	m_mapMaxX = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_MAXX, 0, wsMapConfPath.c_str());
	m_mapMinX = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_MINX, 0, wsMapConfPath.c_str());
	m_mapMaxY = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_MAXY, 0, wsMapConfPath.c_str());
	m_mapMinY = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_MINY, 0, wsMapConfPath.c_str());
	m_mapZoomIn = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_ZOOMIN, 0, wsMapConfPath.c_str());
	m_mapSplitWidth = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_SPLITWIDTH, 0, wsMapConfPath.c_str());
	m_mapWidth = (m_mapMaxX - m_mapMinX) * m_mapZoomIn;
	m_mapHeight = (m_mapMaxY - m_mapMinY) * m_mapZoomIn;
	L_INFO(_T("LoadMapConfig m_mapMaxX=%d, m_mapMinX=%d, m_mapMaxY=%d, m_mapMinY=%d, m_mapZoomIn=%d, m_mapSplitWidth=%d\n"),
		m_mapMaxX, m_mapMinX, m_mapMaxY, m_mapMinY, m_mapZoomIn, m_mapSplitWidth);
}

bool CMapReflesh::GetCarRelativeCoordinate(CarSignal signal, int &x, int &y)
{
	x = abs((int)((signal.dX - m_mapMinX) * m_mapZoomIn));
	y = abs((int)((signal.dY - m_mapMaxY) * m_mapZoomIn));
	if (x < 0 || x > m_mapWidth)
	{
		L_INFO(_T("GetCarRelativeCoordinate fail, x=%d, mapMinX=%d, mapZoomIn=%d\n"),
			signal.dX, m_mapMinX, m_mapZoomIn);
		x = 0;
	}
	if (y < 0 || y > m_mapHeight)
	{
		L_INFO(_T("GetCarRelativeCoordinate fail, y=%d, mapMaxY=%d, mapZoomIn=%d\n"),
			signal.dY, m_mapMaxY, m_mapZoomIn);
		y = 0;
	}
	
	if (0 == x || 0 == y)
	{
		return false;
	}

	return true;
}

void CMapReflesh::SetCarSignal(CarSignal signal)
{
	if (m_bStartExam)
	{
		EnterCriticalSection(&this->m_carSignalLock);
		m_carSignal = signal;
		LeaveCriticalSection(&this->m_carSignalLock);
	}
}

void CMapReflesh::Handle17C51()
{
	m_bStartExam = true;
	m_wsExamStatus = _T("考试开始");
	m_startTime = CTime::GetCurrentTime();
	m_nDisplayDelays = 0;

	SetEvent(m_refleshEvent);
}

void CMapReflesh::Handle17C56(bool bPass, int nScore)
{
	m_bStartExam = false;	//考试结束
	m_bPass = bPass;	//考试是否通过
	m_nCurrentScore = nScore;	//考试得分
	m_nDisplayDelays = DISPLAY_DELAY_SECONDS; //延迟显示一段时间
	m_endTime = CTime::GetCurrentTime();

	if (m_bPass)
	{
		m_wsExamStatus = _T("考试合格");
	}
	else
	{
		m_wsExamStatus = _T("考试不合格");
	}
}

BOOL CMapReflesh::MapRefleshThreadProc(LPVOID parameter, HANDLE stopEvent)
{
	L_INFO(_T("MapRefleshThreadProc Start\n"));

	CMapReflesh *mapRefleshClass = (CMapReflesh*)parameter;

	Graphics graphics(mapRefleshClass->m_DC.GetSafeHdc());
	

	while (true)
	{
		DWORD dwRet = WaitForSingleObject(mapRefleshClass->m_refleshEvent, INFINITE);
		if (WAIT_OBJECT_0 == dwRet)
		{
			if (mapRefleshClass->m_bStartExam || mapRefleshClass->m_nDisplayDelays > 0)
			{
				//获取车辆实时位置
				int nCarRelativeX = 0;
				int nCarRelativeY = 0;
				EnterCriticalSection(&mapRefleshClass->m_carSignalLock);
				CarSignal carSignal = mapRefleshClass->m_carSignal;
				LeaveCriticalSection(&mapRefleshClass->m_carSignalLock);
				mapRefleshClass->GetCarRelativeCoordinate(carSignal, nCarRelativeX, nCarRelativeY);

				//绘制地图
				mapRefleshClass->DrawMap(&graphics, nCarRelativeX, nCarRelativeY);

				//绘制背景，叠加在地图上
				mapRefleshClass->DrawBackground(&graphics);

				//绘制项目实时状态信息
				mapRefleshClass->DrawStatus(carSignal);

				//刷新四合一界面
				mapRefleshClass->Reflesh();

				Sleep(1000);

				//考试结束后，继续显示一段时间
				if (mapRefleshClass->m_nDisplayDelays > 0)
				{
					mapRefleshClass->m_nDisplayDelays -= 1;
				}

				SetEvent(mapRefleshClass->m_refleshEvent);
			}
		}

	}

EXIT:
	L_INFO(_T("MapRefleshThreadProc Exit\n"));
	return TRUE;
}

//绘制背景
void CMapReflesh::DrawBackground(Graphics *graphics)
{
	try
	{
		wstring wsXMBackground = m_wsProgramPath + IMG_PATH_XM_BACKGROUND;
		if (!CWinUtils::FileExists(wsXMBackground))
		{
			L_ERROR(_T("wsXMBackground not exist, file name = %s\n"), wsXMBackground.c_str());
			return;
		}
		wstring wsXMList = m_wsProgramPath + IMG_PATH_XM_LIST;
		if (!CWinUtils::FileExists(wsXMList))
		{
			L_ERROR(_T("wsXMList not exist, file name = %s\n"), wsXMList.c_str());
			return;
		}
		wstring wsMapBackground = m_wsProgramPath + IMG_PATH_MAP_BACKGROUND;
		if (!CWinUtils::FileExists(wsMapBackground))
		{
			L_ERROR(_T("wsMapBackground not exist, file name = %s\n"), wsMapBackground.c_str());
			return;
		}

		//项目牌背景
		Image *imgXMBackground = Image::FromFile(wsXMBackground.c_str());	
		graphics->DrawImage(imgXMBackground, Rect(264, 0, 88, 288), 264, 0, 88, 288, UnitPixel);

		//项目牌列表
		Image *imgXMList = Image::FromFile(wsXMList.c_str());
		if (KSKM_3 == m_nKskm || m_bBigCar)
		{
			graphics->DrawImage(imgXMList, Rect(264, 0, 88, 288), 0, 0, 88, 288, UnitPixel);
		}
		else
		{
			graphics->DrawImage(imgXMList, Rect(264, 36, 88, 252), 0, 0, 88, 252, UnitPixel);
		}

		//地图背景
		Image *imgMapBackground = Image::FromFile(wsMapBackground.c_str());	
		graphics->DrawImage(imgMapBackground, Rect(0, 0, VIDEO_WIDTH - 88, VIDEO_HEIGHT));

		delete imgXMBackground;
		delete imgXMList;
		delete imgMapBackground;
	}
	catch (...)
	{
		L_ERROR(_T("DrawBackground catch an error.\n"));
	}
}

//绘制地图
void CMapReflesh::DrawMap(Graphics *graphics, int carX, int carY)
{
	try
	{
		int rowNum = carY / m_mapSplitWidth;
		int columnNum = carX / m_mapSplitWidth;
		L_DEBUG(_T("carNo=%d, carX=%d, carY=%d, rowNum=%d, colunmNum=%d\n"), m_nCarNo, carX, carY, rowNum, columnNum);

		if (0 == rowNum && 0 == columnNum)
		{
			L_DEBUG(_T("maybe error, rowNum=%d, columnNum=%d\n"), rowNum, columnNum);
			rowNum = 1;
			columnNum = 1;
			carX += m_mapSplitWidth;
			carY += m_mapSplitWidth;
		}

		//加载考车周围的9张地图
		wstring wsFileLeftTop = m_wsProgramPath + CStringUtils::Format(MAP_FILENAME_FORMAT, rowNum - 1, columnNum - 1);
		wstring wsFileTop = m_wsProgramPath + CStringUtils::Format(MAP_FILENAME_FORMAT, rowNum - 1, columnNum);
		wstring wsFileRightTop = m_wsProgramPath + CStringUtils::Format(MAP_FILENAME_FORMAT, rowNum - 1, columnNum + 1);
		wstring wsFileLeftMiddle = m_wsProgramPath + CStringUtils::Format(MAP_FILENAME_FORMAT, rowNum, columnNum - 1);
		wstring wsFileMiddle = m_wsProgramPath + CStringUtils::Format(MAP_FILENAME_FORMAT, rowNum, columnNum);
		wstring wsFileRightMiddle = m_wsProgramPath + CStringUtils::Format(MAP_FILENAME_FORMAT, rowNum, columnNum + 1);
		wstring wsFileLeftBottom = m_wsProgramPath + CStringUtils::Format(MAP_FILENAME_FORMAT, rowNum + 1, columnNum - 1);
		wstring wsFileBottom = m_wsProgramPath + CStringUtils::Format(MAP_FILENAME_FORMAT, rowNum + 1, columnNum);
		wstring wsFileRightBottom = m_wsProgramPath + CStringUtils::Format(MAP_FILENAME_FORMAT, rowNum + 1, columnNum + 1);
		Image * imgLeftTop = Image::FromFile(wsFileLeftTop.c_str());
		Image * imgTop = Image::FromFile(wsFileTop.c_str());
		Image * imgRightTop = Image::FromFile(wsFileRightTop.c_str());
		Image * imgLeftMiddle = Image::FromFile(wsFileLeftMiddle.c_str());
		Image * imgMiddle = Image::FromFile(wsFileMiddle.c_str());
		Image * imgRightMiddle = Image::FromFile(wsFileRightMiddle.c_str());
		Image * imgLeftBottom = Image::FromFile(wsFileLeftBottom.c_str());
		Image * imgBottom = Image::FromFile(wsFileBottom.c_str());
		Image * imgRightBottom = Image::FromFile(wsFileRightBottom.c_str());

		//生成一张临时的大图
		Bitmap bm(3 * m_mapSplitWidth, 3 * m_mapSplitWidth);
		Graphics *gNineMaps;
		gNineMaps = Graphics::FromImage(&bm);
		gNineMaps->DrawImage(imgLeftTop, Rect(0, 0, m_mapSplitWidth, m_mapSplitWidth));
		gNineMaps->DrawImage(imgTop, Rect(m_mapSplitWidth, 0, m_mapSplitWidth, m_mapSplitWidth));
		gNineMaps->DrawImage(imgRightTop, Rect(2 * m_mapSplitWidth, 0, m_mapSplitWidth, m_mapSplitWidth));
		gNineMaps->DrawImage(imgLeftMiddle, Rect(0, m_mapSplitWidth, m_mapSplitWidth, m_mapSplitWidth));
		gNineMaps->DrawImage(imgMiddle, Rect(m_mapSplitWidth, m_mapSplitWidth, m_mapSplitWidth, m_mapSplitWidth));
		gNineMaps->DrawImage(imgRightMiddle, Rect(2 * m_mapSplitWidth, m_mapSplitWidth, m_mapSplitWidth, m_mapSplitWidth));
		gNineMaps->DrawImage(imgLeftBottom, Rect(0, 2 * m_mapSplitWidth, m_mapSplitWidth, m_mapSplitWidth));
		gNineMaps->DrawImage(imgBottom, Rect(m_mapSplitWidth, 2 * m_mapSplitWidth, m_mapSplitWidth, m_mapSplitWidth));
		gNineMaps->DrawImage(imgRightBottom, Rect(2 * m_mapSplitWidth, 2 * m_mapSplitWidth, m_mapSplitWidth, m_mapSplitWidth));

		//绘制实时地图
		int relativeX = carX % m_mapSplitWidth + m_mapSplitWidth;
		int relativeY = carY % m_mapSplitWidth + m_mapSplitWidth;
		L_DEBUG(_T("car relative cordirate in ninemaps, x=%d, y=%d\n"), relativeX, relativeY);
		graphics->DrawImage(&bm, Rect(0, 0, VIDEO_WIDTH - 88, VIDEO_HEIGHT), 
			(relativeX - (VIDEO_WIDTH - 88) / 2), (relativeY - VIDEO_HEIGHT / 2), VIDEO_WIDTH - 88, VIDEO_HEIGHT, UnitPixel);

		delete imgLeftTop;
		delete imgTop;
		delete imgRightTop;
		delete imgLeftMiddle;
		delete imgMiddle;
		delete imgRightMiddle;
		delete imgLeftBottom;
		delete imgBottom;
		delete imgRightBottom;
		
	}
	catch (...)
	{
		L_ERROR(_T("DrawMap catch an error.\n"));
	}

}

//绘制状态信息
void CMapReflesh::DrawStatus(CarSignal carSignal)
{
	try
	{
		CFont font;
		font.CreateFont(20, 0, 0, 0, FW_BOLD, TRUE, FALSE, 0, ANSI_CHARSET,
			OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
			DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, _T("微软雅黑"));
		m_DC.SetBkMode(TRANSPARENT);	//透明
		m_DC.SetTextColor(RGB(255, 255, 255));

		//项目状态信息，显示在title
		m_DC.DrawText(m_wsExamStatus.c_str(), CRect(0, 0, VIDEO_WIDTH - 88, 30), DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		//速度
		wstring wsSpeed = CStringUtils::Format(_T("%4.1f km/h"), carSignal.fSpeed);
		m_DC.DrawText(wsSpeed.c_str(), CRect(0, 236, 73, 262), DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

		//里程
		wstring wsMileage = CStringUtils::Format(_T("%6.1f m"), carSignal.fMileage);
		m_DC.DrawText(wsMileage.c_str(), CRect(0, 262, 73, 288), DT_RIGHT | DT_SINGLELINE | DT_VCENTER);

		//成绩
		wstring wsScore = CStringUtils::Format(_T("成绩:%d"), m_nCurrentScore);
		m_DC.DrawText(wsScore.c_str(), CRect(198, 236, 264, 262), DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		//时间
		CTimeSpan span;
		if (m_bStartExam)
		{
			span = CTime::GetCurrentTime() - m_startTime;
		}
		else
		{
			span = m_endTime - m_startTime;
		}
		wstring wsTimeSpan = CStringUtils::Format(_T("%d%d:%d%d:%d%d"), span.GetHours() / 10, span.GetHours() % 10,
			span.GetMinutes() / 10, span.GetMinutes() % 10, span.GetSeconds() / 10, span.GetSeconds() % 10);
		m_DC.DrawText(wsTimeSpan.c_str(), CRect(198, 262, 264, 288), DT_LEFT | DT_SINGLELINE | DT_VCENTER);

		font.DeleteObject();
	}
	catch (...)
	{
		L_ERROR(_T("DrawStatus catch an error.\n"));
	}
}
