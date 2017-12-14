
// BekVideoMergeDlg.h : header file
//

#pragma once


#include "VideoMergeManager.h"


// CBekVideoMergeDlg dialog
class CBekVideoMergeDlg : public CDialogEx
{
// Construction
public:
	CBekVideoMergeDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BEKVIDEOMERGE_DIALOG };
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
	afx_msg LRESULT OnSocketUDP(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

private:
	CVideoMergeManager videoMergeManager;
	CJmqUDP udpServer;
	CTCPSockz tcpServer;
};
