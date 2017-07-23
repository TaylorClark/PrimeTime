#pragma once


// CSysTrayIconDlg dialog

class CSysTrayIconDlg : public CDialog
{
	DECLARE_DYNAMIC(CSysTrayIconDlg)

public:
	CSysTrayIconDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CSysTrayIconDlg();

	// The notify icon data structure to display the icon in system tray
	NOTIFYICONDATA m_NotifyIcon;

// Dialog Data
	enum { IDD = IDD_SYSTRAYICON_DIALOG };

protected:

	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	afx_msg LRESULT OnNotifyIcon( WPARAM wParam, LPARAM lParam );

	DECLARE_MESSAGE_MAP()
};
