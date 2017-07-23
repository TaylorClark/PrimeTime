//=================================================================================================
/*!
	\file GUIEditDlg.cpp
	2D Graphics Engine
	GUI Layout Source
	\author Taylor Clark
	\date February 6, 2006

	This source file contains the implementation for the GUI editor dialog class.
*/
//=================================================================================================

#include "../GUIEditDlg.h"
#include "../Resource.h"
#include <commctrl.h>
#include "GUI/GUIControl.h"
#include "Resource/ResourceMgr.h"
#include "Base/MsgLogger.h"
#include "GamePlay/GameGUILayout.h"


static const wchar_t* CLASS_NAME = L"TCGUI_EDITDLG";
static const wchar_t* PROG_NAME = L"GUI Data";

static INT_PTR DialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

const int MARGIN_LISTVIEW_WINDOW = 20;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUIEditDlg::Init()  Public
///
///	\param pParentHWnd The parent window for this dialog
///
///	Create and initialize the dialog.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIEditDlg::Init( HWND pParentHWnd )
{
	INITCOMMONCONTROLSEX vals;
	vals.dwSize = sizeof( vals );
	vals.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx( &vals );

	/*HRSRC res = FindResource( GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_FORMVIEW), RT_DIALOG );
	HGLOBAL hGlob = LoadResource( GetModuleHandle(NULL), res );
	void* pDlgData = LockResource( hGlob );
	m_HWnd = CreateDialogIndirect( GetModuleHandle(NULL), (LPCDLGTEMPLATE)pDlgData, pParentHWnd, (DLGPROC)DialogProc );
	FreeResource( hGlob );*/
	
	HMODULE hCurInst = GetModuleHandle( L"PrimeTime_dbg.dll" );
	m_HWnd = CreateDialog( hCurInst, MAKEINTRESOURCE( IDD_FORMVIEW ), pParentHWnd, (DLGPROC)DialogProc );
	if( !m_HWnd )
	{
		DWORD errorCode = GetLastError();

        const int BufferSize = 1024;
		wchar_t szError[BufferSize];
		FormatMessage( FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, LANG_SYSTEM_DEFAULT, szError, BufferSize, NULL );
		MessageBox( NULL, szError, L"Failed To Open GUI Edit Dialog", MB_OK );
	}
	// Else initialize the controls
	else
	{
		// Add the list box columns
		HWND hListWnd = GetDlgItem( m_HWnd, IDC_PROPERTYLIST );
		LVCOLUMN col;
		col.pszText = L"Name";
		col.cx = 100;
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		ListView_InsertColumn( hListWnd, 0, &col );

		col.pszText = L"Value";
		col.cx = 100;
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		ListView_InsertColumn( hListWnd, 1, &col );

		col.pszText = L"Type";
		col.cx = 100;
		col.mask = LVCF_TEXT | LVCF_WIDTH;
		ListView_InsertColumn( hListWnd, 2, &col );

		// Make the list control full row select
		ListView_SetExtendedListViewStyle( hListWnd, LVS_EX_FULLROWSELECT );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUIEditDlg::PumpMessages()  Public
///
///	Handle the windows messages for this dialog.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIEditDlg::PumpMessages()
{
	if( m_HWnd == 0 )
		return;

	// Get a windows message when available
	MSG msg;
	if( PeekMessage( &msg, m_HWnd, 0, 0, PM_NOREMOVE ) )
	{
		GetMessage( &msg, m_HWnd, 0, 0 );
		TranslateMessage( &msg );

		// Dispatch the message
		DispatchMessage(&msg);
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUIEditDlg::Close()  Public
///
///	Close and destroy the window and free data related to it.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIEditDlg::Close()
{
	if( m_HWnd == NULL )
		return;
	DestroyWindow( m_HWnd );

	m_HWnd = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUIEditDlg::Show()  Public
///
///	Make the window visible.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIEditDlg::Show()
{
	if( m_HWnd == NULL )
		return;
	ShowWindow( m_HWnd, SW_SHOW );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUIEditDlg::Hide()  Public
///
///	Hide the window.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIEditDlg::Hide()
{
	if( m_HWnd == NULL )
		return;
	ShowWindow( m_HWnd, SW_HIDE );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUIEditDlg::SetReferenceControl()  Public
///
///	\param pCtrl The control that is receiving focus
///
///	Set which GUI control has focus.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIEditDlg::SetReferenceControl( GUIControl* pCtrl )
{
	// Get the list handle
	HWND hListWnd = GetDlgItem( m_HWnd, IDC_PROPERTYLIST );
	if( !hListWnd )
		return;

	// Clear the items
	ListView_DeleteAllItems( hListWnd );

	// If there is no control being passed in then bail
	m_pSelCtrl = pCtrl;
	if( !pCtrl )
	{
		SetWindowText( m_HWnd, L"Nothing Selected" );
		return;
	}

	// Add the properties to the list box
	int itemIndex = 0;
	for( std::list<GUICtrlProperty*>::iterator iterProp = pCtrl->m_PropList.begin();
			iterProp != pCtrl->m_PropList.end();
			++iterProp )
	{
		// Add the property name
		LVITEM item;
		item.pszText = (LPWSTR)(*iterProp)->GetName();
		item.mask = LVIF_TEXT | LVIF_PARAM;;
		item.iItem = itemIndex;
		item.iSubItem = 0;
		item.lParam = (LPARAM)*iterProp;
		ListView_InsertItem( hListWnd, &item );

		// Add the property value in string form
		item.pszText = (LPWSTR)(*iterProp)->ValToString();
		item.mask = LVIF_TEXT;
		item.iItem = itemIndex;
		item.iSubItem = 1;
		ListView_SetItem( hListWnd, &item );

		// Add the type
		item.pszText = (LPWSTR)GUICtrlProperty::GetPropDescStr( (*iterProp)->GetType() );
		item.mask = LVIF_TEXT;
		item.iItem = itemIndex;
		item.iSubItem = 2;
		ListView_SetItem( hListWnd, &item );

		// Step the item index
		++itemIndex;
	}

	// Set the window title
	SetWindowText( m_HWnd, GUIControl::GetCtrlTypeName( m_pSelCtrl->GetCtrlType() ) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUIEditDlg::OnListItemSel()  Public
///
///	\param pCtrl The control that is receiving focus
///
///	Set which GUI control has focus.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIEditDlg::OnListItemSel()
{
	// Get the window controls
	HWND hListWnd = GetDlgItem( m_HWnd, IDC_PROPERTYLIST );
	HWND hEditBoxWnd = GetDlgItem( m_HWnd, IDC_VALUEEDIT );
	if( !hListWnd || !hEditBoxWnd )
		return;

	// Get the selected item
	m_SelPropIndex = ListView_GetSelectionMark( hListWnd );

	// Get the list item data
	LVITEM listItem;
	listItem.iItem = m_SelPropIndex;
	listItem.iSubItem = 0;
	listItem.mask = LVIF_PARAM;
	if( ListView_GetItem( hListWnd, &listItem ) == FALSE )
		return;

	// Get the property we have selected
	m_pActiveProp = (GUICtrlProperty*)listItem.lParam;
	if( !m_pActiveProp )
		return;

	// If the property is a string list
	if( m_pActiveProp->GetType() == GUIDefines::PT_StringList )
	{
		// Edit the list
		GUICtrlPropStringList* pStrListProp = (GUICtrlPropStringList*)m_pActiveProp;
		DoListEditDlgModal( m_HWnd, pStrListProp->Get() );
	}
	// Else if the property is a font
	else if( m_pActiveProp->GetType() == GUIDefines::PT_Font )
	{
		HMODULE hDll = LoadLibrary( L"ResPathDlg_dbg.dll" );

		// If the DLL was loaded
		if( hDll )
		{
			typedef bool (*RunResChooseFunc)( HWND hWnd, const ResourceMgr::KnownResVector&, ResourceID*, EResourceType );
			RunResChooseFunc pDlgFunc = (RunResChooseFunc)GetProcAddress( hDll, "RunResChooserDlgList_Filter" );
			ResourceID resID = 0;
			if( pDlgFunc )
			{
				// If the user chose a resource
				if( pDlgFunc( m_HWnd, ResourceMgr::Get().GetKnownRes(), &resID, RT_Font ) )
				{
					// Load the font and use it
					GUICtrlPropFont* pFontProp = (GUICtrlPropFont*)m_pActiveProp;
					TCFontHndl newFont = ResourceMgr::Get().GetTCFont( resID );
					pFontProp->Set( newFont );
				}
			}

			// Free the resource chooser DLL
			FreeLibrary( hDll );
		}
		else
			MsgLogger::Get().Output( MsgLogger::MI_Error, L"Failed to loaded resource chooser DLL." );
	}
	// Else if the property is a sprite
	else if( m_pActiveProp->GetType() == GUIDefines::PT_Sprite )
	{
		HMODULE hDll = LoadLibrary( L"ResPathDlg_dbg.dll" );
	
		DWORD failCode = GetLastError();

        const int BufferSize = 1024;
		wchar_t buffer[BufferSize] = {0};
		FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,NULL,failCode,0,buffer,BufferSize,NULL);

		// If the DLL was loaded
		if( hDll )
		{
			typedef bool (*RunResChooseFunc)( HWND hWnd, const ResourceMgr::KnownResVector&, ResourceID*, EResourceType );
			RunResChooseFunc pDlgFunc = (RunResChooseFunc)GetProcAddress( hDll, "RunResChooserDlgList_Filter" );
			ResourceID resID = 0;
			if( pDlgFunc )
			{
				// If the user chose a resource
				if( pDlgFunc( m_HWnd, ResourceMgr::Get().GetKnownRes(), &resID, RT_Sprite ) )
				{
					// Load the font and use it
					GUICtrlPropSprite* pSprProp = (GUICtrlPropSprite*)m_pActiveProp;
					RefSpriteHndl newSpr = ResourceMgr::Get().GetRefSprite( resID );
					pSprProp->Set( newSpr );
				}
			}

			// Free the resource chooser DLL
			FreeLibrary( hDll );
		}
		else
			MsgLogger::Get().Output( MsgLogger::MI_Error, L"Failed to loaded resource chooser DLL." );
	}

	// Add the property value in string form
	listItem.pszText = (LPWSTR)m_pActiveProp->ValToString();
	listItem.mask = LVIF_TEXT;
	listItem.iSubItem = 1;
	ListView_SetItem( hListWnd, &listItem );

	// Set the edit box text
	SetWindowText( hEditBoxWnd, m_pActiveProp->ValToString() );
	if( m_pActiveProp->GetType() == GUIDefines::PT_Int
		|| m_pActiveProp->GetType() == GUIDefines::PT_Boolean
		|| m_pActiveProp->GetType() == GUIDefines::PT_String )
		EnableWindow( hEditBoxWnd, TRUE );
	else
		EnableWindow( hEditBoxWnd, FALSE );	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GUIEditDlg::OnTextChange()  Global
///
///	A message handler called when the user changes a property value.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIEditDlg::OnTextChange( const wchar_t* szNewText )
{
	// If there is no property then bail
	if( !m_pActiveProp )
		return;
		
	// Set the value from the string
	m_pActiveProp->ValFromString( szNewText );

	// Get the list item window
	HWND hListWnd = GetDlgItem( m_HWnd, IDC_PROPERTYLIST );
	if( hListWnd )
	{
		// Get the list item data
		LVITEM listItem;
		listItem.iItem = m_SelPropIndex;
		listItem.iSubItem = 1;
		listItem.mask = LVIF_TEXT;
		listItem.pszText = (LPWSTR)m_pActiveProp->ValToString();
		
		// Update the text in the list	
		ListView_SetItem( hListWnd, &listItem );
	}

	/// If there is a control
	if( m_pSelCtrl )
	{
		m_pSelCtrl->ReformatControl();

		/// If the user modified the name property then see if we can hook up the controls
		if( wcscmp( m_pActiveProp->GetName(), L"Name" ) == 0 )
		{
			GameGUILayout* pLayout = reinterpret_cast<GameGUILayout*>( m_pSelCtrl->GetContainingLayout() );
			if( pLayout )
				pLayout->HookupCtrls();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	DialogProc()  Global
///
///	This function handles the dialog messages.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
INT_PTR DialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch(uMsg)
	{
	// If the window is being closed
	case WM_CLOSE:
		ShowWindow( hWnd, SW_HIDE );
		return TRUE;

	case WM_SIZE:
		{
			// If this is the window rather than a control
			if( IsWindow( hWnd ) )
			{
				// Get the control windows
				HWND hListWnd = GetDlgItem( hWnd, IDC_PROPERTYLIST );
				HWND hEditBoxWnd = GetDlgItem( hWnd, IDC_VALUEEDIT );

				if( hListWnd && hEditBoxWnd )
				{
					// Get the new size
					int width = (int)(lParam & 0x0000FFFF);
					int height = (int)((lParam >> 16) & 0x0000FFFF);

					// Get the edit box window
					RECT editBoxRect;
					GetWindowRect( hEditBoxWnd, &editBoxRect );
					MoveWindow( hEditBoxWnd, MARGIN_LISTVIEW_WINDOW, MARGIN_LISTVIEW_WINDOW, width - (MARGIN_LISTVIEW_WINDOW * 2), editBoxRect.bottom - editBoxRect.top, TRUE );
					
					int topOfList = (editBoxRect.bottom - editBoxRect.top) + (MARGIN_LISTVIEW_WINDOW * 2);
					
					// Make the list view grow with the window
					MoveWindow( hListWnd, MARGIN_LISTVIEW_WINDOW, topOfList, width - (MARGIN_LISTVIEW_WINDOW * 2), height - (topOfList + (MARGIN_LISTVIEW_WINDOW * 2)), TRUE );
				}
			}
		}
		return TRUE;

	case WM_COMMAND:
		{
			// Get the command ID
			int cmdID = (int)wParam & 0x0000FFFF;

			// If the command ID is that of the value edit control
			if( cmdID == IDC_VALUEEDIT )
			{
				// If this is a text update message
				int notifyCode = ((int)wParam >> 16) & 0x0000FFFF;
				if( notifyCode == EN_UPDATE )
				{
					// Get the edit control text
					const int BUFFER_LEN = 512;
					wchar_t szText[BUFFER_LEN] = {0};
					HWND hEditBoxWnd = GetDlgItem( hWnd, IDC_VALUEEDIT );
					GetWindowText( hEditBoxWnd, szText, BUFFER_LEN );

					// Update the value if possible
					GUIEditDlg::Get().OnTextChange( szText );
				}
			}
		}

		return FALSE;

	// Handle notifications
	case WM_NOTIFY:
		{
			// If the message is for the list box
			int ctrlID = (int)wParam;
			if( ctrlID == IDC_PROPERTYLIST )
			{
				// If an item's state has changed
				NMHDR* pNMHeader = (NMHDR*)lParam;
				if( pNMHeader->code == LVN_ITEMCHANGED )
				{
					NMLISTVIEW* pItemStateChange = (NMLISTVIEW*)pNMHeader;

					// If we are activating an item
					if( pItemStateChange->uChanged & LVIF_STATE
						&& pItemStateChange->uNewState & LVIS_SELECTED )
					{
						// Set the selection mark on this item
						HWND hListWnd = GetDlgItem( hWnd, IDC_PROPERTYLIST );
						if( hListWnd )
							ListView_SetSelectionMark( hListWnd, pItemStateChange->iItem );

						// Handle selection
						GUIEditDlg::Get().OnListItemSel();
					}

					return TRUE;
				}
			}
		}
		return FALSE;

	// Ensure the window isn't shrunk too much
	case WM_GETMINMAXINFO:
		MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
		pInfo->ptMinTrackSize.x = 119;
		pInfo->ptMinTrackSize.y = 145;
		return TRUE;
	}

	// We did not handle the message
	return FALSE;
}