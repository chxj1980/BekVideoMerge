
// BekVideoDaemonDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BekVideoDaemon.h"
#include "BekVideoDaemonDlg.h"
#include "afxdialogex.h"


#define MODULE_NAME	_T("CBekVideoDaemon")

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CBekVideoDaemonDlg dialog



CBekVideoDaemonDlg::CBekVideoDaemonDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BEKVIDEODAEMON_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBekVideoDaemonDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBekVideoDaemonDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
END_MESSAGE_MAP()


// CBekVideoDaemonDlg message handlers

BOOL CBekVideoDaemonDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

	//添加托盘图标
	HICON m_hicon = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));//加载一个托盘图标    
	m_nId.cbSize = sizeof(NOTIFYICONDATA);
	m_nId.hWnd = m_hWnd;                          //指定窗口句柄    
	m_nId.uID = IDR_MAINFRAME;
	m_nId.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;  //指定有效成员    
	m_nId.uCallbackMessage = WM_SYSTEMTRAY;       //自定义消息    
	m_nId.hIcon = m_hicon;                        //指定托盘图标    
	wcscpy_s(m_nId.szTip, _T("六合一"));           //添加气泡提示    
	::Shell_NotifyIcon(NIM_ADD, &m_nId);          //在托盘区添加图标     

	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);

	//守护线程
	HANDLE hDaemonThread = CreateThread(NULL, 0, DaemonThread, LPVOID(this), 0, NULL);
	CloseHandle(hDaemonThread);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

DWORD WINAPI CBekVideoDaemonDlg::DaemonThread(LPVOID lpParameter)
{
	CBekVideoDaemonDlg *pDlg = (CBekVideoDaemonDlg*)lpParameter;
	
	wstring wstrCurrentPath = _T("");
	wstring wsVideoMergePath = _T("");
	CWinUtils::GetCurrentProcessPath(wstrCurrentPath);
	wsVideoMergePath = wstrCurrentPath + _T("\\BekVideoMerge.exe");

	while (true)
	{
		if (!pDlg->GetProcessidFromName(_T("BekVideoMerge.exe")))
		{
			L_INFO(_T("BekVideoMerge.exe is not running\n"));

			int nRet = (int)ShellExecuteW(NULL, NULL, wsVideoMergePath.c_str(), _T("1"), NULL, SW_HIDE);
			if (nRet <= 31)
			{
				L_ERROR(_T("Run BekVideoMerge.exe failed, errorId=%d\n"), GetLastError());
			}
			else
			{
				L_INFO(_T("Run BekVideoMerge.exe success\n"));
			}
		}

		Sleep(10000);
	}
	return 0;
}

LRESULT CBekVideoDaemonDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO:  在此添加专用代码和/或调用基类  
	switch (message)
	{
	case WM_SYSCOMMAND:
		if (wParam == SC_MINIMIZE)
		{
			CBekVideoDaemonDlg *dlg = (CBekVideoDaemonDlg*)AfxGetApp()->GetMainWnd();//获取窗口CWnd指针    
			HWND hwnd = dlg->m_hWnd;//获取h_hWnd指针    
			::ShowWindow(hwnd, 0);//隐藏界面    
			return TRUE;
		}
		if (wParam == SC_RESTORE)
		{
		}
		break;

	case WM_CLOSE:
		Shell_NotifyIcon(NIM_DELETE, &m_nId);//关闭软件时卸载托盘图标    
		break;
	case WM_SYSTEMTRAY:
	{
		if (wParam != IDR_MAINFRAME)
			return 1;
		switch (lParam)
		{
		case WM_RBUTTONUP://右键起来时弹出快捷菜单，这里只有一个"关闭"     
		{
			CMenu menu;
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);     // 得到鼠标位置  
			menu.CreatePopupMenu(); // 声明一个弹出式菜单  
			SetForegroundWindow();//如果点击弹出菜单的时候,菜单失去焦点让菜单自动关闭  
			menu.AppendMenu(MF_STRING, WM_DESTROY, _T("退出"));// 增加菜单项"关闭"，点击则发送消息WM_DESTROY 给主窗口（已隐藏），将程序结束  
															 //此类型的菜单需要自定义消息，做消息映射和相关的消息响应函数  
			menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);// 确定弹出式菜单的位置  

			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();// 资源回收  
			delete lpoint;// 资源回收  
			break;
		}
		case WM_LBUTTONDBLCLK://双击左键的处理     
		{
			//this->ShowWindow(SW_SHOW);//简单的显示主窗口完事儿    
			break;
		}
		case WM_LBUTTONDOWN://单击左键的处理  
		{
			//this->ShowWindow(SW_SHOW);//简单的显示主窗口完事儿   
			break;
		}
		}
	}
	break;
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}

void CBekVideoDaemonDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

void CBekVideoDaemonDlg::OnDestroy()
{
	CDialogEx::OnDestroy();

	L_INFO(_T("OnDestroy CBekVideoDaemonDlg\n"));

	int nRet = (int)ShellExecuteW(NULL, _T("open"), _T("taskkill"), _T("/im BekVideoMerge.exe /f /t"), NULL, SW_HIDE);
	if (nRet <= 31)
	{
		L_ERROR(_T("Kill BekVideoMerge.exe failed, errorId=%d\n"), GetLastError());
	}
	else
	{
		L_INFO(_T("Kill BekVideoMerge.exe success\n"));
	}


	// TODO:  在此处添加消息处理程序代码  
	Shell_NotifyIcon(NIM_DELETE, &m_nId);//关闭软件时卸载托盘图标
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBekVideoDaemonDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CBekVideoDaemonDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

int CBekVideoDaemonDlg::GetProcessidFromName(wchar_t* name)
{
	PROCESSENTRY32W pe;
	DWORD id = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32W);
	if (!Process32First(hSnapshot, &pe))
		return 0;
	while (1)
	{
		pe.dwSize = sizeof(PROCESSENTRY32W);
		if (Process32Next(hSnapshot, &pe) == FALSE)
			break;
		if (StrCmpW(pe.szExeFile, name) == 0)
		{
			id = pe.th32ProcessID;
			break;
		}
	}
	CloseHandle(hSnapshot);
	return id;
}

