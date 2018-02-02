
// BekVideoDaemonDlg.h : header file
//

#pragma once


// CBekVideoDaemonDlg dialog
class CBekVideoDaemonDlg : public CDialogEx
{
// Construction
public:
	CBekVideoDaemonDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BEKVIDEODAEMON_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);


// Implementation
protected:
	HICON m_hIcon;

private:
	static DWORD WINAPI DaemonThread(LPVOID lpParameter);		// ÿª§œﬂ≥Ã
	int GetProcessidFromName(wchar_t* name);

private:
	NOTIFYICONDATA m_nId;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
};
