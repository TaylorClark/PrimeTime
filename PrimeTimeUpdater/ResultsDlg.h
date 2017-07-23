#pragma once


// CResultsDlg dialog

class CResultsDlg : public CDialog
{
	DECLARE_DYNAMIC(CResultsDlg)

	CString m_ResultsText;

public:
	CResultsDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CResultsDlg();

// Dialog Data
	enum { IDD = IDD_RESULTS_DIALOG };

	CPoint m_EditBoxSizeOffset;

	int m_ButtonVertOffset;

	virtual BOOL OnInitDialog();

	void SetMultilineResultText( const wchar_t* szText );

	void AppendNewLineResultText( CString sText );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
};
