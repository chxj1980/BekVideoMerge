
// BekVideoMergeDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BekVideoMerge.h"
#include "BekVideoMergeDlg.h"
#include "afxdialogex.h"

#define MODULE_NAME	_T("BekVideoMerge")

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


// CBekVideoMergeDlg dialog



CBekVideoMergeDlg::CBekVideoMergeDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BEKVIDEOMERGE_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBekVideoMergeDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBekVideoMergeDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET_UDP, OnSocketTCP)
	ON_MESSAGE(WM_SOCKET_UDP, OnSocketUDP)
END_MESSAGE_MAP()


// CBekVideoMergeDlg message handlers

BOOL CBekVideoMergeDlg::OnInitDialog()
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

	L_DEBUG(_T("CBekVideoMergeDlg OnInitDialog\n"));
	videoMergeManager.StartWork();

	if (!jmqTcp.InitSockS(m_hWnd, LISTENING_PORT_TCP, WM_SOCKET_TCP, 0))
	{
		L_ERROR(_T("jmqTcp.InitSockS failed, Exit.\n"));
		CDialog::DestroyWindow();
		return FALSE;
	}
	if (!jmqUdp.InitSockU(m_hWnd, LISTENING_PORT_UDP, WM_SOCKET_UDP))
	{
		L_ERROR(_T("jmqUdp.InitSockU failed, Exit.\n"));
		CDialog::DestroyWindow();
		return FALSE;
	}


	

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBekVideoMergeDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CBekVideoMergeDlg::OnPaint()
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
HCURSOR CBekVideoMergeDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CBekVideoMergeDlg::OnSocketTCP(WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

LRESULT CBekVideoMergeDlg::OnSocketUDP(WPARAM wParam, LPARAM lParam)
{
	return S_OK;
}

