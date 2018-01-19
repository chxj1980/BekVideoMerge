#include "stdafx.h"
#include "Function.h"

wstring Base64Decode(wstring encodeStr)
{
	wstring wsRet = _T("");
	
	int nLen = 0;
	CBase64Utils::Decode(encodeStr, nLen, NULL);
	if (nLen > 0)
	{
		char *decodeBuf = new char[nLen];
		CBase64Utils::Decode(encodeStr, nLen, decodeBuf);
		string sDecode = decodeBuf;
		CStringUtils::ASCII2Unicode(sDecode, wsRet);

		if (NULL != decodeBuf)
		{
			delete	[]decodeBuf;
		}
	}

	return wsRet;
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
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