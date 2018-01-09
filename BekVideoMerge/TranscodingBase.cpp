#include "stdafx.h"
#include "TranscodingBase.h"

#define MODULE_NAME	_T("CTranscodingBase")

CTranscodingBase::CTranscodingBase()
{
	m_nCarNo = -1;
	m_lpHandle = -1;

	opts[0] = &pCompressOption;
}

CTranscodingBase::~CTranscodingBase()
{
	if (m_bmpBackground.m_hObject  != NULL)
	{
		m_bmpBackground.DeleteObject();
	}

	if (NULL != m_pBmpInfoHeader)
	{
		delete m_pBmpInfoHeader;
	}
}

bool CTranscodingBase::Init(wstring path, int carNo, int lpHanble)
{
	try
	{
		m_wsProgramPath = path;
		m_nCarNo = carNo;
		m_lpHandle = lpHanble;

		m_mutexMakeAvi = CreateMutex(NULL, FALSE, NULL);	//创建互斥对象
		
		if (NULL == m_DC.CreateCompatibleDC(NULL))
		{
			L_ERROR(_T("CreateCompatibleDC return NULL\n"));
			return false;
		}
		m_bmpBackground.LoadBitmapW(MAKEINTRESOURCEW(IDB_BITMAP_BACKGROUND));
		if (NULL == m_bmpBackground.m_hObject)
		{
			L_ERROR(_T("LoadBitmapW return NULL\n"));
			return false;
		}
		m_DC.SelectObject(m_bmpBackground);

		m_pBmpInfoHeader = new BITMAPINFOHEADER;
		m_pBmpInfoHeader->biBitCount = 32;
		m_pBmpInfoHeader->biClrImportant = 0;
		m_pBmpInfoHeader->biCompression = 0;
		m_pBmpInfoHeader->biHeight = 288;
		m_pBmpInfoHeader->biPlanes = 1;
		m_pBmpInfoHeader->biSize = sizeof(BITMAPINFOHEADER);
		m_pBmpInfoHeader->biSizeImage = 405504;
		m_pBmpInfoHeader->biWidth = 352;
		m_pBmpInfoHeader->biXPelsPerMeter = 0;
		m_pBmpInfoHeader->biYPelsPerMeter = 0;

		m_bitmapInfo.bmiHeader = *m_pBmpInfoHeader;
		memset(&m_aviStreamInfo, 0, sizeof(m_aviStreamInfo));
		m_aviStreamInfo.fccType = streamtypeVIDEO;
		m_aviStreamInfo.fccHandler = 0;
		m_aviStreamInfo.dwScale = 1;
		m_aviStreamInfo.dwRate = 3;//关键帧数 1秒3帧
		m_aviStreamInfo.dwSuggestedBufferSize = m_pBmpInfoHeader->biSizeImage;
		SetRect(&m_aviStreamInfo.rcFrame, 0, 0, m_pBmpInfoHeader->biWidth, m_pBmpInfoHeader->biHeight);
		opts[0]->fccType = streamtypeVIDEO;
		opts[0]->fccHandler = mmioStringToFOURCC(_T("MSVC"), 0);
		opts[0]->dwQuality = 10000;
		opts[0]->dwBytesPerSecond = 0;
		opts[0]->dwFlags = AVICOMPRESSF_VALID || AVICOMPRESSF_KEYFRAMES;
		opts[0]->lpFormat = 0;
		opts[0]->cbFormat = 0;
		opts[0]->dwInterleaveEvery = 0;
	}
	catch (...)
	{
		L_ERROR(_T("CTranscodingBase::Init catch an error\n"));
		return false;
	}
	
	return true;
}

void CTranscodingBase::Reflesh()
{
#ifdef __DEBUG_SAVE_PNG__
	SavePngFile();
#else
	SendDataToHikDevice();
#endif
}

