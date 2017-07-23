#pragma once

#include "Base/Types.h"
#include "afxcmn.h"
#include "afxwin.h"


// CFileProgressDlg dialog
class CFileProgressDlg : public CDialog
{
	DECLARE_DYNAMIC(CFileProgressDlg)

public:
	CFileProgressDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CFileProgressDlg();

// Dialog Data
	enum { IDD = IDD_FILEPROGRESS_DIALOG };

	/// Set the primary text
	void SetPrimaryText( const wchar_t* szText );

	/// Set the primary progress
	void SetPrimaryProgress( float32 normPercent );

	/// Set the secondary text
	void SetSecondaryText( const wchar_t* szText );

	/// Set the secondary progress
	void SetSecondaryProgress( float32 normPercent );

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_ctlPrimaryProgress;
	CProgressCtrl m_ctlSecondaryProgress;
public:
	CStatic m_ctlPrimaryLabel;
public:
	CStatic m_ctlSecondaryLabel;
};
