
// BekVideoWallDlg.h : header file
//

#pragma once

#include "WallVideoMerge.h"

// CBekVideoWallDlg dialog
class CBekVideoWallDlg : public CDialogEx
{
// Construction
public:
	CBekVideoWallDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BEKVIDEOWALL_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg LRESULT OnSocketTCP(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CWallVideoMerge wallVideoMerge;
	CTCPSockz tcpServer;
};