bool CTranscodingBase::SendDataToHikDevice()
{
	wstring wsVideoPath = m_wsProgramPath + _T("\\video");
	if (!CWinUtils::FileExists(wsVideoPath))
	{
		CWinUtils::CreateDirectorys(wsVideoPath);
	}
	
	int nRand = rand();
	wstring aviFileName = CStringUtils::Format(_T("%d_%d_%d.avi"), m_nCarNo, m_lpHandle, nRand);
	wstring yuvFileName = CStringUtils::Format(_T("%d_%d_%d.yuv"), m_nCarNo, m_lpHandle, nRand);
	wstring aviFilePath = wsVideoPath + _T("\\") + aviFileName;
	wstring yuvFilePath = wsVideoPath + _T("\\") + yuvFileName;

	if (!WriteAVIFile(aviFilePath))	//生成AVI文件
	{
		L_ERROR(_T("WriteAVIFile fail\n"));
		return false;
	}

	if (!Transcode(aviFilePath, yuvFilePath))	//h264转码
	{
		L_ERROR(_T("Transcode fail\n"));
		return false;
	}

	if (!MatrixSendData(yuvFilePath))
	{
		L_ERROR(_T("MatrixSendData fail\n"));
		return false;
	}

	if (CWinUtils::FileExists(aviFilePath))
	{
		CFile::Remove(aviFilePath.c_str());
	}
	if (CWinUtils::FileExists(yuvFilePath))
	{
		CFile::Remove(yuvFilePath.c_str());
	}

	return true;
}

void CTranscodingBase::SavePngFile()
{
	Bitmap mbitmap(HBITMAP(m_bmpBackground), NULL);//从CBitmap中得到HBitmap		
	CLSID pngClsid;
	GetEncoderClsid(L"image/png", &pngClsid);
	unsigned short tcharArr[100];
	//CString logstr;
	//logstr.Format("%s\\log\\%d_%d.png", exePath, m_iCarNum, timeGetTime());
	//memset(tcharArr, 0x0, sizeof(tcharArr));
	//swprintf((wchar_t *)tcharArr, L"%s", logstr.AllocSysString());
	//mbitmap.Save(tcharArr, &pngClsid);//保存

	wstring wsFile = _T("D:\\TETETETET.png");
	mbitmap.Save(wsFile.c_str(), &pngClsid);
}

int CTranscodingBase::GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT num = 0;
	UINT size = 0;
	ImageCodecInfo* pImageCodecInfo = NULL;
	GetImageEncodersSize(&num, &size);
	if (size == 0)
	{
		return -1;
	}
	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if (pImageCodecInfo == NULL)
	{
		return -1;
	}
	GetImageEncoders(num, size, pImageCodecInfo);
	for (UINT j = 0; j < num; ++j)
	{
		if (wcscmp(pImageCodecInfo[j].MimeType, format) == 0)
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;
		}
	}
	free(pImageCodecInfo);
	return -1;
}

bool CTranscodingBase::WriteAVIFile(wstring aviFilePath)
{
	bool breturn = true;
	BYTE lpData[405504];
	int nFrames = 0;
	PAVIFILE pfile;
	PAVISTREAM ps;
	PAVISTREAM pComStream;

	if (CWinUtils::FileExists(aviFilePath))
	{
		CFile::Remove(aviFilePath.c_str());
	}
	
	GetDIBits(m_DC.m_hDC, m_bmpBackground, 0, 288, &lpData, &m_bitmapInfo, DIB_RGB_COLORS);

	WaitForSingleObject(m_mutexMakeAvi, 3000);//互斥量
	AVIFileInit();
	if (AVIFileOpen(&pfile, aviFilePath.c_str(), OF_WRITE | OF_CREATE, NULL) == 0)
	{
		HRESULT hr = AVIFileCreateStream(pfile, &ps, &m_aviStreamInfo);
		AVIMakeCompressedStream(&pComStream, ps, &pCompressOption, NULL);
		AVIStreamSetFormat(pComStream, 0, m_pBmpInfoHeader, sizeof(BITMAPINFOHEADER));
		while (nFrames <= 2)
		{
			hr = AVIStreamWrite(pComStream, nFrames, 1, (LPBYTE)lpData,
				m_pBmpInfoHeader->biSizeImage, AVIIF_KEYFRAME, NULL, NULL);
			nFrames++;
		}
	}
	else
	{
		L_ERROR(_T("AVIFileOpen failed\n"));
		return false;
	}

	if (pComStream)
	{
		AVIStreamClose(pComStream);
	}
	if (ps)
	{
		AVIStreamClose(ps);
	}
	if (pfile != NULL)
		AVIFileRelease(pfile);
	nFrames = 0;
	AVIFileExit();
	ReleaseMutex(m_mutexMakeAvi);//互斥量

	return true;
}

