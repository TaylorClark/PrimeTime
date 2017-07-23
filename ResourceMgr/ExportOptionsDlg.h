#pragma once

#include "Resource.h"
#include "afxwin.h"
#include "afxcmn.h"
#include <map>
#include <string>
#include "ResourceTools/ResourceToolsDB.h"


// CExportOptionsDlg dialog
typedef std::map< ResourceID, std::wstring > ExportMap;

class CExportOptionsDlg : public CDialog
{
private:
	
	enum OutputTemplate
	{
		OT_Default,
		OT_AllInOne,
		OT_Custom,
		OT_Clear
	};

	DECLARE_DYNAMIC(CExportOptionsDlg)

public:
	CExportOptionsDlg(ResourceToolsDB* pResDB, CWnd* pParent = NULL);   // standard constructor
	virtual ~CExportOptionsDlg();

// Dialog Data
	enum { IDD = IDD_EXPORTFILES_DIALOG };

private:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	virtual BOOL OnInitDialog();

	void UpdateListOutputs();
	
	ResourceID GetResIDFromIndex( int index );

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_ctlResList;
	ResourceToolsDB* m_pResDB;

	/// The current template used to save the file
	OutputTemplate m_OutputTemplate;

	/// The map to store how to export the resources
	ExportMap m_RetMap;

	/// The currently selected list item index, -1 for nothing selected
	int m_CurSelListIndex;

private:
	CComboBox m_ctlTemplateCombo;
	afx_msg void OnCbnSelchangeTemplateCombo();
	afx_msg void OnBnClickedSetpathButton();
private:
	CEdit m_ctlPathEdit;
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnEditchangeDestfileCombo();
private:
	CComboBox m_ctlDestFileCombo;
	afx_msg void OnNMClickResList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnLvnItemActivateResList(NMHDR *pNMHDR, LRESULT *pResult);
public:
	afx_msg void OnHdnItemclickResList(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnBnClickedSetallButton();
};
