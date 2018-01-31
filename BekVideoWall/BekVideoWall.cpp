
// BekVideoWall.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "BekVideoWall.h"
#include "BekVideoWallDlg.h"

#define MODULE_NAME	_T("BekVideoWall")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CBekVideoWallApp

BEGIN_MESSAGE_MAP(CBekVideoWallApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CBekVideoWallApp construction

CBekVideoWallApp::CBekVideoWallApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CBekVideoWallApp object

CBekVideoWallApp theApp;


// CBekVideoWallApp initialization

BOOL CBekVideoWallApp::InitInstance()
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
	HANDLE hObject = CreateMutex(NULL, FALSE, _T("BekVideoWall"));
	if (ERROR_ALREADY_EXISTS == GetLastError())
	{
		CloseHandle(hObject);
		return FALSE;
	}

	//日志模块初始化
	wstring wstrCurrentPath = _T("");
	wstring wstrLogPath = _T("");
	wstring wstrLogConfPath = _T("");
	CWinUtils::GetCurrentProcessPath(wstrCurrentPath);
	wstrLogPath = wstrCurrentPath + _T("\\log");
	if (!CWinUtils::FileExists(wstrLogPath))
	{
		CWinUtils::CreateDirectorys(wstrLogPath);
	}
	wstrLogConfPath = wstrCurrentPath + LOG_CONF_BEKVIDEOWALL;
	LogBase_init(wstrLogConfPath.c_str());

	CoInitialize(NULL);

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

	CBekVideoWallDlg *pDlg = new CBekVideoWallDlg;
	m_pMainWnd = pDlg;
	return pDlg->Create(IDD_BEKVIDEOWALL_DIALOG);
}

int CBekVideoWallApp::ExitInstance()
{
	// TODO: Add your specialized code here and/or call the base class

	L_INFO(_T("BekVideoWall Exit"));

	CoUninitialize();

	return CWinApp::ExitInstance();
}

