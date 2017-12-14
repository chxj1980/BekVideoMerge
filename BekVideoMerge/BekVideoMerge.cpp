
// BekVideoMerge.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BekVideoMerge.h"
#include "BekVideoMergeDlg.h"

#define MODULE_NAME	_T("BekVideoMerge")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CBekVideoMergeApp

BEGIN_MESSAGE_MAP(CBekVideoMergeApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBekVideoMergeApp construction

CBekVideoMergeApp::CBekVideoMergeApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBekVideoMergeApp object

CBekVideoMergeApp theApp;


// CBekVideoMergeApp initialization

BOOL CBekVideoMergeApp::InitInstance()
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
	HANDLE hObject = CreateMutex(NULL, FALSE, _T("BekVideoMerge"));
	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		CloseHandle(hObject);
		return FALSE;
	}

	//日志模块初始化
	wstring wstrCurrentPath = _T("");
	wstring wstrLogConfPath = _T("");
	CWinUtils::GetCurrentProcessPath(wstrCurrentPath);
	wstrLogConfPath = wstrCurrentPath + _T("\\log");
	if (!CWinUtils::FileExists(wstrLogConfPath))
	{
		CWinUtils::CreateDirectorys(wstrLogConfPath);
	}
	LogBase_init(LOG_CONF_BEKVIDEOMERGE);

	//gdiplus初始化
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);
	CoInitialize(NULL);//ADO in

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
	
	CBekVideoMergeDlg *pDlg = new CBekVideoMergeDlg;
	m_pMainWnd = pDlg;
	return pDlg->Create(IDD_BEKVIDEOMERGE_DIALOG);
}

int CBekVideoMergeApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	L_INFO(_T("BekVideoMerge Exit"));

	GdiplusShutdown(gdiplusToken);	//GDI+ exit
	CoUninitialize();

	return CWinApp::ExitInstance();
}

