#pragma once

#include "Resource.h"
#include <string>
#include "afxwin.h"


// CResPathDlg dialog

class CResPathDlg : public CDialog
{
	DECLARE_DYNAMIC(CResPathDlg)

	std::wstring m_sResPath;
	std::wstring m_sGamePath;

public:
	CResPathDlg( const wchar_t *szResPath, const wchar_t *szGamePath );   // standard constructor
	virtual ~CResPathDlg();

// Dialog Data
	enum { IDD = IDD_MAIN };

	std::wstring GetPathRes() const { return m_sResPath; }
	std::wstring GetPathGame() const { return m_sGamePath; }

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonok();
	afx_msg void OnBnClickedButtoncancel();
	CEdit m_ctlPathRes;
	CEdit m_ctlPathGame;
	afx_msg void OnBnClickedButtonRespath();
	afx_msg void OnBnClickedButtonGamepath();
};
