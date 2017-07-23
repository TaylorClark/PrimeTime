// ResChooserDlg.cpp : implementation file
//

#include <string>
#include "stdafx.h"
#include "ResChooserDlg.h"
#include "Base/RegKeyObj.h"


// CResChooserDlg dialog

IMPLEMENT_DYNAMIC(CResChooserDlg, CDialog)

CResChooserDlg::CResChooserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResChooserDlg::IDD, pParent),
		m_pResList( NULL ),
		m_FilterResType( RT_Error )
{
	m_SelResID = 0;
}

CResChooserDlg::CResChooserDlg( const std::vector< ResourceMgr::KnownResourceItem >& knownResources, CWnd* pParent /*=NULL*/)
	: CDialog(CResChooserDlg::IDD, pParent),
		m_pResList( &knownResources ),
		m_FilterResType( RT_Error )
{
	m_SelResID = 0;
}

CResChooserDlg::~CResChooserDlg()
{
}

void CResChooserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TREE_RESOURCES, m_ctlResTree);
}


BEGIN_MESSAGE_MAP(CResChooserDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CResChooserDlg::OnBnClickedOk)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE_RESOURCES, &CResChooserDlg::OnNMDblclkTreeResources)
END_MESSAGE_MAP()


// CResChooserDlg message handlers

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResChooserDlg::OnBnClickedOk  Public
///
///	A message handler called when the user presses the OK button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResChooserDlg::OnBnClickedOk()
{
	// Get the selected item
	HTREEITEM hSelItem = m_ctlResTree.GetSelectedItem();

	// Get the resource ID
	m_SelResID = (ResourceID)m_ctlResTree.GetItemData( hSelItem );

	// End the dialog
	OnOK();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResChooserDlg::Init  Private
///
///	Initialize the dialog with a list of resources.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResChooserDlg::Init( const std::vector< ResourceMgr::KnownResourceItem >& knownResources )
{
	// Populate the tree
	for( std::vector< ResourceMgr::KnownResourceItem >::const_iterator iterRes = knownResources.begin();
			iterRes != knownResources.end();
			++iterRes )
	{
		// If we are filtering and this resource is not of the filter type
		if( (m_FilterResType != RT_Error) && (iterRes->indexData.resType != m_FilterResType) )
			continue;

		// Initialize the add item structure
		TVINSERTSTRUCT newTreeItem;
		newTreeItem.hInsertAfter = NULL;
		newTreeItem.hParent = NULL;
		newTreeItem.item.mask = 0;
		newTreeItem.itemex.mask = TVIF_TEXT;
		newTreeItem.itemex.pszText = (LPWSTR)iterRes->indexData.szName;

		// Add the item
		HTREEITEM hAddedItem = m_ctlResTree.InsertItem( &newTreeItem );
		if( !hAddedItem )
			continue;

		// Store the resource ID
		m_ctlResTree.SetItemData( hAddedItem, iterRes->indexData.resourceID );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResChooserDlg::Init  Private
///
///	Initialize the dialog by loading the tools resource index.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResChooserDlg::Init()
{
	// Get the resource path
	RegKeyObj regKey;
	regKey.Init( RegKeyObj::RK_LocalMachine, ResourceToolsDB::REGPATH_RootKeyPath );
	std::wstring sResPath;

	// If we can't find the registry key
	if( !regKey.GetValue( ResourceToolsDB::REGPATH_ResPath, &sResPath ) )
	{
		// Add a bad item
		std::wstring sError = _T("Unable to find tools resource index file from registry");
		sError += sResPath;
		m_ctlResTree.InsertItem( sError.c_str() );

		return;
	}

	// Add on the file name
	sResPath += ResourceToolsDB::FILENAME_ToolsResIndex;

	// Load the tools resource index file
	ResourceToolsDB resDB;
	if( !resDB.LoadToolsIndex( sResPath ) )
	{
		// Add a bad item indicating we couldn't load the data
		std::wstring sError = _T("Unable to load tools resource index file (");
		sError += sResPath;
		sError += _T(")");
		m_ctlResTree.InsertItem( sError.c_str() );

		return;
	}

	// Populate the tree
	const ResourceToolsDB::ResourceList& resList = resDB.GetResList();
	for( ResourceToolsDB::ResourceList::const_iterator iterRes = resList.begin();
			iterRes != resList.end();
			++iterRes )
	{
		// If we are filtering and this resource is not of the filter type
		if( (m_FilterResType != RT_Error) && (iterRes->resType != m_FilterResType) )
			continue;

		// Create the text item
		std::wstring sItemText = iterRes->sName;
		sItemText += _T(" (");
		sItemText += iterRes->sFilePath;
		sItemText += _T(")");

		// Initialize the add item structure
		TVINSERTSTRUCT newTreeItem;
		newTreeItem.hInsertAfter = NULL;
		newTreeItem.hParent = NULL;
		newTreeItem.item.mask = 0;
		newTreeItem.itemex.mask = TVIF_TEXT;
		newTreeItem.itemex.pszText = (LPWSTR)sItemText.c_str();

		// Add the item
		HTREEITEM hAddedItem = m_ctlResTree.InsertItem( &newTreeItem );
		if( !hAddedItem )
			continue;

		// Store the resource ID
		m_ctlResTree.SetItemData( hAddedItem, iterRes->resID );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResChooserDlg::OnInitDialog  Protected
///
///	A message handler called when the dialog is initialized.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CResChooserDlg::OnInitDialog()
{
	// Initialize the base class
	CDialog::OnInitDialog();

	// Initialize the tree view data
	if( !m_pResList )
		Init();
	else
		Init( *m_pResList );

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResChooserDlg::OnNMDblclkTreeResources  Public
///
///	A message handler called when the user double-clicks on a tree item.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResChooserDlg::OnNMDblclkTreeResources(NMHDR *pNMHDR, LRESULT *pResult)
{
	*pResult = 0;

	// Get the selected item
	HTREEITEM hSelItem = m_ctlResTree.GetSelectedItem();

	// Get the resource ID
	m_SelResID = (ResourceID)m_ctlResTree.GetItemData( hSelItem );

	// End the dialog
	if( m_SelResID >= ResourceMgr::STARTING_RES_ID )
		OnOK();
	else
		OnCancel();
}
