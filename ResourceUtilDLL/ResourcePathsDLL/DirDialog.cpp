#include "stdafx.h"
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include "DirDialog.h"
#include "Base/StringFuncs.h"


// Callback function called by SHBrowseForFolder's browse control
// after initialization and when selection changes
int __stdcall CBrowseDialog::BrowseCtrlCallback(HWND hwnd, UINT uMsg, LPARAM, LPARAM lpData)
{
	CBrowseDialog* pDirDialogObj = (CBrowseDialog*)lpData;
	
	if (uMsg == BFFM_INITIALIZED && pDirDialogObj->m_strSelDir[0] != 0 )
	{
		::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)(LPCTSTR)(pDirDialogObj->m_strSelDir));
	}
	else // uMsg == BFFM_SELCHANGED
	{
	}
	
	return 0;
}

CBrowseDialog::CBrowseDialog() :	m_Flags(0),
									m_InternalFlags(0)
{
	m_hParent = NULL;
	
	wcscpy_s( m_strTitle, _MAX_PATH, L"Select a folder" );
	
	m_strInitDir[0] = 0;
	m_strSelDir[0] = 0;
	
	SetMode( DIRECTORIES );
}

CBrowseDialog::~CBrowseDialog()
{
}

void CBrowseDialog::SetMode(Mode mode)
{
	switch(mode)
	{
	case DIRECTORIES:
		m_InternalFlags = BIF_RETURNFSANCESTORS | BIF_RETURNONLYFSDIRS;
		break;
	case PRINTERS:
		m_InternalFlags = BIF_BROWSEFORPRINTER;
		break;
	case COMPUTERS:
		m_InternalFlags = BIF_BROWSEFORCOMPUTER;
		break;
	};
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	CBrowseDialog::SetInitialDir  Public
///
///	\param sDir The initial directory string
///
///	Set the directory that is selected when the window is displayed.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void CBrowseDialog::SetInitialDir( const std::wstring& sDir )
{
	// Store the directory
	wcscpy_s( m_strSelDir, _MAX_PATH, sDir.c_str() );
	//wcscpy_s( m_strInitDir, _MAX_PATH, sDir.c_str() );
}

bool CBrowseDialog::DoBrowse()
{
	LPMALLOC pMalloc;
	if( SHGetMalloc( &pMalloc ) != NOERROR )
		return false;
	
	BROWSEINFO bInfo;
	LPITEMIDLIST pidl;
	ZeroMemory( (PVOID)&bInfo, sizeof( BROWSEINFO ) );
	
	if( m_strInitDir[0] != 0 )
	{
		ULONG         chEaten;
		ULONG         dwAttributes;
		HRESULT       hr;
		LPSHELLFOLDER pDesktopFolder;
		// 
		// Get a pointer to the Desktop's IShellFolder interface. 
		//
		if( SUCCEEDED( SHGetDesktopFolder( &pDesktopFolder ) ) )
		{
			//
			// Convert the path to an ITEMIDLIST.
			//
			hr = pDesktopFolder->ParseDisplayName(NULL, NULL, m_strInitDir, &chEaten, &pidl, &dwAttributes );
			if( FAILED( hr ) )
			{
				pMalloc->Free( pidl );
				pMalloc->Release();
				return false;
			}
			bInfo.pidlRoot = pidl;
		}
	}

	wchar_t strPath[_MAX_PATH];
	bInfo.hwndOwner = m_hParent;
	bInfo.pszDisplayName = strPath;
	bInfo.lpszTitle = m_strTitle;
	bInfo.ulFlags = m_InternalFlags | m_Flags | BIF_NEWDIALOGSTYLE;
	bInfo.lpfn = BrowseCtrlCallback;  // address of callback function
	bInfo.lParam = (LPARAM)this;      // pass address of object to callback function
	
	if( ( pidl = ::SHBrowseForFolder( &bInfo ) ) == NULL )
		return false;

	m_iImageIndex = bInfo.iImage;
	
	if( ::SHGetPathFromIDListW( pidl, strPath ) == FALSE )
	{
		pMalloc->Free( pidl );
		pMalloc->Release();
		return false;
	}
	
	pMalloc->Free( pidl );
	pMalloc->Release();

	// Store the selected path
	m_sSelDir = strPath;

	// Ensure a valid path string
	TCBase::EnsureValidPathSlashes( m_sSelDir );

	// Return the user hit success
	return true;
}
