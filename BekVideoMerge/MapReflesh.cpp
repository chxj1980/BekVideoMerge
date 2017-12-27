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
	EnterCriticalSection(&this->m_carSignalLock);
	m_carSignal = signal;
	LeaveCriticalSection(&this->m_carSignalLock);
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
		DRAFT_QUALITY, VARIABLE_PITCH | FF_SWISS, _T("宋体"));
	mapRefleshClass->m_DC.SetBkMode(TRANSPARENT);	//透明

	wstring wsImgPathMap = mapRefleshClass->m_wsProgramPath + IMG_PATH_XMP_MARK;
	imgMap = Image::FromFile(wsImgPathMap.c_str());

	while (true)
	{
		DWORD dwRet = WaitForSingleObject(mapRefleshClass->m_refleshEvent, INFINITE);
		if (WAIT_OBJECT_0 == dwRet)
		{
			EnterCriticalSection(&mapRefleshClass->m_carSignalLock);
			CarSignal carSignal = mapRefleshClass->m_carSignal;
			LeaveCriticalSection(&mapRefleshClass->m_carSignalLock);

			//获取车辆实时位置
			int nCarRelativeX = 0;
			int nCarRelativeY = 0;
			mapRefleshClass->GetCarRelativeCoordinate(carSignal, nCarRelativeX, nCarRelativeY);

			//绘制地图
			mapRefleshClass->DrawMap(&graphics, nCarRelativeX, nCarRelativeY);

			
			//graphics.DrawImage(imgMap, Rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT));	//遮罩

			//刷新四合一界面
			mapRefleshClass->Reflesh();

			Sleep(1000);
		}

	}

EXIT:
	font.DeleteObject();
	L_INFO(_T("MapRefleshThreadProc Exit\n"));
	return TRUE;
}

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
		graphics->DrawImage(&bm, Rect(0, 0, VIDEO_WIDTH, VIDEO_HEIGHT), 
			(relativeX - VIDEO_WIDTH / 2), (relativeY - VIDEO_HEIGHT / 2), VIDEO_WIDTH, VIDEO_HEIGHT, UnitPixel);

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