bool CTranscodingBase::Transcode(wstring aviFilePath, wstring yuvFilePath)
{
	if (!CWinUtils::FileExists(aviFilePath))
	{
		L_ERROR(_T("avi file %s not exist\n"), aviFilePath.c_str());
		return false;
	}
	if (CWinUtils::FileExists(yuvFilePath))
	{
		CFile::Remove(yuvFilePath.c_str());
	}

	wstring wsPathMencoder = m_wsProgramPath + THIRDPARTY_PATH_MENCODER;
	wstring wsCommand = _T(" -ovc x264 -x264encopts bitrate=256 -vf scale=352:288 \"") + aviFilePath +
												_T("\" -O \"") + yuvFilePath + _T("\"");

	PROCESS_INFORMATION  ProcessInfo;
	STARTUPINFO  StartupInfo;
	ZeroMemory(&StartupInfo, sizeof(StartupInfo));
	StartupInfo.cb = sizeof(StartupInfo);
	StartupInfo.dwFlags = STARTF_USESHOWWINDOW;
	StartupInfo.wShowWindow = SW_HIDE;
	//	StartupInfo.wShowWindow = SW_SHOW;		
	if (CreateProcess(wsPathMencoder.c_str(), (LPWSTR)wsCommand.c_str(), NULL, NULL, FALSE, 0, NULL, NULL, &StartupInfo, &ProcessInfo))
	{
		DWORD dwWait = WaitForSingleObject(ProcessInfo.hProcess, 5403);
		if (dwWait == WAIT_TIMEOUT || dwWait == WAIT_FAILED)
		{
			TerminateProcess(ProcessInfo.hProcess, 0);//强制关闭  
			CloseHandle(ProcessInfo.hThread);
			CloseHandle(ProcessInfo.hProcess);
			L_ERROR(_T("mencoder WaitForSingleObject WAIT_TIMEOUT OR WAIT_FAILED\n"));

			Sleep(10);
			return false;
		}
		CloseHandle(ProcessInfo.hThread);
		CloseHandle(ProcessInfo.hProcess);
	}
	else
	{
		L_ERROR(_T("mencoder transcode fail\n"));
		return false;
	}

	return true;
}

bool CTranscodingBase::MatrixSendData(wstring yuvFilePath)
{
	bool bRet = true;
	HANDLE hFileHandle;
	hFileHandle = INVALID_HANDLE_VALUE;
	hFileHandle = CreateFile(yuvFilePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileHandle == INVALID_HANDLE_VALUE)
	{
		L_ERROR(_T("CreateFile yuv file fail, file=%s\n"), yuvFilePath.c_str());
		return false;
	}

	DWORD dwFileSize = GetFileSize(hFileHandle, NULL);
	DWORD dwRealSize = 0;
	char *tmpFile = new char[32 * 1024];
	DWORD tmpSize = 32 * 1024;
	BOOL bReadFlag = TRUE;
	while (true)
	{
		bReadFlag = ReadFile(hFileHandle, tmpFile, tmpSize, &dwRealSize, NULL);
		if (!bReadFlag)
		{
			bRet = false;
			L_ERROR(_T("Read yuv file fail, file=%s\n"), yuvFilePath);
			break;
		}
		if (dwRealSize > 0)
		{
			if (!CBekHikUtil::MatrixSendData(m_lpHandle, tmpFile, tmpSize))
			{
				bRet = false;
				L_ERROR(_T("MatrixSendData fail\n"));
				break;
			}
			
			Sleep(300);
		}
		else
		{
			bRet = true;
			break;
		}
	}
	CloseHandle(hFileHandle);
	hFileHandle = INVALID_HANDLE_VALUE;
	if (NULL != tmpFile)
	{
		delete[]tmpFile;
		tmpFile = NULL;
	}

	return bRet;
}
