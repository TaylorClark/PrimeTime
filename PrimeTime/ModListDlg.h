#pragma once
#include <afxwin.h>
#include "../Resource.h"


// CModListDlg dialog

class CModListDlg : public CDialog
{
	DECLARE_DYNAMIC(CModListDlg)

public:
	CModListDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CModListDlg();

// Dialog Data
	enum { IDD = IDD_ORDEREDLISTDIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedAddbutton();
	CEdit m_ctlNewValueEdit;
	CListBox m_ctlStrList;
};
