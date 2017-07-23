// ExportOptionsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceMgrApp.h"
#include "ExportOptionsDlg.h"
#include "Base/NumFuncs.h"
#include "Base/StringFuncs.h"
#include "..\ResourceUtilDLL\ResourcePathsDLL\DirDialog.h"
#include "Base/RegKeyObj.h"
#include <set>
#include "ProgressDlg.h"


static const int RESLIST_SUBINDEX_NAME = 0;
static const int RESLIST_SUBINDEX_RESID = 1;
static const int RESLIST_SUBINDEX_OUTFILE = 2;
static const int RESLIST_SUBINDEX_DEFINEID = 3;

static const wchar_t* REGPATH_LastExportPath = _T("LastExportPath");

// CExportOptionsDlg dialog

IMPLEMENT_DYNAMIC(CExportOptionsDlg, CDialog)

CExportOptionsDlg::CExportOptionsDlg(ResourceToolsDB* pResDB, CWnd* pParent /*=NULL*/)
	: CDialog(CExportOptionsDlg::IDD, pParent),
		m_pResDB( pResDB ),
		m_OutputTemplate( OT_Default ),
		m_CurSelListIndex( -1 )
{

}

CExportOptionsDlg::~CExportOptionsDlg()
{
}

void CExportOptionsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_RES_LIST, m_ctlResList);
	DDX_Control(pDX, IDC_TEMPLATE_COMBO, m_ctlTemplateCombo);
	DDX_Control(pDX, IDC_PATH_EDIT, m_ctlPathEdit);
	DDX_Control(pDX, IDC_DESTFILE_COMBO, m_ctlDestFileCombo);
}


BEGIN_MESSAGE_MAP(CExportOptionsDlg, CDialog)
	ON_CBN_SELCHANGE(IDC_TEMPLATE_COMBO, &CExportOptionsDlg::OnCbnSelchangeTemplateCombo)
	ON_BN_CLICKED(IDC_SETPATH_BUTTON, &CExportOptionsDlg::OnBnClickedSetpathButton)
	ON_BN_CLICKED(IDOK, &CExportOptionsDlg::OnBnClickedOk)
	ON_CBN_EDITCHANGE(IDC_DESTFILE_COMBO, &CExportOptionsDlg::OnCbnEditchangeDestfileCombo)
	ON_NOTIFY(NM_CLICK, IDC_RES_LIST, &CExportOptionsDlg::OnNMClickResList)
	ON_NOTIFY(LVN_ITEMACTIVATE, IDC_RES_LIST, &CExportOptionsDlg::OnLvnItemActivateResList)
	ON_NOTIFY(HDN_ITEMCLICK, 0, &CExportOptionsDlg::OnHdnItemclickResList)
	ON_BN_CLICKED(IDC_SETALL_BUTTON, &CExportOptionsDlg::OnBnClickedSetallButton)
END_MESSAGE_MAP()


