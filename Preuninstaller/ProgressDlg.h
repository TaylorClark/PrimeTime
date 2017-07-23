#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CProgressDlg dialog

class CProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CProgressDlg)

public:
	CProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CProgressDlg();

// Dialog Data
	enum { IDD = IDD_PROG_DIALOG };

	void SetProgress( float prog );

	void SetLabelText( const wchar_t* szText );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

private:
	CProgressCtrl m_ctlProgressBar;
public:
	CStatic m_ctlProgLabel;
};
