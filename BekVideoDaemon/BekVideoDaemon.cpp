
// BekVideoDaemon.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BekVideoDaemon.h"
#include "BekVideoDaemonDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBekVideoDaemonApp

BEGIN_MESSAGE_MAP(CBekVideoDaemonApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBekVideoDaemonApp construction

CBekVideoDaemonApp::CBekVideoDaemonApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBekVideoDaemonApp object

CBekVideoDaemonApp theApp;


// CBekVideoDaemonApp initialization

BOOL CBekVideoDaemonApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);


	//进程互斥
	HANDLE hObject = CreateMutex(NULL, FALSE, _T("BekVideoDaemon"));
	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		CloseHandle(hObject);
		return FALSE;
	}

	//文件目录初始化
	wstring wstrCurrentPath = _T("");
	CWinUtils::GetCurrentProcessPath(wstrCurrentPath);
	wstring wstrLogPath = wstrCurrentPath + _T("\\log");
	if (!CWinUtils::FileExists(wstrLogPath))
	{
		CWinUtils::CreateDirectorys(wstrLogPath);
	}

	//日志模块初始化
	wstring wsLogPath = wstrCurrentPath + _T("\\conf\\BekVideoDaemon_logconf.ini");
	LogBase_init(wsLogPath.c_str());


	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	//CBekVideoDaemonDlg *pDlg = new CBekVideoDaemonDlg;
	//m_pMainWnd = pDlg;
	//return pDlg->Create(IDD_BEKVIDEODAEMON_DIALOG);

	CBekVideoDaemonDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}
	else if (nResponse == -1)
	{
		TRACE(traceAppMsg, 0, "Warning: dialog creation failed, so application is terminating unexpectedly.\n");
		TRACE(traceAppMsg, 0, "Warning: if you are using MFC controls on the dialog, you cannot #define _AFX_NO_MFC_CONTROLS_IN_DIALOGS.\n");
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

#ifndef _AFXDLL
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

