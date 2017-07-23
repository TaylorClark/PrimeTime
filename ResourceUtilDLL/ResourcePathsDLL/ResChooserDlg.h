#pragma once

#include "Resource.h"
#include "afxcmn.h"
#include "ResourceTools/ResourceToolsDB.h"

// CResChooserDlg dialog

class CResChooserDlg : public CDialog
{
private:
	DECLARE_DYNAMIC(CResChooserDlg)

	/// The resource ID of the selected item, 0 if none
	ResourceID m_SelResID;
	const ResourceMgr::KnownResVector* m_pResList;
	EResourceType m_FilterResType;

	std::wstring _resIndexPath;

	void Init( );
	void Init( const std::vector< ResourceMgr::KnownResourceItem >& knownResources );

public:
	CResChooserDlg(const wchar_t* szResIndexPath, CWnd* pParent = NULL);   // standard constructor
	CResChooserDlg( const std::vector< ResourceMgr::KnownResourceItem >& knownResources, CWnd* pParent = NULL);   // standard constructor
	virtual ~CResChooserDlg();

	/// Set the resource type by which we are filtering
	void SetFilter( EResourceType resType ){ m_FilterResType = resType; }

// Dialog Data
	enum { IDD = IDD_DIALOG_RESCHOOSER };

	/// Get the selected resource ID
	ResourceID GetSelectedResourceID() const { return m_SelResID; }

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	CTreeCtrl m_ctlResTree;
public:
	afx_msg void OnNMDblclkTreeResources(NMHDR *pNMHDR, LRESULT *pResult);
};
