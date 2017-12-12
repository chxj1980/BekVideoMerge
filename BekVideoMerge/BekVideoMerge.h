
// BekVideoMerge.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CBekVideoMergeApp:
// See BekVideoMerge.cpp for the implementation of this class
//

class CBekVideoMergeApp : public CWinApp
{
public:
	CBekVideoMergeApp();

// Overrides
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

private:
	ULONG_PTR   gdiplusToken;	//GDI+

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CBekVideoMergeApp theApp;