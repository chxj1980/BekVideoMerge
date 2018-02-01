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

	m_bStartExam = false;
	m_bEndExam = false;
	m_wsExamStatus = _T("");
	m_nDisplayDelays = 0;

	m_bNineMaps = false;

	SetEvent(m_refleshEvent);
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

	int uNineMaps = GetPrivateProfileInt(CONF_SECTION_CONFIG, CONF_KEY_NINEMAPS, 0, wsEnvConfPath.c_str());
	if (1 == uNineMaps)
	{
		m_bNineMaps = true;
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
	m_bEndExam = false;
	m_wsExamStatus = _T("考试开始");
	m_startTime = CTime::GetCurrentTime();
	m_nDisplayDelays = 0;

	SetEvent(m_refleshEvent);
}

void CMapReflesh::Handle17C56(bool bPass)
{
	m_bStartExam = false;	//考试结束
	m_bEndExam = true;
	m_bPass = bPass;	//考试是否通过
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
	graphics.SetInterpolationMode(Gdiplus::InterpolationMode::InterpolationModeHighQuality);
	graphics.SetSmoothingMode(Gdiplus::SmoothingMode::SmoothingModeHighQuality);

	while (true)
	{
		DWORD dwRet = WaitForSingleObject(mapRefleshClass->m_refleshEvent, INFINITE);
		if (WAIT_OBJECT_0 == dwRet)
		{
			if (mapRefleshClass->m_bStartExam || !mapRefleshClass->m_bEndExam || mapRefleshClass->m_nDisplayDelays > 0)
			{
				//获取车辆实时位置
				int nCarRelativeX = 0;
				int nCarRelativeY = 0;
				EnterCriticalSection(&mapRefleshClass->m_carSignalLock);
				CarSignal carSignal = mapRefleshClass->m_carSignal;
				LeaveCriticalSection(&mapRefleshClass->m_carSignalLock);
				mapRefleshClass->GetCarRelativeCoordinate(carSignal, nCarRelativeX, nCarRelativeY);

				//绘制地图
				if (mapRefleshClass->m_bNineMaps)
				{
					mapRefleshClass->DrawNineMap(&graphics, nCarRelativeX, nCarRelativeY);
				}
				else
				{
					mapRefleshClass->DrawNormalMap(&graphics, nCarRelativeX, nCarRelativeY);
				}

				//绘制车模型
				mapRefleshClass->DrawCar(&graphics, carSignal.fDirectionAngle);

				if (mapRefleshClass->m_bStartExam || mapRefleshClass->m_nDisplayDelays > 0)	//考试中才展示这些信息
				{
					//绘制项目实时状态信息
					mapRefleshClass->DrawStatus(&graphics, carSignal);
				}

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

////绘制背景
//void CMapReflesh::DrawBackground(Graphics *graphics)
//{
//	try
//	{
//		wstring wsXMBackground = m_wsProgramPath + IMG_PATH_XM_BACKGROUND;
//		if (!CWinUtils::FileExists(wsXMBackground))
//		{
//			L_ERROR(_T("wsXMBackground not exist, file name = %s\n"), wsXMBackground.c_str());
//			return;
//		}
//		wstring wsXMList = m_wsProgramPath + IMG_PATH_XM_LIST;
//		if (!CWinUtils::FileExists(wsXMList))
//		{
//			L_ERROR(_T("wsXMList not exist, file name = %s\n"), wsXMList.c_str());
//			return;
//		}
//		wstring wsMapBackground = m_wsProgramPath + IMG_PATH_MAP_BACKGROUND;
//		if (!CWinUtils::FileExists(wsMapBackground))
//		{
//			L_ERROR(_T("wsMapBackground not exist, file name = %s\n"), wsMapBackground.c_str());
//			return;
//		}
//
//		////项目牌背景
//		//Image *imgXMBackground = Image::FromFile(wsXMBackground.c_str());	
//		//graphics->DrawImage(imgXMBackground, Rect(264, 0, 88, 288), 264, 0, 88, 288, UnitPixel);
//
//		////项目牌列表
//		//Image *imgXMList = Image::FromFile(wsXMList.c_str());
//		//if (KSKM_3 == m_nKskm || m_bBigCar)
//		//{
//		//	graphics->DrawImage(imgXMList, Rect(264, 0, 88, 288), 0, 0, 88, 288, UnitPixel);
//		//}
//		//else
//		//{
//		//	graphics->DrawImage(imgXMList, Rect(264, 36, 88, 252), 0, 0, 88, 252, UnitPixel);
//		//}
//
//		//地图背景
//		Image *imgMapBackground = Image::FromFile(wsMapBackground.c_str());	
//		graphics->DrawImage(imgMapBackground, Rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT));
//
//		delete imgMapBackground;
//	}
//	catch (...)
//	{
//		L_ERROR(_T("DrawBackground catch an error.\n"));
//	}
//}

//绘制地图
void CMapReflesh::DrawNineMap(Graphics *graphics, int carX, int carY)
{
	try
	{
		int rowNum = carY / m_mapSplitWidth;
		int columnNum = carX / m_mapSplitWidth;
		L_DEBUG(_T("carNo=%d, carX=%d, carY=%d, rowNum=%d, colunmNum=%d\n"), m_nCarNo, carX, carY, rowNum, columnNum);

		if (0 == rowNum && 0 == columnNum)
		{
			L_DEBUG(_T("maybe wrong, rowNum=%d, columnNum=%d\n"), rowNum, columnNum);
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
		graphics->DrawImage(&bm, Rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT), 
			(relativeX - (VIDEO_WIDTH) / 2), (relativeY - VIDEO_HEIGHT / 2), VIDEO_WIDTH, VIDEO_HEIGHT, UnitPixel);

		delete imgLeftTop;
		delete imgTop;
		delete imgRightTop;
		delete imgLeftMiddle;
		delete imgMiddle;
		delete imgRightMiddle;
		delete imgLeftBottom;
		delete imgBottom;
		delete imgRightBottom;
		delete gNineMaps;
	}
	catch (...)
	{
		L_ERROR(_T("DrawMap catch an error.\n"));
	}

}

void CMapReflesh::DrawNormalMap(Graphics *graphics, int carX, int carY)
{
	wstring wsImgMap = m_wsProgramPath + IMG_PATH_NORMAL_MAP;
	if (!CWinUtils::FileExists(wsImgMap))
	{
		L_ERROR(_T("wsImgMap not exist, file name = %s\n"), wsImgMap.c_str());
		return;
	}

	Image * imgMap = Image::FromFile(wsImgMap.c_str());

	if (carX < VIDEO_WIDTH / 2)
	{
		carX = VIDEO_WIDTH / 2;
	}
	if (carY < VIDEO_HEIGHT / 2)
	{
		carY = VIDEO_HEIGHT / 2;
	}
	graphics->DrawImage(imgMap, Rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT),
		(carX - (VIDEO_WIDTH) / 2), (carY - VIDEO_HEIGHT / 2), VIDEO_WIDTH, VIDEO_HEIGHT, UnitPixel);

	delete imgMap;
}

//绘制车模型
void CMapReflesh::DrawCar(Graphics *graphics, float angle)
{
	graphics->TranslateTransform(VIDEO_WIDTH / 2, VIDEO_HEIGHT / 2);
	graphics->RotateTransform(angle);
	graphics->TranslateTransform(-VIDEO_WIDTH / 2, -VIDEO_HEIGHT / 2);

	wstring wsImgCar = m_wsProgramPath + IMG_PATH_CAR_SKIN;
	if (!CWinUtils::FileExists(wsImgCar))
	{
		L_ERROR(_T("wsImgCar not exist, file name = %s\n"), wsImgCar.c_str());
		return;
	}

	Image *imgCar = Image::FromFile(wsImgCar.c_str());
	int carWidth = imgCar->GetWidth();
	int carHeight = imgCar->GetHeight();
	graphics->DrawImage(imgCar, Rect(VIDEO_WIDTH / 2 - 100, VIDEO_HEIGHT / 2 - 100, 200, 200), 
		carWidth / 2 - 100, carHeight / 2 - 100, 200, 200, UnitPixel);
	graphics->ResetTransform();

	delete imgCar;
}

//绘制状态信息
void CMapReflesh::DrawStatus(Graphics *graphics, CarSignal carSignal)
{
	try
	{
		//背景
		Gdiplus::SolidBrush brush(Gdiplus::Color(220, 220, 220));	//灰色
		graphics->FillEllipse(&brush, 10, 10, 70, 70);
		graphics->FillEllipse(&brush, 10, 100, 70, 70);

		Gdiplus::Font font(FONT_MICROSOFT_YAHEI, 10, FontStyleRegular, UnitPoint);
		StringFormat format;
		format.SetAlignment(StringAlignmentCenter);
		SolidBrush blackBrush(Color(0, 0, 200));

		//速度
		wstring wsSpeedCar = CStringUtils::Format(_T("%.1f"), carSignal.fSpeed);
		wstring wsUnitCar = _T("km/h");
		graphics->DrawString(wsSpeedCar.c_str(), wsSpeedCar.length(), &font,
			RectF(10, 25, 70, 35), &format, &blackBrush);
		graphics->DrawString(wsUnitCar.c_str(), wsUnitCar.length(), &font,
			RectF(10, 45, 70, 35), &format, &blackBrush);

		//转速
		wstring wsSpeedEngine = CStringUtils::Format(_T("%.1f"), carSignal.fSpeedEngine);
		wstring wsUnitEngine = _T("r/min");
		graphics->DrawString(wsSpeedEngine.c_str(), wsSpeedEngine.length(), &font,
			RectF(10, 115, 70, 35), &format, &blackBrush);
		graphics->DrawString(wsUnitEngine.c_str(), wsUnitEngine.length(), &font,
			RectF(10, 135, 70, 35), &format, &blackBrush);
	}
	catch (...)
	{
		L_ERROR(_T("DrawStatus catch an error.\n"));
	}
}
