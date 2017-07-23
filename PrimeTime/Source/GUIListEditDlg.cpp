#include "../GUIEditDlg.h"
#include "../Resource.h"
#include <commctrl.h>

static INT_PTR DialogProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	DoListEditDlgModal()  Global
///
///	\param pParentHWnd The parent window for this dialog
///	\param pInitialItems The list of strings to modify
///	\returns True if the user hit OK, false otherwise
///
///	Display a dialog to modify a list of strings.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool DoListEditDlgModal( HWND pParentHWnd, std::vector<GUIListItem>* pStringList )
{
	if( !pStringList )
		return false;

	// Initialize the common controls
	INITCOMMONCONTROLSEX vals;
	vals.dwSize = sizeof( vals );
	vals.dwICC = ICC_LISTVIEW_CLASSES;
	InitCommonControlsEx( &vals );
	
	// Make a copy of the list
	std::vector<GUIListItem> tempStrList;
	for( std::vector<GUIListItem>::iterator iterStr = pStringList->begin(); iterStr != pStringList->end(); ++iterStr )
		tempStrList.push_back( *iterStr );

	// Display the dialog
	INT_PTR retVal = DialogBoxParam( GetModuleHandle( L"PrimeTime_dbg.dll" ),
				MAKEINTRESOURCE( IDD_ORDEREDLISTDIALOG ),
				pParentHWnd,
				(DLGPROC)DialogProc,
				(LPARAM)&tempStrList );
	
	// If the user hit cancel then bail out before storing the strings
	if( (int)retVal == 0 )
		return false;

	// Store the strings
	pStringList->clear();
	for( std::vector<GUIListItem>::iterator iterStr = tempStrList.begin(); iterStr != tempStrList.end(); ++iterStr )
		pStringList->push_back( *iterStr );

	// Return the user hit OK
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	MoveSelectedListItem()  Global
///
///	\param hStrListCtl The list control whose selected item we are moving
///	\param moveUp True if we are moving the item up, false to move down
///
///	Move the selected list item up or down.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MoveSelectedListItem( HWND hStrListCtl, bool moveUp )
{
	// Get the selected index
	int itemIndex = ListView_GetSelectionMark( hStrListCtl );
	if( itemIndex == -1 )
		return;

	// Get the number of items
	int numItems = ListView_GetItemCount( hStrListCtl );

	// Easy out if we can't move the item
	if( moveUp && itemIndex == 0 )
		return;
	else if( !moveUp && itemIndex == (numItems - 1) )
		return;

	// Get the selected text
	const int BUFFER_SIZE = 256;
	wchar_t szItemText[ BUFFER_SIZE ];
	ListView_GetItemText( hStrListCtl, itemIndex, 0, szItemText, BUFFER_SIZE );

	// Remove the item
	ListView_DeleteItem( hStrListCtl, itemIndex );

	// Get the new index
	if( moveUp )
		--itemIndex;
	else
		++itemIndex;

	// Re-add the item
	LVITEM item;
	item.mask = LVIF_TEXT;
	item.pszText = szItemText;
	item.iItem = itemIndex;
	item.iSubItem = 0;
	ListView_InsertItem( hStrListCtl, &item );

	// Select it
	ListView_SetItemState( hStrListCtl, itemIndex, LVIS_FOCUSED | LVIS_SELECTED, LVIS_FOCUSED | LVIS_SELECTED );
	ListView_SetSelectionMark( hStrListCtl, itemIndex );
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

	case WM_INITDIALOG:
		{
			// Store the string list which is passed via the lParam
			SetWindowLong( hWnd, GWL_USERDATA, (LONG)lParam );

			// Add the list box columns
			HWND hListWnd = GetDlgItem( hWnd, IDC_STRINGLIST );
			LVCOLUMN col;
			col.pszText = L"Name";
			col.cx = 100;
			col.mask = LVCF_TEXT | LVCF_WIDTH;
			ListView_InsertColumn( hListWnd, 0, &col );

			SetFocus( GetDlgItem( hWnd, IDC_VALUEEDIT ) );

			HWND hStrListCtl = GetDlgItem( hWnd, IDC_STRINGLIST );

			// If there is an initial string list then use it
			if( lParam != NULL && hStrListCtl )
			{
				std::vector<GUIListItem>* pStrList = (std::vector<GUIListItem>*)lParam;
				int itemIndex = 0;
				for( std::vector<GUIListItem>::iterator iterStr = pStrList->begin(); iterStr != pStrList->end(); ++iterStr )
				{
					// Get the string into a temporary buffer
					const int BUFFER_LEN = 512;
					wchar_t szText[BUFFER_LEN] = {0};
					wcscpy_s( szText, BUFFER_LEN, iterStr->sText.c_str() );

					// Add the item
					LVITEM item;
					item.mask = LVIF_TEXT;
					item.pszText = szText;
					item.iItem = itemIndex++;
					item.iSubItem = 0;
					ListView_InsertItem( hStrListCtl, &item );
				}
			}

		}
		return FALSE;

	case WM_COMMAND:
		{
			// Get the command ID
			int cmdID = (int)wParam & 0x0000FFFF;

			switch( cmdID )
			{

				// If the command ID is that of the value edit control
				case IDC_VALUEEDIT:
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
					}
				}
				break;

				// If we are adding a string
				case IDC_ADDBUTTON:
				{
					// Get the edit box and list window
					HWND hEditCtl = GetDlgItem( hWnd, IDC_VALUEEDIT );
					HWND hStrListCtl = GetDlgItem( hWnd, IDC_STRINGLIST );

					if( !hEditCtl || !hStrListCtl )
						break;
					
					// Get the text from the edit box
					const int BUFFER_SIZE = 256;
					wchar_t charBuffer[ BUFFER_SIZE ];
					GetWindowText( hEditCtl, charBuffer, BUFFER_SIZE );

					// If there is actually a string
					if( wcslen( charBuffer ) > 0 )
					{
						// Get the number of strings in the list
						int numItems = ListView_GetItemCount( hStrListCtl );

						// Add a test item
						LVITEM item;
						item.mask = LVIF_TEXT;
						item.pszText = charBuffer;
						item.iItem = numItems;
						item.iSubItem = 0;
						ListView_InsertItem( hStrListCtl, &item );

						// Clear the edit box
						SetWindowText( hEditCtl, L"" );

						// Focus back on the edit box
						SetFocus( GetDlgItem( hWnd, IDC_VALUEEDIT ) );
					}
				}
				break;

				case IDC_REMOVEBUTTON:
				{
					// Get the string list control handle
					HWND hStrListCtl = GetDlgItem( hWnd, IDC_STRINGLIST );
					if( !hStrListCtl )
						break;

					// Get the selected item index
					int itemIndex = ListView_GetSelectionMark( hStrListCtl );
					if( itemIndex == -1 )
						break;

					// Remove the item
					ListView_DeleteItem( hStrListCtl, itemIndex );
				}
				break;

				// If we are moving a string up
				case IDC_MOVEUPBUTTON:
				{
					// Get the string list control handle
					HWND hStrListCtl = GetDlgItem( hWnd, IDC_STRINGLIST );
					if( !hStrListCtl )
						break;

					// Move the item
					MoveSelectedListItem( hStrListCtl, true );
				}
				break;

				// If we are moving a string down
				case IDC_MOVEDOWNBUTTON:
				{
					// Get the string list control handle
					HWND hStrListCtl = GetDlgItem( hWnd, IDC_STRINGLIST );
					if( !hStrListCtl )
						break;

					// Move the item
					MoveSelectedListItem( hStrListCtl, false );
				}
				break;

				// If we are accepting the list
				case IDOK:
				{
					// Get the list box control
					HWND hStrListCtl = GetDlgItem( hWnd, IDC_STRINGLIST );

					// Get the string list
					std::vector<GUIListItem>* pStrList = (std::vector<GUIListItem>*)( GetWindowLong( hWnd, GWL_USERDATA ) );
					if( pStrList && hStrListCtl )
					{
						pStrList->clear();

						// Store each string from the list box
						int numItems = ListView_GetItemCount( hStrListCtl );
						const int BUFFER_SIZE = 256;
						wchar_t charBuffer[ BUFFER_SIZE ];
						for( int itemIndex = 0; itemIndex < numItems; ++itemIndex )
						{
							// Get the string
							ListView_GetItemText( hStrListCtl, itemIndex, 0, charBuffer, BUFFER_SIZE );

							// Store the string
							pStrList->push_back( GUIListItem((const wchar_t*)charBuffer) );
						}
					}

					EndDialog( hWnd, (INT_PTR)1 );
				}
				break;

				// Don't do anything
				case IDCANCEL:
					EndDialog( hWnd, (INT_PTR)0 );
					break;

				default:
					return FALSE;
			}
		}
		return TRUE;

	case WM_GETMINMAXINFO:
		MINMAXINFO* pInfo = (MINMAXINFO*)lParam;
		pInfo->ptMinTrackSize.x = 119;
		pInfo->ptMinTrackSize.y = 145;
		return TRUE;

	/*case WM_DESTROY:
		if( m_HWnd != 0 )
			DestroyWindow( m_HWnd );
		return TRUE;*/
	}

	// We did not handle the message
	return FALSE;
}