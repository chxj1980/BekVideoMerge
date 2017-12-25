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