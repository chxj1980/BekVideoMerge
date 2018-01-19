#pragma once
#ifndef __FUNCTION_H__
#define __FUNCTION_H__



wstring Base64Decode(wstring encodeStr);

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid);

#endif