// CExportOptionsDlg message handlers


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CExportOptionsDlg::OnInitDialog  Protected
///
///	Called when the dialog is initialized.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CExportOptionsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add the templates
	m_ctlTemplateCombo.AddString( _T("Default") );
	m_ctlTemplateCombo.AddString( _T("All In One") );
	m_ctlTemplateCombo.AddString( _T("Custom") );
	m_ctlTemplateCombo.AddString( _T("Clear") );
	m_ctlTemplateCombo.SetCurSel( 0 );

	// Add the columns to the resource list
	m_ctlResList.InsertColumn( 0, _T("Resource Name"), LVCFMT_LEFT, 225, 0 );
	m_ctlResList.InsertColumn( 1, _T("ID"), LVCFMT_LEFT, 50, 1 );
	m_ctlResList.InsertColumn( 2, _T("Output File"), LVCFMT_LEFT, 120, 2 );
	m_ctlResList.InsertColumn( 3, _T("Resouce #Define"), LVCFMT_LEFT, 275, 3 );

	// Get the last export path
	RegKeyObj regKey;
	regKey.Init( RegKeyObj::RK_CurrentUser, ResourceToolsDB::REGPATH_RootKeyPath );
	std::wstring sExportDir;
	regKey.GetValue( REGPATH_LastExportPath, &sExportDir );
	m_ctlPathEdit.SetWindowTextW( sExportDir.c_str() );

	CImageList imgList;
	imgList.Create( 16, 16, ILC_COLOR24, 1, 1 );
	CBitmap warnImg;
	warnImg.LoadBitmapW( IDB_WARN_BITMAP );
	imgList.Add( &warnImg, RGB(255,255,255) );
	m_ctlResList.SetImageList( &imgList, LVSIL_STATE );
	
	// Get all of the known resources
	std::set<std::wstring> resIDDefines;
	if( m_pResDB )
	{
		const ResourceToolsDB::ResourceList& resList = m_pResDB->GetResList();
		for( ResourceToolsDB::ResourceList::const_iterator iterRes = resList.begin();
					iterRes != resList.end();
					++iterRes )
		{
			LVITEMW itemData;
			memset( &itemData, 0, sizeof(itemData) );

			// Create the item
			itemData.mask = LVIF_TEXT | LVIF_STATE | LVIF_PARAM;
			itemData.iItem = m_ctlResList.GetItemCount();
			itemData.iSubItem = 0;
			itemData.state = 0;
			itemData.lParam = iterRes->resID;
			itemData.pszText = (LPWSTR)iterRes->sName.c_str();
			int itemIndex = m_ctlResList.InsertItem( &itemData );
			if( itemIndex == -1 )
				continue;
			
			// Set the ID
			itemData.mask = LVIF_TEXT;
			itemData.iItem = itemIndex;
			itemData.iSubItem = 1;
			itemData.pszText = (LPWSTR)TCBase::EasyIToA( iterRes->resID );
			m_ctlResList.SetItem( &itemData );

			std::wstring sResID = iterRes->sResHeaderDefine;
			if( sResID.length() > 0 )
			{
				if( resIDDefines.find(sResID) != resIDDefines.end() )
					sResID = std::wstring(L"DUPEID - ") + sResID;
				else
					resIDDefines.insert( sResID );
			}

			// Set the resource define
			
			itemData.mask = LVIF_TEXT;
			itemData.iItem = itemIndex;
			itemData.iSubItem = 3;
			itemData.pszText = (LPWSTR)sResID.c_str();
			m_ctlResList.SetItem( &itemData );

			m_RetMap[ iterRes->resID ] = std::wstring(L"default.rdb");
		}
	}

	// Update the outputs
	UpdateListOutputs();

	// Disable the combo box since nothing is selected
	m_ctlDestFileCombo.EnableWindow( FALSE );
	
	// Return TRUE unless you set the focus to a control
	return TRUE;
}


