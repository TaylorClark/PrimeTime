// ResourceMgrDlg.h : header file
//

#pragma once
#include "afxcmn.h"
#include "ResourceTools/ResourceToolsDB.h"
#include "afxwin.h"

// CResourceMgrDlg dialog
class CResourceMgrDlg : public CDialog
{
// Construction
public:
	CResourceMgrDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_RESOURCEMGR_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	HTREEITEM GetTreeItemParent( EResourceType resType );
	void AddResToTree( const ResourceToolsDB::ToolsResourceInfo& res );
	void RefreshTreeFromDB();
	void UpdateWindowTitle();
	void AddRes( const std::wstring& sFileName );

	/// The resource database
	ResourceToolsDB m_ResDB;

	/// The window title before we add any text to it such as the current file name
	std::wstring m_sOrigWinTitle;

	/// The resource path for quick reference
	std::wstring m_sResPath;

	/// A flag that gets set if changes were made since loading a tools resource index
	bool _hasChanges;

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg void OnSize( UINT type, int cx, int cy );
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
private:
	CTreeCtrl m_ResourceTreeCtrl;

	// The tree items
	HTREEITEM m_hIndependentRezTreeItem;
	HTREEITEM m_hIndImageTreeItem;
	HTREEITEM m_hIndSoundTreeItem;
	HTREEITEM m_hIndMusicTreeItem;

	HTREEITEM m_hGameRezTreeItem;
	HTREEITEM m_hGameFontTreeItem;
	HTREEITEM m_hGameSpriteTreeItem;

	CEdit m_ctlNextResID;

	void SetNextResIDText( uint32 nextResID );

public:
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg void OnBnClickedAddrez();
	afx_msg void OnBnClickedRemoveRez();
	afx_msg void OnBnClickedExportDB();
	afx_msg void OnBnClickedOpenIndex();
	afx_msg void OnBnClickedSaveIndex();
	afx_msg void OnBnClickedClear();
	afx_msg void OnBnClickedPathsOptions();
	afx_msg void OnKeyDownResTree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRclickReztree(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNextResIDEditChange();
	afx_msg void OnNextResIDEditLoseFocus();
private:
	CEdit _resRootPathEdit;
public:
	afx_msg void OnEnChangeRootrespathEdit();
	afx_msg void OnBnClickedSelectrootrespathButton();
	afx_msg void OnClose();
};
