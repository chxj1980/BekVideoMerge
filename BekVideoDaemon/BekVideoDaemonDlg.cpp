
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

	//�������ͼ��
	HICON m_hicon = ::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));//����һ������ͼ��    
	m_nId.cbSize = sizeof(NOTIFYICONDATA);
	m_nId.hWnd = m_hWnd;                          //ָ�����ھ��    
	m_nId.uID = IDR_MAINFRAME;
	m_nId.uFlags = NIF_MESSAGE | NIF_ICON | NIF_TIP;  //ָ����Ч��Ա    
	m_nId.uCallbackMessage = WM_SYSTEMTRAY;       //�Զ�����Ϣ    
	m_nId.hIcon = m_hicon;                        //ָ������ͼ��    
	wcscpy_s(m_nId.szTip, _T("����һ"));           //���������ʾ    
	::Shell_NotifyIcon(NIM_ADD, &m_nId);          //�����������ͼ��     

	PostMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);

	//�ػ��߳�
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
	// TODO:  �ڴ����ר�ô����/����û���  
	switch (message)
	{
	case WM_SYSCOMMAND:
		if (wParam == SC_MINIMIZE)
		{
			CBekVideoDaemonDlg *dlg = (CBekVideoDaemonDlg*)AfxGetApp()->GetMainWnd();//��ȡ����CWndָ��    
			HWND hwnd = dlg->m_hWnd;//��ȡh_hWndָ��    
			::ShowWindow(hwnd, 0);//���ؽ���    
			return TRUE;
		}
		if (wParam == SC_RESTORE)
		{
		}
		break;

	case WM_CLOSE:
		Shell_NotifyIcon(NIM_DELETE, &m_nId);//�ر����ʱж������ͼ��    
		break;
	case WM_SYSTEMTRAY:
	{
		if (wParam != IDR_MAINFRAME)
			return 1;
		switch (lParam)
		{
		case WM_RBUTTONUP://�Ҽ�����ʱ������ݲ˵�������ֻ��һ��"�ر�"     
		{
			CMenu menu;
			LPPOINT lpoint = new tagPOINT;
			::GetCursorPos(lpoint);     // �õ����λ��  
			menu.CreatePopupMenu(); // ����һ������ʽ�˵�  
			SetForegroundWindow();//�����������˵���ʱ��,�˵�ʧȥ�����ò˵��Զ��ر�  
			menu.AppendMenu(MF_STRING, WM_DESTROY, _T("�˳�"));// ���Ӳ˵���"�ر�"�����������ϢWM_DESTROY �������ڣ������أ������������  
															 //�����͵Ĳ˵���Ҫ�Զ�����Ϣ������Ϣӳ�����ص���Ϣ��Ӧ����  
			menu.TrackPopupMenu(TPM_LEFTALIGN, lpoint->x, lpoint->y, this);// ȷ������ʽ�˵���λ��  

			HMENU hmenu = menu.Detach();
			menu.DestroyMenu();// ��Դ����  
			delete lpoint;// ��Դ����  
			break;
		}
		case WM_LBUTTONDBLCLK://˫������Ĵ���     
		{
			//this->ShowWindow(SW_SHOW);//�򵥵���ʾ���������¶�    
			break;
		}
		case WM_LBUTTONDOWN://��������Ĵ���  
		{
			//this->ShowWindow(SW_SHOW);//�򵥵���ʾ���������¶�   
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


	// TODO:  �ڴ˴������Ϣ����������  
	Shell_NotifyIcon(NIM_DELETE, &m_nId);//�ر����ʱж������ͼ��
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

