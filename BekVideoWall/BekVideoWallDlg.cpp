
// BekVideoWallDlg.cpp : implementation file
//

#include "stdafx.h"
#include "BekVideoWall.h"
#include "BekVideoWallDlg.h"
#include "afxdialogex.h"

#define MODULE_NAME	_T("BekVideoWall")

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


// CBekVideoWallDlg dialog



CBekVideoWallDlg::CBekVideoWallDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_BEKVIDEOWALL_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CBekVideoWallDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CBekVideoWallDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SOCKET_TCP_WALL, OnSocketTCP)
END_MESSAGE_MAP()


// CBekVideoWallDlg message handlers

BOOL CBekVideoWallDlg::OnInitDialog()
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

	L_DEBUG(_T("CBekVideoWallDlg OnInitDialog\n"));

	//�������Ļ���
	setlocale(LC_ALL, ".936");

	if (!wallVideoMerge.StartWork())
	{
		L_ERROR(_T("wallVideoMerge.StartWork failed, Exit\n"));
		CDialog::OnCancel();
		//CDialog::DestroyWindow();
		return FALSE;
	}
	if (!tcpServer.InitSockS(m_hWnd, LISTENING_PORT_TCP_WALL, WM_SOCKET_TCP_WALL, 0))
	{
		L_ERROR(_T("tcpServer.InitSockS failed, Exit.\n"));
		CDialog::OnCancel();
		//CDialog::DestroyWindow();
		return FALSE;
	}

	L_INFO(_T("CBekVideoWallDlg OnInitDialog Success\n"));
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CBekVideoWallDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CBekVideoWallDlg::OnPaint()
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
HCURSOR CBekVideoWallDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT CBekVideoWallDlg::OnSocketTCP(WPARAM wParam, LPARAM lParam)
{
	int nEvent = WSAGETSELECTEVENT(lParam); //��Ϣ��� 
	int nErrorCode = WSAGETSELECTERROR(lParam);//������� 
	SOCKET sock = (SOCKET)wParam; //Ŀ��socket 
	switch (nEvent)
	{
	case FD_ACCEPT:
	{
		L_INFO(_T("Accept from TCP Client\n"));
		tcpServer.OnFDAccept();
		break;
	}
	case FD_READ:
	{
		string strRecv = tcpServer.RecvFromSock(sock);
		wstring wsRecv = _T("");
		CStringUtils::ASCII2Unicode(strRecv, wsRecv);
		L_DEBUG(_T("TCPServer Receive = %s\n"), wsRecv.c_str());



		break;
	}
	case FD_CLOSE:
	{
		L_INFO(_T("Close from TCP Client\n"));
		closesocket(sock);
		break;
	}
	default:
		break;
	}

	return S_OK;
}