ResourceID CExportOptionsDlg::GetResIDFromIndex( int itemIndex )
{
	LVITEMW itemData;
	memset( &itemData, 0, sizeof(itemData) );
	const int StrBuffer_SIZE = 128;
	wchar_t szStrBuffer[StrBuffer_SIZE] = {0};

	// Get the resource ID string
	itemData.iItem = itemIndex;
	itemData.iSubItem = RESLIST_SUBINDEX_RESID;
	itemData.mask = LVIF_TEXT;
	itemData.pszText = szStrBuffer;
	itemData.cchTextMax = StrBuffer_SIZE;
	if( m_ctlResList.GetItem( &itemData ) == 0 )
	{
		// We failed to get the resource ID text to display an error
		itemData.iItem = itemIndex;
		itemData.iSubItem = RESLIST_SUBINDEX_OUTFILE;
		itemData.mask = LVIF_TEXT;
		itemData.pszText = _T("Failed to get resID");
		m_ctlResList.SetItem( &itemData );
		return 0;
	}

	// Get the ID
	return (ResourceID)_wtoi( itemData.pszText );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CExportOptionsDlg::UpdateListOutputs  Protected
///
///	Update the resource list output files based on the selected template.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CExportOptionsDlg::UpdateListOutputs()
{
	// If the user is using custom then do nothing
	if( m_OutputTemplate == OT_Custom || !m_pResDB )
		return;

	// Data needed to update
	LVITEMW itemData;
	memset( &itemData, 0, sizeof(itemData) );
	const int StrBuffer_SIZE = 128;
	wchar_t szStrBuffer[StrBuffer_SIZE] = {0};
	ResourceToolsDB::ToolsResourceInfo resInfo;
	
	// Go through each resource
	for( int itemIndex = 0; itemIndex < m_ctlResList.GetItemCount(); ++itemIndex )
	{
		itemData.iItem = itemIndex;
		itemData.pszText = szStrBuffer;
		itemData.cchTextMax = StrBuffer_SIZE;

		// Get the resource ID
		ResourceID resID = GetResIDFromIndex( itemIndex );
		if( resID == 0 )
			continue;

		// Get the resource info
		if( !m_pResDB->GetResInfo( resID, resInfo ) )
		{
			// We failed to get the resource ID text to display an error
			itemData.iSubItem = RESLIST_SUBINDEX_OUTFILE;
			itemData.mask = LVIF_TEXT;
			itemData.pszText = _T("Failed to get resource info");
			m_ctlResList.SetItem( &itemData );
			continue;
		}

		// Set the output file
		itemData.iItem = itemIndex;
		itemData.iSubItem = RESLIST_SUBINDEX_OUTFILE;
		itemData.mask = LVIF_TEXT;
		
		// Default exporting
		if( m_OutputTemplate == OT_Default )
		{
			switch( resInfo.resType )
			{
			case RT_Font:
			case RT_Sprite:
				itemData.pszText = _T("game.rdb");
				break;

			case RT_Image:
				itemData.pszText = _T("visual.rdb");
				break;

			case RT_Music:
			case RT_Sound:
				itemData.pszText = _T("auditory.rdb");
				break;
			}
		}
		// All in one exporting
		else if( m_OutputTemplate == OT_AllInOne )
		{
			itemData.pszText = _T("gamedata.rdb");
		}
		// Clear
		else if( m_OutputTemplate == OT_Clear )
		{
			itemData.pszText = _T("");
		}

		m_RetMap[ resID ] = itemData.pszText;
		m_ctlResList.SetItem( &itemData );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CExportOptionsDlg::OnCbnSelchangeTemplateCombo  Private
///
///	A message handler called when the user selects a output template.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CExportOptionsDlg::OnCbnSelchangeTemplateCombo()
{
	m_OutputTemplate = (OutputTemplate)m_ctlTemplateCombo.GetCurSel();
	UpdateListOutputs();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CExportOptionsDlg::OnBnClickedSetpathButton  Private
///
///	A message handler called when the user presses the button to select an output path.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CExportOptionsDlg::OnBnClickedSetpathButton()
{
	// Create the directory browser to choose the output directory
	CBrowseDialog browseDlg;

	// Get the previous directory
	CString sCStringExportDir;
	m_ctlPathEdit.GetWindowText( sCStringExportDir );
	browseDlg.SetInitialDir( (LPCTSTR)sCStringExportDir );

	// Allow the user to choose a directory
	if( !browseDlg.DoBrowse() )
		return;
	std::wstring sExportDir( browseDlg.GetSelDir().c_str() );
	
	// Ensure a valid path string
	TCBase::EnsureValidPathSlashes( sExportDir );

	m_ctlPathEdit.SetWindowText( sExportDir.c_str() );
}


//CExportOptionsDlg* g_pExportDlg = 0;
CProgressDlg* g_pProgDlg = 0;

void ExportProgressCB( float32 normProg, const wchar_t* szFile )
{
	g_pProgDlg->SetProgress( normProg );
	g_pProgDlg->SetLabelText( szFile );

	// Handle messages
	MSG msg;
	while( ::PeekMessage( &msg, NULL, 0, 0, PM_NOREMOVE ) )
		theApp.PumpMessage();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CExportOptionsDlg::OnBnClickedOk  Private
///
///	A message handler called when the user presses the export button
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CExportOptionsDlg::OnBnClickedOk()
{
	// Get the export directory
	CString sCStringExportDir;
	m_ctlPathEdit.GetWindowText( sCStringExportDir );
	std::wstring sExportDir = (LPCTSTR)sCStringExportDir;

	if( sExportDir.size() == 0 )
	{
		MessageBox( L"Please select an output path", _T("Error"), MB_OK );
		return;
	}	

	// Ensure a valid path string
	TCBase::EnsureValidPathSlashes( sExportDir );

	g_pProgDlg = new CProgressDlg( this );
	g_pProgDlg->Create( CProgressDlg::IDD, this );
	g_pProgDlg->ShowWindow( SW_SHOW );

	// Export the data
	if( m_pResDB )
		m_pResDB->CompileFiles( sExportDir, m_RetMap, ExportProgressCB );
	
	g_pProgDlg->DestroyWindow();
	delete g_pProgDlg;
	g_pProgDlg = 0;

	// Store the selected directory for next time
	RegKeyObj regKey;
	regKey.Init( RegKeyObj::RK_CurrentUser, ResourceToolsDB::REGPATH_RootKeyPath );
	regKey.SetValue( REGPATH_LastExportPath, sExportDir.c_str() );

	// Close the dialog
	OnOK();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CExportOptionsDlg::OnCbnEditchangeDestfileCombo  Private
///
///	A message handler called when the user changes the text in the output file name combo box.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CExportOptionsDlg::OnCbnEditchangeDestfileCombo()
{
	// If the box is disabled then bail
	if( m_ctlDestFileCombo.IsWindowEnabled() == FALSE || m_CurSelListIndex == -1 )
		return;

	// Get the entered text
	CString enteredText;
	m_ctlDestFileCombo.GetWindowTextW( enteredText );

	// Initialize the structure to set the output file name
	LVITEMW itemData;
	memset( &itemData, 0, sizeof(itemData) );
	itemData.iItem = m_CurSelListIndex;
	itemData.iSubItem = RESLIST_SUBINDEX_OUTFILE;
	itemData.mask = LVIF_TEXT;
	itemData.pszText = (LPTSTR)(LPCTSTR)enteredText;

	// Get the resource ID
	ResourceID resID = GetResIDFromIndex( m_CurSelListIndex );

	// Set the output file
	m_RetMap[ resID ] = itemData.pszText;
	m_ctlResList.SetItem( &itemData );
}

void CExportOptionsDlg::OnNMClickResList(NMHDR *pHdr, LRESULT *pResult)
{
	
	OnLvnItemActivateResList( pHdr, pResult );
	// TODO: Add your control notification handler code here
	*pResult = 0;
}

void CExportOptionsDlg::OnLvnItemActivateResList(NMHDR *, LRESULT *pResult)
{
	//LPNMITEMACTIVATE pNMIA = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);

	// Get the currently selected list box item
	int curSelItem = m_ctlResList.GetSelectionMark();

	// If nothing new is selected then do nothing
	if( curSelItem == m_CurSelListIndex )
		return;

	// Store the selected index
	m_CurSelListIndex = curSelItem;

	// If nothing is selected then disable the dest file box
	if( m_CurSelListIndex == -1 )
	{
		m_ctlDestFileCombo.EnableWindow( FALSE );
		m_ctlDestFileCombo.SetWindowTextW( _T("") );
	}
	// Else update the box based on the selected item
	else
	{
		// Initialize the structure to retrieve the output file name
		LVITEMW itemData;
		memset( &itemData, 0, sizeof(itemData) );
		const int StrBuffer_SIZE = 128;
		wchar_t szStrBuffer[StrBuffer_SIZE] = {0};
		itemData.iItem = m_CurSelListIndex;
		itemData.iSubItem = RESLIST_SUBINDEX_OUTFILE;
		itemData.mask = LVIF_TEXT;
		itemData.pszText = szStrBuffer;
		itemData.cchTextMax = StrBuffer_SIZE;

		// Get the output file
		m_ctlResList.GetItem( &itemData );

		m_ctlDestFileCombo.SetWindowTextW( itemData.pszText );

		m_ctlDestFileCombo.EnableWindow( TRUE );
	}

	// TODO: Add your control notification handler code here
	*pResult = 0;
}

ResourceToolsDB* g_pSortResDB = 0;
int g_FlipSort = 1;

int CALLBACK CompareFunc(LPARAM lParam1, LPARAM lParam2, LPARAM sortParam)
{
	int colIndex = (int)sortParam;

	ResourceToolsDB::ToolsResourceInfo resItem1;
	g_pSortResDB->GetResInfo( (ResourceID)lParam1, resItem1 );

	ResourceToolsDB::ToolsResourceInfo resItem2;
	g_pSortResDB->GetResInfo( (ResourceID)lParam2, resItem2 );

	int cmpValue = 0;
	if( colIndex == RESLIST_SUBINDEX_NAME )
		cmpValue = _wcsicmp( resItem1.sName.c_str(), resItem2.sName.c_str() );
	else if( colIndex == RESLIST_SUBINDEX_RESID )
		cmpValue = (resItem1.resID > resItem2.resID) ? -1 : 1;
	else
		cmpValue = _wcsicmp( resItem1.sResHeaderDefine.c_str(), resItem2.sResHeaderDefine.c_str() );

	return ((cmpValue > 0) ? 1 : -1) * g_FlipSort;
}

void CExportOptionsDlg::OnHdnItemclickResList(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMHEADER phdr = reinterpret_cast<LPNMHEADER>(pNMHDR);
	
	g_FlipSort *= -1;

	// Get the column that was selected
	g_pSortResDB = m_pResDB;
	m_ctlResList.SortItems( CompareFunc, phdr->iItem);

	*pResult = 0;
}

void CExportOptionsDlg::OnBnClickedSetallButton()
{
	// Get the output file for the current resource
	CString destFile;
	m_ctlDestFileCombo.GetWindowTextW(destFile);

	// Data needed to update
	LVITEMW itemData;
	memset( &itemData, 0, sizeof(itemData) );
	const int StrBuffer_SIZE = 128;
	wchar_t szStrBuffer[StrBuffer_SIZE] = {0};
	ResourceToolsDB::ToolsResourceInfo resInfo;
	
	// Go through each resource
	for( int itemIndex = 0; itemIndex < m_ctlResList.GetItemCount(); ++itemIndex )
	{
		itemData.iItem = itemIndex;
		itemData.pszText = szStrBuffer;
		itemData.cchTextMax = StrBuffer_SIZE;

		// Get the resource ID
		ResourceID resID = GetResIDFromIndex( itemIndex );
		if( resID == 0 )
			continue;

		// Get the resource info
		if( !m_pResDB->GetResInfo( resID, resInfo ) )
		{
			// We failed to get the resource ID text to display an error
			itemData.iSubItem = RESLIST_SUBINDEX_OUTFILE;
			itemData.mask = LVIF_TEXT;
			itemData.pszText = _T("Failed to get resource info");
			m_ctlResList.SetItem( &itemData );
			continue;
		}

		// Set the output file
		itemData.iItem = itemIndex;
		itemData.iSubItem = RESLIST_SUBINDEX_OUTFILE;
		itemData.mask = LVIF_TEXT;
		
		itemData.pszText = const_cast<LPWSTR>( (LPCWSTR)destFile );

		m_RetMap[ resID ] = itemData.pszText;
		m_ctlResList.SetItem( &itemData );
	}
}
