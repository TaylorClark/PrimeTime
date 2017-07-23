// ResourceMgrDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ResourceMgrApp.h"
#include "ResourceMgrDlg.h"
#include "Base/FourCC.h"
#include <fstream>
#include "Base/StringFuncs.h"
#include "Base/FileFuncs.h"
#include "Base/NumFuncs.h"
#include "Base/RegKeyObj.h"
#include "ResourcePropDlg.h"
#include "ExportOptionsDlg.h"
#include "..\ResourceUtilDLL\ResourcePathsDLL\DirDialog.h"

//#include "ResourcePathsDLL/ResourcePathsDLL/ResPathDLL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CResourceMgrDlg dialog



CResourceMgrDlg::CResourceMgrDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CResourceMgrDlg::IDD, pParent),
		_hasChanges( false )
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CResourceMgrDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_REZTREE, m_ResourceTreeCtrl);
	DDX_Control(pDX, IDC_NEXTRESID_EDIT, m_ctlNextResID);
	DDX_Control(pDX, IDC_ROOTRESPATH_EDIT, _resRootPathEdit);
}

BEGIN_MESSAGE_MAP(CResourceMgrDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	ON_WM_GETMINMAXINFO()
	ON_BN_CLICKED(IDADDREZ, OnBnClickedAddrez)
	ON_BN_CLICKED(IDREMOVEREZ, OnBnClickedRemoveRez)
	ON_BN_CLICKED(IDEXPORTDB, OnBnClickedExportDB)
	ON_BN_CLICKED(IDC_OPENINDEX, OnBnClickedOpenIndex)
	ON_BN_CLICKED(IDC_SAVEINDEX2, OnBnClickedSaveIndex)
	ON_BN_CLICKED(IDC_RESET_BUTTON, OnBnClickedClear)
	ON_BN_CLICKED(IDC_BUTTON_PATHSOPTIONS, OnBnClickedPathsOptions)
	ON_NOTIFY(NM_RCLICK, IDC_REZTREE, &CResourceMgrDlg::OnNMRclickReztree)
	ON_NOTIFY(TVN_KEYDOWN, IDC_REZTREE, &CResourceMgrDlg::OnKeyDownResTree)
	ON_EN_CHANGE( IDC_NEXTRESID_EDIT, OnNextResIDEditChange)
	ON_EN_KILLFOCUS( IDC_NEXTRESID_EDIT, OnNextResIDEditLoseFocus )
	ON_EN_CHANGE(IDC_ROOTRESPATH_EDIT, &CResourceMgrDlg::OnEnChangeRootrespathEdit)
	ON_BN_CLICKED(IDC_SELECTROOTRESPATH_BUTTON, &CResourceMgrDlg::OnBnClickedSelectrootrespathButton)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnInitDialog  Protected
///
///	A message handler called when the dialog is initialized.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
BOOL CResourceMgrDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Store the window title
	CString sWinTitle;
	GetWindowText( sWinTitle );
	m_sOrigWinTitle = (const wchar_t*)sWinTitle;

	// Get the resource path
	/*RegKeyObj regKey;
	regKey.Init( RegKeyObj::RK_CurrentUser, ResourceToolsDB::REGPATH_RootKeyPath );
	std::wstring sRegVal;
	if( regKey.GetValue( ResourceToolsDB::REGPATH_ResPath, &sRegVal ) )
		m_ResDB.SetResPath( sRegVal );
	else
		MessageBox( _T("Failed to retrieve resource path from registry, please use the 'Paths' button to set the paths before using the tool."), _T("Resource Manager"), MB_OK | MB_ICONERROR );
	*/
	// Initialize the tree
	RefreshTreeFromDB();

	m_ctlNextResID.SetWindowText( _T("100") );

	// Return TRUE unless you set the focus to a control
	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnSysCommand  Public
///
///	A message handler for when we receive a system message
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CResourceMgrDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::GetTreeItemParent  Protected
///
///	\param resType The type of the resource
///	\returns The tree item that is the parent of these types of resources items, NULL if the type
///				is unrecognized.
///
///	Get a resource's parent tree item based on the resource type
///
///////////////////////////////////////////////////////////////////////////////////////////////////
HTREEITEM CResourceMgrDlg::GetTreeItemParent( EResourceType resType )
{
	// Retrieve the parent tree item based on the resource type
	switch( resType )
	{
	case RT_Image:
		return m_hIndImageTreeItem;

	case RT_Sound:
		return m_hIndSoundTreeItem;

	case RT_Music:
		return m_hIndMusicTreeItem;

	case RT_Font:
		return m_hGameFontTreeItem;

	case RT_Sprite:
		return m_hGameSpriteTreeItem;
	}
	
	// If we don't know the type then return a NULL value
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::RefreshTreeFromDB  Protected
///
///	Refresh the data in the resource tree to reflect the resources in the database.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::RefreshTreeFromDB()
{
	// Clear the tree
	if( m_ResourceTreeCtrl.GetCount() > 0 )
		m_ResourceTreeCtrl.DeleteAllItems();
	
	// Initialize the add item structure
	TVINSERTSTRUCT newTreeItem;
	newTreeItem.hInsertAfter = NULL;
	newTreeItem.hParent = NULL;
	newTreeItem.item.mask = 0;
	newTreeItem.itemex.mask = TVIF_TEXT | TVIF_PARAM;
	newTreeItem.itemex.lParam = 0;
	
	// Add the independent resource root tree item
	newTreeItem.itemex.pszText = _T("Independent Resource");
	m_hIndependentRezTreeItem = m_ResourceTreeCtrl.InsertItem( &newTreeItem );
	
	// Add the independent resource roots
	newTreeItem.hParent = m_hIndependentRezTreeItem;
	newTreeItem.itemex.pszText = _T("Image");
	m_hIndImageTreeItem = m_ResourceTreeCtrl.InsertItem( &newTreeItem );
	
	newTreeItem.itemex.pszText = _T("Sound");
	m_hIndSoundTreeItem = m_ResourceTreeCtrl.InsertItem( &newTreeItem );
	
	newTreeItem.itemex.pszText = _T("Music");
	m_hIndMusicTreeItem = m_ResourceTreeCtrl.InsertItem( &newTreeItem );
	
	// Add the game resource root
	newTreeItem.hParent = NULL;
	newTreeItem.itemex.pszText = _T("Game Resource");
	m_hGameRezTreeItem = m_ResourceTreeCtrl.InsertItem( &newTreeItem );

	// Add the game resource sub types
	newTreeItem.hParent = m_hGameRezTreeItem;
	newTreeItem.itemex.pszText = _T("Font");
	m_hGameFontTreeItem = m_ResourceTreeCtrl.InsertItem( &newTreeItem );
	
	newTreeItem.itemex.pszText = _T("Sprite");
	m_hGameSpriteTreeItem = m_ResourceTreeCtrl.InsertItem( &newTreeItem );
	
	// Go through all of the resources
	const ResourceToolsDB::ResourceList& resList = m_ResDB.GetResList();
	for( ResourceToolsDB::ResourceList::const_iterator iterRes = resList.begin(); iterRes != resList.end(); ++iterRes )
		AddResToTree( *iterRes );

	// Expand the tree root items
	m_ResourceTreeCtrl.Expand( m_hIndependentRezTreeItem, TVE_EXPAND );
	m_ResourceTreeCtrl.Expand( m_hGameRezTreeItem, TVE_EXPAND );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::AddResToTree  Protected
///
///	\param res The resource to add to the tree
///
///	Add a resource to the tree.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::AddResToTree( const ResourceToolsDB::ToolsResourceInfo& res )
{
	// Get this resource's parent item in the tree
	HTREEITEM hParentTreeItem = GetTreeItemParent( res.resType );
	
	// Insert the resource item
	if( hParentTreeItem )
	{
		// Create the text to display for the item
		std::wstring sText;
		if( res.sName.length() < 1 )
			sText += _T("UNNAMED");
		else
			sText += res.sName;
		sText += _T(" (");
		sText += res.sFilePath;
		sText += _T(")");

		// Initialize the add item structure
		TVINSERTSTRUCT newTreeItem;
		newTreeItem.hInsertAfter = NULL;
		newTreeItem.hParent = hParentTreeItem;
		newTreeItem.item.mask = 0;
		newTreeItem.itemex.mask = TVIF_TEXT | TVIF_PARAM;
		newTreeItem.itemex.pszText = (LPWSTR)sText.c_str();
		newTreeItem.itemex.lParam = (LPARAM)res.resID;

		// Add the item
		m_ResourceTreeCtrl.InsertItem( &newTreeItem );
		m_ResourceTreeCtrl.RedrawWindow();
	}
	// Else there is no parent tree item so display an error
	else
	{
		std::wstring sMsg = _T("The file \"");
		sMsg += res.sFilePath;
		sMsg += _T("\" has an unrecognized resource type and will not be add to the tree.");
		MessageBox( sMsg.c_str(), _T("Error"), MB_OK );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnQueryDragIcon  Protected
///
///	The system calls this function to obtain the cursor to display while the user drags
///	the minimized window.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
HCURSOR CResourceMgrDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnGetMinMaxInfo  Public
///
///	\param lpMMI The minmax info for the window to be filled in
///
///	Get the minimum and maximum size
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO: Add your message handler code here and/or call default
	lpMMI->ptMinTrackSize.x = 300;
	lpMMI->ptMinTrackSize.y = 250;
	CDialog::OnGetMinMaxInfo(lpMMI);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::AddRes  Protected
///
///	Add a file to the tools database and update the tree accordingly.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::AddRes( const std::wstring& sFileName )
{
	// If the file could not be added to the database
	ResourceToolsDB::ToolsResourceInfo newRes;
	if( !m_ResDB.AddFile( sFileName, &newRes ) )
	{
		// Display an error message
		std::wstring sErrorMsg = _T("The file '");
		sErrorMsg += sFileName;
		sErrorMsg += _T("' is not a valid resource.");
		MessageBox( sErrorMsg.c_str(), _T("Error"), MB_OK );
	}
	// Else add the resource to the tree
	else
	{
		AddResToTree( newRes );
		SetNextResIDText( m_ResDB.GetNextResID() );
	}
}

void CResourceMgrDlg::SetNextResIDText( uint32 nextResID )
{
	m_ctlNextResID.SetWindowText( TCBase::EasyUIToA( nextResID ) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnBnClickedAddrez  Public
///
///	A message handler called when the user presses the "Add Resource" button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnBnClickedAddrez()
{
	const wchar_t* FILE_FILTERS = _T("All Resource Files|*.bmp;*.jpg;*.wav;*.mp3;*.spr;*.fnt;*.ogg|Independent Resources (*.bmp,*.jpg,*.wav,*.mp3,*.ogg)")\
		_T("|*.bmp;*.jpg;*.wav;*.mp3;*.ogg|Game Resources (*.spr,*.fnt)|*.spr;*.fnt|All Files (*.*)|*.*||");

	CFileDialog fileDlg( true, NULL, NULL, OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, FILE_FILTERS, this );

	fileDlg.m_pOFN->lpstrInitialDir = m_ResDB.GetResPath().c_str();
	
	// Initialize a buffer to store the file names
	const int MAX_SEL_FILE_COUNT = 32;
	const size_t CHAR_BUFFER_SIZE = MAX_SEL_FILE_COUNT * (_MAX_PATH + 1) + 1;
	wchar_t* szSelFileBuffer = new wchar_t[ CHAR_BUFFER_SIZE ];
	memset( szSelFileBuffer, 0, CHAR_BUFFER_SIZE );
	fileDlg.GetOFN().lpstrFile = szSelFileBuffer;
	fileDlg.GetOFN().nMaxFile = CHAR_BUFFER_SIZE;

	// Open the open file dialog
	if( fileDlg.DoModal() != IDOK )
	{
		delete [] szSelFileBuffer;
		return;
	}

	// Make sure the string is valid
	if( wcslen( szSelFileBuffer ) < 1 )
	{
		delete [] szSelFileBuffer;
		return;
	}

	// Read the directory path
	std::wstring sDir = szSelFileBuffer;
	size_t curBufferPos = wcslen( szSelFileBuffer ) + 1;

	// If we are at a NULL then there is only 1 file selected
	if( szSelFileBuffer[curBufferPos] == 0 )
	{
		// Attempt to add the resource
		AddRes( std::wstring( szSelFileBuffer ) );

		// Free the string buffer
		delete [] szSelFileBuffer;
		return;
	}

	// Ensure a trailing slash
	if( sDir[ sDir.length() - 1 ] != _T('\\') )
		sDir += _T("\\");

	// Go through the files picked
	while( szSelFileBuffer[curBufferPos] != NULL )
	{
		// Add the resource
		std::wstring sFullFile = sDir + (szSelFileBuffer + curBufferPos);
		AddRes( sFullFile );

		// Step past this string
		curBufferPos = curBufferPos + wcslen( szSelFileBuffer + curBufferPos ) + 1;
	}

	_hasChanges = true;

	// Free the string buffer
	delete [] szSelFileBuffer;
}

typedef bool (*ResChooseFunc)(ResourceID*);


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnBnClickedRemoveRez  Public
///
///	A message handler called when the user presses the "Remove Resource" button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnBnClickedRemoveRez()
{
	// Get the selection
	HTREEITEM hSelItem = m_ResourceTreeCtrl.GetSelectedItem();
	if( !hSelItem )
		return;

	// Get the ID
	unsigned int resID = (int)m_ResourceTreeCtrl.GetItemData( hSelItem );
	if( resID < ResourceMgr::STARTING_RES_ID )
		return;

	// Remove the resource
	if( !m_ResDB.RemoveResource( resID ) )
		MessageBox( _T("Failed to remove the resource."), _T("Error"), MB_OK );

	// Remove the item
	m_ResourceTreeCtrl.DeleteItem( hSelItem );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnKeyDownResTree  Public
///
///	A message handler called when the user presses a keyboard key on the resource tree control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnKeyDownResTree(NMHDR *pNMHDR, LRESULT *)
{
	NMTVKEYDOWN* pKeyDown = (NMTVKEYDOWN*)pNMHDR;
	if( pKeyDown->wVKey == VK_DELETE )
	{
		// Call the remove item function
		OnBnClickedRemoveRez();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnBnClickedPathsOptions  Public
///
///	A message handler called when the user presses the "Paths" button.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnBnClickedPathsOptions()
{
	typedef bool (*RunFunc)(void);

	// Load the DLL
	HMODULE hLib = LoadLibrary( _T("ResPathDlg_dbg.dll") );
	if( !hLib )
	{
		MessageBox(_T("Failed to find resource DLL"));
		return;
	}

	// Open the paths dialog
	RunFunc pDlgFunc = (RunFunc)GetProcAddress( hLib, "RunResDlg" );
	if( pDlgFunc )
	{
		// If the user hit OK
		if( pDlgFunc() )
		{
			/*
			RegKeyObj regKey;
			regKey.Init( RegKeyObj::RK_CurrentUser, ResourceToolsDB::REGPATH_RootKeyPath );
			std::wstring sRegVal;
			if( !regKey.GetValue( ResourceToolsDB::REGPATH_ResPath, &sRegVal ) )
				MessageBox( _T("Failed to get resource path from registry"), _T("Error"), MB_OK );

			// Update the database
			m_ResDB.SetResPath( sRegVal );
			*/
		}
	}

	// Unload the DLL
	FreeLibrary( hLib );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnBnClickedExportDB  Public
///
///	A message handler called when the user presses the 'export db' button
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnBnClickedExportDB()
{
	CExportOptionsDlg exportDlg( &m_ResDB );
	exportDlg.DoModal();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnBnClickedOpenIndex  Public
///
///	A message handler called when the user presses the 'open index' button
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnBnClickedOpenIndex()
{
	const wchar_t* FILE_FILTERS = _T("Resource Index Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	CFileDialog openFileDialog( true, NULL, NULL, OFN_HIDEREADONLY | OFN_FILEMUSTEXIST, FILE_FILTERS, this );

	openFileDialog.m_pOFN->lpstrInitialDir = m_ResDB.GetResPath().c_str();
	
	// Open the open file dialog
	if( openFileDialog.DoModal() != IDOK )
		return;

	// Make sure the string is valid
	if( openFileDialog.GetFileName().GetLength()< 1 )
		return;

	// Open the database
	ResourceToolsDB newDB;
	if( newDB.LoadToolsIndex( (LPCTSTR)openFileDialog.GetPathName() ) )
	{
		// Store the loaded database
		m_ResDB = newDB;

		// Reseet the resource path
		//m_ResDB.SetResPath( sResPath );

		_resRootPathEdit.SetWindowTextW( m_ResDB.GetResPath().c_str() );

		// Store the file name and update the title
		UpdateWindowTitle();

		// Refresh the interface
		RefreshTreeFromDB();

		// Update the next resource ID
		SetNextResIDText( m_ResDB.GetNextResID() );
	}
	else
		MessageBox( _T("Failed to loaded resource index."), _T("Error"), MB_OK );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnBnClickedSaveIndex  Public
///
///	A message handler called when the user presses the 'save index' button
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnBnClickedSaveIndex()
{
	const wchar_t* FILE_FILTERS = _T("Resource Index Files (*.dat)|*.dat|All Files (*.*)|*.*||");

	// Get the number of resources
	unsigned int numRes = (unsigned int)m_ResDB.GetResList().size();
	if( numRes == 0 )
	{
		MessageBox( _T("Can not save empty index file."), _T("Index Not Saved"), MB_OK );
		return;
	}

	CFileDialog saveFileDialog( FALSE, NULL, NULL, OFN_HIDEREADONLY, FILE_FILTERS, this );

	saveFileDialog.m_pOFN->lpstrInitialDir = m_ResDB.GetResPath().c_str();

	// Open the save file dialog
	if( saveFileDialog.DoModal() != IDOK )
		return;

	// Create the file name
	std::wstring sResIndexFile = m_ResDB.GetResPath();
	sResIndexFile += ResourceToolsDB::FILENAME_ToolsResIndex;

	sResIndexFile = saveFileDialog.GetPathName();

	// Attempt to save the file
	if( !m_ResDB.SaveToolsIndex( sResIndexFile ) )
	{
		MessageBox( _T("Failed to save index file, ensure the directory exists and is not read-only."), _T("Error"), MB_OK );
	}
	else
		_hasChanges = false;

	// Store the file name and update the title
	UpdateWindowTitle();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnBnClickedClear  Public
///
///	A message handler called when the user presses the 'Clear' button
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnBnClickedClear()
{
	if( _hasChanges )
	{
		int msgBoxResult = MessageBox( _T("You (may) have unsaved changes. Are you sure you want to reset?"), _T("Reset?"), MB_YESNO | MB_ICONHAND );
		if( msgBoxResult == IDNO )
			return;
	}

	std::wstring sResPath = m_ResDB.GetResPath();

	// Clear the resource database
	m_ResDB.Clear();

	// Reseet the resource path
	m_ResDB.SetResPath( sResPath.c_str() );

	// Store the file name and update the title
	UpdateWindowTitle();

	// Refresh the interface
	RefreshTreeFromDB();

	// Clear the next resource ID
	SetNextResIDText( m_ResDB.GetNextResID() );

	_hasChanges = false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CResourceMgrDlg::OnSize  Public
///
///	A message handler called when the window is resized
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CResourceMgrDlg::OnSize( UINT type, int cx, int cy )
{
	static POINT s_OldSize = {-1, -1 };

	// Store the values at first
	if( s_OldSize.x == -1 || s_OldSize.y == -1 )
	{
		s_OldSize.x = cx;
		s_OldSize.y = cy;
		return;
	}

	// Handle changes only
	if( type != SIZE_RESTORED && type != SIZE_MAXIMIZED )
		return;

	// Get the change in size
	POINT change = { cx - s_OldSize.x, cy - s_OldSize.y };
	s_OldSize.x = cx;
	s_OldSize.y = cy;
	if( change.x == 0 && change.y == 0 )
		return;

	// Update right anchored controls
	const int RIGHT_ANCHOR_CTL_IDS[] = { IDC_OPENINDEX, IDC_SAVEINDEX2, IDEXPORTDB, IDADDREZ, IDREMOVEREZ, IDC_RESET_BUTTON, IDC_BUTTON_PATHSOPTIONS, IDC_NEXTRESID_EDIT, IDC_NEXTRESID_LABEL };
	int numCtls = sizeof(RIGHT_ANCHOR_CTL_IDS) / sizeof(RIGHT_ANCHOR_CTL_IDS[0]);
	for( int ctlIndex = 0; ctlIndex < numCtls; ++ctlIndex )
	{
		// Get the control's rectangle
		CWnd* pCurWnd = GetDlgItem( RIGHT_ANCHOR_CTL_IDS[ctlIndex] );
		if( !pCurWnd )
			continue;
		RECT winRect;
		pCurWnd->GetWindowRect( &winRect );
		ScreenToClient( &winRect );

		// Move it
		winRect.left += change.x;
		winRect.right += change.x;

		// Update the control
		pCurWnd->MoveWindow( &winRect, 1 );
	}

	// Update right-left-top-bottom controls
	const int FULL_ANCHOR_CTL_IDS[] = { IDC_RESGROUP, IDC_REZTREE };
	numCtls = sizeof(FULL_ANCHOR_CTL_IDS) / sizeof(FULL_ANCHOR_CTL_IDS[0]);
	for( int ctlIndex = 0; ctlIndex < numCtls; ++ctlIndex )
	{
		// Get the control's rectangle
		CWnd* pCurWnd = GetDlgItem( FULL_ANCHOR_CTL_IDS[ctlIndex] );
		if( !pCurWnd )
			continue;
		RECT winRect;
		pCurWnd->GetWindowRect( &winRect );
		ScreenToClient( &winRect );

		// Resize it
		winRect.right += change.x;
		winRect.bottom += change.y;

		// Update the control
		pCurWnd->MoveWindow( &winRect, 1 );
	}

	// Update left-bottom controls
	const int LEFT_BOTTOM_ANCHOR_CTL_IDS[] = { IDC_SELECTROOTRESPATH_BUTTON};
	numCtls = sizeof(LEFT_BOTTOM_ANCHOR_CTL_IDS) / sizeof(LEFT_BOTTOM_ANCHOR_CTL_IDS[0]);
	for( int ctlIndex = 0; ctlIndex < numCtls; ++ctlIndex )
	{
		// Get the control's rectangle
		CWnd* pCurWnd = GetDlgItem( LEFT_BOTTOM_ANCHOR_CTL_IDS[ctlIndex] );
		if( !pCurWnd )
			continue;
		RECT winRect;
		pCurWnd->GetWindowRect( &winRect );
		ScreenToClient( &winRect );

		// Resize it
		winRect.left += change.x;
		winRect.right += change.x;
		winRect.top += change.y;
		winRect.bottom += change.y;

		// Update the control
		pCurWnd->MoveWindow( &winRect, 1 );
	}

	// Update right-left-bottom controls
	const int RIGHT_LEFT_BOTTOM_ANCHOR_CTL_IDS[] = { IDC_ROOTRESPATH_EDIT  };
	numCtls = sizeof(RIGHT_LEFT_BOTTOM_ANCHOR_CTL_IDS) / sizeof(RIGHT_LEFT_BOTTOM_ANCHOR_CTL_IDS[0]);
	for( int ctlIndex = 0; ctlIndex < numCtls; ++ctlIndex )
	{
		// Get the control's rectangle
		CWnd* pCurWnd = GetDlgItem( RIGHT_LEFT_BOTTOM_ANCHOR_CTL_IDS[ctlIndex] );
		if( !pCurWnd )
			continue;
		RECT winRect;
		pCurWnd->GetWindowRect( &winRect );
		ScreenToClient( &winRect );

		// Resize it
		winRect.right += change.x;
		winRect.top += change.y;
		winRect.bottom += change.y;

		// Update the control
		pCurWnd->MoveWindow( &winRect, 1 );
	}


	// Update bottom controls
	const int BOTTOM_ANCHOR_CTL_IDS[] = { IDC_ROOTRESPATH_LABEL };
	numCtls = sizeof(BOTTOM_ANCHOR_CTL_IDS) / sizeof(BOTTOM_ANCHOR_CTL_IDS[0]);
	for( int ctlIndex = 0; ctlIndex < numCtls; ++ctlIndex )
	{
		// Get the control's rectangle
		CWnd* pCurWnd = GetDlgItem( BOTTOM_ANCHOR_CTL_IDS[ctlIndex] );
		if( !pCurWnd )
			continue;
		RECT winRect;
		pCurWnd->GetWindowRect( &winRect );
		ScreenToClient( &winRect );

		// Resize it
		winRect.top += change.y;
		winRect.bottom += change.y;

		// Update the control
		pCurWnd->MoveWindow( &winRect, 1 );
	}
}


void CResourceMgrDlg::UpdateWindowTitle()
{
	// If there is no path then reset the title
	//if( m_sCurDBFile.length() < 1 )
	//{
	//	SetWindowText( m_sOrigWinTitle.c_str() );
	//	return;
	//}

	//// Get the file name with extension
	//wchar_t szFile[ _MAX_PATH ] = {0};
	//wchar_t szExt[ _MAX_PATH ] = {0};
	//_wsplitpath_s( m_sCurDBFile.c_str(), NULL, 0, NULL, 0, szFile, _MAX_PATH, szExt, _MAX_PATH );
	//std::wstring sFileWithExt = szFile;
	//sFileWithExt += szExt;

	//// Update the title
	//SetWindowText( (m_sOrigWinTitle + _T(" - ") + sFileWithExt).c_str() );
}


void CResourceMgrDlg::OnNMRclickReztree(NMHDR *, LRESULT *pResult)
{
	*pResult = 0;

	// Get the mouse position in tree view coordinates
	POINT clientPt;
	GetCursorPos( &clientPt );
	m_ResourceTreeCtrl.ScreenToClient( &clientPt );

	// Get the selection at the point
	CPoint clientPos( clientPt );
	HTREEITEM hTreeItem = m_ResourceTreeCtrl.HitTest( clientPos );
	if( hTreeItem == NULL )
		return;

	// Get the resource ID of the item
	unsigned int resID = (int)m_ResourceTreeCtrl.GetItemData( hTreeItem );
	if( resID < ResourceMgr::STARTING_RES_ID )
		return;

	// Get the resource info
	ResourceToolsDB::ToolsResourceInfo resInfo;
	if( !m_ResDB.GetResInfo( resID, resInfo ) )
		return;

	// Open the resource properties dialog
	CResourcePropDlg dlg( resInfo, m_ResDB.GetResPath() );
	if( dlg.DoModal() != IDOK )
		return;

	// The user might not have made changes, but flag it anyways to be safe
	_hasChanges = true;

	try
	{
		// Update the data
		m_ResDB.UpdateResourceInfo( resID, resInfo );
	}
	catch( const wchar_t* szMsg )
	{
		MessageBox( szMsg, _T("Failed To Change Resource ID"), MB_OK );
		resInfo.resID = resID;
	}

	// Create the text to display for the item
	std::wstring sText;
	if( resInfo.sName.length() < 1 )
		sText += _T("UNNAMED");
	else
		sText += resInfo.sName;
	sText += _T(" (");
	sText += resInfo.sFilePath;
	sText += _T(")");

	// Refresh the tree item
	m_ResourceTreeCtrl.SetItemText( hTreeItem, sText.c_str() );
	m_ResourceTreeCtrl.SetItemData( hTreeItem, (DWORD_PTR)resInfo.resID );
}


void CResourceMgrDlg::OnNextResIDEditChange()
{
	// Get the new value
	CString sVal;
	m_ctlNextResID.GetWindowText( sVal );
#ifdef UNICODE
	int newResID = _wtoi( (LPCTSTR)sVal );
#else
	int newResID = atoi( (LPCTSTR)sVal );
#endif

	if( newResID <= 0 || newResID == m_ResDB.GetNextResID() )
		return;

	// Attempt to set the next resource ID
	m_ResDB.SetNextResID( (uint32)newResID );
}

void CResourceMgrDlg::OnNextResIDEditLoseFocus()
{
	// Validate the text if invalid

	// Get the new value
	CString sVal;
	m_ctlNextResID.GetWindowText( sVal );
#ifdef UNICODE
	int newResID = _wtoi( (LPCTSTR)sVal );
#else
	int newResID = atoi( (LPCTSTR)sVal );
#endif

	bool isInvalid = newResID == 0 && sVal != _T("0");
	if( !isInvalid )
		isInvalid = !m_ResDB.SetNextResID( (uint32)newResID );
	if( isInvalid )
		MessageBox( _T("That resource ID is not valid, please reenter a value or manually set the ID by right-clicking on a resource."), _T("Resource Manager") );

	m_ctlNextResID.SetWindowText( TCBase::EasyIToA( m_ResDB.GetNextResID() ) );
}
void CResourceMgrDlg::OnEnChangeRootrespathEdit()
{
	// TODO:  If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.

	CString sResPath;
	_resRootPathEdit.GetWindowTextW(sResPath);

	m_ResDB.SetResPath( (LPCTSTR)sResPath );
}

void CResourceMgrDlg::OnBnClickedSelectrootrespathButton()
{
	// Bring up a directory chooser
	CBrowseDialog dirDlg;
	if( !dirDlg.DoBrowse() )
		return;

	// Store the path
	_resRootPathEdit.SetWindowTextW(dirDlg.GetSelDir().c_str());
}


void CResourceMgrDlg::OnClose()
{
	if( _hasChanges )
	{
		int msgBoxResult = MessageBox( _T("You (may) have unsaved changes. Are you sure you want to close?"), _T("Close?"), MB_YESNO | MB_ICONHAND );
		if( msgBoxResult == IDNO )
			return;
	}

	CDialog::OnClose();
}
