#pragma once

#include <string>
#include "Resource.h"
#include "ResourceTools/ResourceToolsDB.h"
#include "afxwin.h"

// CResourcePropDlg dialog

class CResourcePropDlg : public CDialog
{
private:

	DECLARE_DYNAMIC(CResourcePropDlg)

	/// The resource info
	ResourceToolsDB::ToolsResourceInfo& m_ResInfo;

	/// The resource root path
	std::wstring m_sResRootPath;

public:
	CResourcePropDlg( ResourceToolsDB::ToolsResourceInfo& resInfo, const std::wstring& sRootResPath, CWnd* pParent = NULL);   // standard constructor
	virtual ~CResourcePropDlg();

// Dialog Data
	enum { IDD = IDD_RESPROP_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL OnInitDialog();
	
	void CheckPathForValidity();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CEdit m_ctlFullPathText;
	CEdit m_ctlNameText;
	CEdit m_ctlResIDText;
	CEdit m_ctlIDDefEdit;
	CStatic m_ctlBadPathStatic;
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnEnChangeRespathEdit();
private:
	CButton m_ctlPackRLECheck;
};
