#pragma once
#ifndef __TRANSCODING_BASE_H__
#define __TRANSCODING_BASE_H__

#include "resource.h"
#include <math.h>
#include <vfw.h>
#include <mmsystem.h>
#pragma comment (lib,"vfw32.lib")
#pragma comment (lib,"winmm.lib")

class CTranscodingBase
{
public:
	CTranscodingBase();
	~CTranscodingBase();

public:
	bool Init(wstring path, int carNo, int lpHanble);
	void Reflesh();

public:
	CDC m_DC;
	wstring m_wsProgramPath;
	int m_nCarNo;


private:
	int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);
	bool WriteAVIFile(wstring aviFilePath);
	bool Transcode(wstring aviFilePath, wstring yuvFilePath);
	bool MatrixSendData(wstring yuvFilePath);
	void SavePngFile();
	bool SendDataToHikDevice();

private:
	CBitmap m_bmpBackground;
	BITMAPINFOHEADER *m_pBmpInfoHeader;
	BITMAPINFO m_bitmapInfo;
	AVISTREAMINFO m_aviStreamInfo;
	HANDLE m_mutexMakeAvi;
	AVICOMPRESSOPTIONS pCompressOption;
	AVICOMPRESSOPTIONS FAR * opts[1];// = {&pCompressOption};

	int m_lpHandle;
};

#endif



