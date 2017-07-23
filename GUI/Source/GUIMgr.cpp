//=================================================================================================
/*!
	\file GUIMgr.cpp
	2D Graphics Engine
	GUI Manager Source
	\author Taylor Clark

	This source file contains the implementation for the GUI manager class.
*/
//=================================================================================================

#include "../GUIMgr.h"
#include "../GUIMessages.h"
#include "../GUIControl.h"
#include "../GUILayout.h"
#include "Base/MsgLogger.h"
#include <fstream>
#include "Base/NetSafeSerializer.h"
#include "Base/TCAssert.h"
#include "Base/FileFuncs.h"
#include "../MsgBox.h"
#include "Base/CriticalSection.h"
#include "PrimeTime/ApplicationBase.h"

#ifdef WINDOWS
#include <windows.h>
#elif SFML
#include <SFML/Window/Mouse.hpp>
#include <SFML/Window/Keyboard.hpp>
#endif

#ifdef ENABLE_GUI_EDITOR
#include "PrimeTime/ApplicationWin.h"
#include "PrimeTime/GUIEditDlg.h"
#include "PrimeTime/GUIListEditDlg.h"
#include "GamePlay/GameDefines.h"
#include "GamePlay/GameGUILayout.h"
#endif

static TCBase::CriticalSection g_msgListCriticalSection;

const FourCC GUIMgr::FOURCCKEY_GUI( "UIDB" );


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::Draw()  Public
///
///	Draw the current layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::Draw() const
{
	// If there is a layout then draw it
	if( m_pCurLayout )
		m_pCurLayout->Draw();

	// If there is a message box then draw it
	if( m_pCurMsgBox )
		m_pCurMsgBox->Draw();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::ClearLayouts()  Public
///
///	Clear the layout data in the GUI manager.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::ClearLayouts()
{
	// Clear the layouts
	m_pCurLayout = NULL;
	for( LayoutMap::iterator iterLayout = m_LayoutMap.begin(); iterLayout != m_LayoutMap.end(); ++iterLayout )
		delete iterLayout->second;
	m_LayoutMap.clear();

	// Clear the compressed layouts
	for( CompressedLayoutMap::iterator iterLayout = m_CompLayoutMap.begin(); iterLayout != m_CompLayoutMap.end(); ++iterLayout )
	{
		delete [] iterLayout->second.pLayoutData;

		// For some reason gcc can't deduce the type of iterLayout->second.pLayout
		GUILayout* pLayout = iterLayout->second.pLayout;
		delete pLayout;
	}
	m_CompLayoutMap.clear();

	// Clear any messages that were due to be handled
	for( std::list< GUIMessage* >::iterator iterMsg = m_GUIMessageList.begin(); iterMsg != m_GUIMessageList.end(); ++iterMsg )
		delete *iterMsg;
	m_GUIMessageList.clear();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::Update()  Public
///
///	\param frameTime The elapsed frame time in seconds
///
///	Update the user interface.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::Update( float32 frameTime )
{
	// Set the control frame time for controls to reference if needed
	GUIControl::SetFrameTime( frameTime );

	// If we have a pending layout
	if( m_pPendingActiveLayout )
	{
		// Deactivate the current layout
		if( m_pCurLayout )
		{
			m_pCurLayout->OnDeactivate();

			// Undo the changes, if any, that may have occured for optimization to the background
			// image
			m_pCurLayout->UndoBackgroundMods();
		}

		// Store the layout
		m_pCurLayout = m_pPendingActiveLayout;
		if( m_pCurLayout )
		{
			m_pCurLayout->OnActivate();
		}

		m_pPendingActiveLayout = NULL;
	}

	// If we are inactive then bail
	if( !m_IsActive )
		return;

	// Update the mouse enter and leave messages
	if( m_pCurMsgBox )
	{
		m_pCurMsgBox->UpdateMouseEnterLeaveMsgs();
		m_pCurMsgBox->Update( frameTime );

		if( m_pCurLayout )
			m_pCurLayout->Update( frameTime );
	}
	else if( m_pCurLayout )
	{
		m_pCurLayout->UpdateMouseEnterLeaveMsgs();
		m_pCurLayout->Update( frameTime );
	}

	// Enter the message list critical section
	g_msgListCriticalSection.Enter();

	// Go through the GUI messages
	for( GUIMessageList::iterator iterMsg = m_GUIMessageList.begin(); iterMsg != m_GUIMessageList.end(); ++iterMsg )
	{
#ifdef ENABLE_GUI_EDITOR
		if( HandleGUIMessage( *iterMsg ) )
		{
			delete *iterMsg;
			continue;
		}
#endif

		// If there is a message box then let that handle messages
		bool msgHandled = false;
		if( m_pCurMsgBox )
			msgHandled = m_pCurMsgBox->HandleMessage( *iterMsg );
		// Else send the message to handlers
		else if( m_pCurLayout )
			msgHandled = m_pCurLayout->HandleMessage( *iterMsg );

		// Free the memory associated with this message
		delete *iterMsg;
	}

	// Clear the messages
	m_GUIMessageList.clear();

	// Exit the critical section
	g_msgListCriticalSection.Leave();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::Init()  Public
///
///	\returns True if the GUI manager initialized successfully, false otherwise
///
///	Initialize the GUI manager for use.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUIMgr::Init()
{
	// Return success
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::Term()  Public
///
///	Free any resources used by the GUI manager.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::Term()
{
	m_IsActive = false;

	// If there is an activate layout then deactivate it
	if( m_pCurLayout )
	{
		m_pCurLayout->OnDeactivate();
		m_pCurLayout = NULL;
	}

	// Free the layouts
	ClearLayouts();
	m_pPendingActiveLayout = NULL;
}

#ifdef ENABLE_GUI_EDITOR
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::PopupToolsContextMenu()  Private
///
///	\param mousePos The mouse position in client coordinates
///
///	Display the GUI popup menu.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::PopupToolsContextMenu( const Point2i& mousePos )
{
	// Create the menu
	m_ToolsPopupMenu = CreatePopupMenu();

	// Setup the item default values
	MENUITEMINFO itemInfo;
	memset( &itemInfo, 0, sizeof(itemInfo) );
	itemInfo.cbSize = sizeof( itemInfo );
	itemInfo.fMask = MIIM_STRING | MIIM_ID;
	itemInfo.fType = MFT_STRING;
	
	// The bit masks for items
	const int ITEM_MASK_CREATE = 0x00001000;
	const int ITEM_MASK_LAYOUT = 0x00002000;
	const int ITEM_MASK_EDITCTRL = 0x00004000;

	enum ELayoutItems
	{
		LI_Save = 0,
		LI_Load
	};

	enum EEditCtrlItems
	{
		ECI_SendToBack = 0,
		ECI_BringToFront,
		ECI_MoveBack,
		ECI_MoveForward,
		ECI_Split1,
		ECI_DeleteControl,
		ECI_Split2,
		ECI_CenterHorz,
		ECI_CenterVert
	};

	// Create the create item menu
	HMENU hCreateMenu = CreateMenu();

	// Add all of the create menu items
	int menuIndex = 0;
	for( int curCtrlType = GUIDefines::CT_Error + 1; curCtrlType < GUIDefines::CT_COUNT; ++curCtrlType )
	{
		itemInfo.wID = ITEM_MASK_CREATE | curCtrlType;
		itemInfo.dwTypeData = (LPWSTR)GUIControl::GetCtrlTypeName( (GUIDefines::EControlType)curCtrlType );
		InsertMenuItem( hCreateMenu, menuIndex++, TRUE, &itemInfo );
	}


	// If there is a selected control
	GUIControl* pSelCtrl = GUIEditDlg::Get().GetReferenceControl();
	HMENU hCtrlEditMenu = NULL;
	if( pSelCtrl )
	{
		// Create the control edit menu
		hCtrlEditMenu = CreateMenu();

		itemInfo.fMask = MIIM_STRING | MIIM_ID;
		itemInfo.fType = MFT_STRING;
		itemInfo.wID = ITEM_MASK_EDITCTRL | ECI_SendToBack;
		itemInfo.dwTypeData = L"Send to back";
		InsertMenuItem( hCtrlEditMenu, ECI_SendToBack, TRUE, &itemInfo );
		
		itemInfo.wID = ITEM_MASK_EDITCTRL | ECI_BringToFront;
		itemInfo.dwTypeData = L"Bring to front";
		InsertMenuItem( hCtrlEditMenu, ECI_BringToFront, TRUE, &itemInfo );

		itemInfo.wID = ITEM_MASK_EDITCTRL | ECI_MoveBack;
		itemInfo.dwTypeData = L"Move back";
		InsertMenuItem( hCtrlEditMenu, ECI_MoveBack, TRUE, &itemInfo );

		itemInfo.wID = ITEM_MASK_EDITCTRL | ECI_MoveForward;
		itemInfo.dwTypeData = L"Move forward";
		InsertMenuItem( hCtrlEditMenu, ECI_MoveForward, TRUE, &itemInfo );
		

		itemInfo.wID = ITEM_MASK_EDITCTRL | ECI_Split1;
		itemInfo.fMask = MIIM_FTYPE;
		itemInfo.fType = MFT_SEPARATOR;
		InsertMenuItem( hCtrlEditMenu, ECI_Split1, TRUE, &itemInfo );


		itemInfo.fMask = MIIM_STRING | MIIM_ID;
		itemInfo.fType = MFT_STRING;
		itemInfo.wID = ITEM_MASK_EDITCTRL | ECI_DeleteControl;
		itemInfo.dwTypeData = L"Delete";
		InsertMenuItem( hCtrlEditMenu, ECI_DeleteControl, TRUE, &itemInfo );


		itemInfo.wID = ITEM_MASK_EDITCTRL | ECI_Split2;
		itemInfo.fMask = MIIM_FTYPE;
		itemInfo.fType = MFT_SEPARATOR;
		InsertMenuItem( hCtrlEditMenu, ECI_Split2, TRUE, &itemInfo );


		itemInfo.fMask = MIIM_STRING | MIIM_ID;
		itemInfo.fType = MFT_STRING;
		itemInfo.wID = ITEM_MASK_EDITCTRL | ECI_CenterHorz;
		itemInfo.dwTypeData = L"Center Horizontally";
		InsertMenuItem( hCtrlEditMenu, ECI_CenterHorz, TRUE, &itemInfo );

		itemInfo.wID = ITEM_MASK_EDITCTRL | ECI_CenterVert;
		itemInfo.dwTypeData = L"Center Vertically";
		InsertMenuItem( hCtrlEditMenu, ECI_CenterVert, TRUE, &itemInfo );
	}



	// Create the layout menu
	HMENU hLayoutMenu = CreateMenu();

	// Add the options
	const int LAYOUT_ID_OFFSET = 2;
	UINT menuItemID = LAYOUT_ID_OFFSET;
	for( uint32 layoutIndex = 0; layoutIndex < GameDefines::MS_COUNT; ++layoutIndex )
	{
		if( layoutIndex == m_pCurLayout->GetLayoutID() )
		{
			itemInfo.fMask = MIIM_STRING | MIIM_STATE | MIIM_ID;
			itemInfo.fState = MFS_CHECKED;
		}
		else
		{
			itemInfo.fMask = MIIM_STRING | MIIM_ID;
			itemInfo.fState = 0;
		}
		itemInfo.wID = ITEM_MASK_LAYOUT | menuItemID++;
		itemInfo.dwTypeData = (LPWSTR)GameDefines::GetMenuScreenDesc( (GameDefines::EMenuScreens)layoutIndex );
		InsertMenuItem( hLayoutMenu, 0, TRUE, &itemInfo );
	}
	itemInfo.wID = ITEM_MASK_LAYOUT | menuItemID++;
	itemInfo.fMask = MIIM_FTYPE;
	itemInfo.fType = MFT_SEPARATOR;
	itemInfo.fState = 0;
	InsertMenuItem( hLayoutMenu, 0, TRUE, &itemInfo );
	
	itemInfo.wID = ITEM_MASK_LAYOUT | 0;
	itemInfo.fMask = MIIM_STRING | MIIM_ID;
	itemInfo.fType = MFT_STRING;
	itemInfo.dwTypeData = L"Save...";
	InsertMenuItem( hLayoutMenu, LI_Save, TRUE, &itemInfo );
	
	itemInfo.wID = ITEM_MASK_LAYOUT | 1;
	itemInfo.dwTypeData = L"Load...";
	InsertMenuItem( hLayoutMenu, LI_Load, TRUE, &itemInfo );


	// Insert the create menu
	itemInfo.hSubMenu = hCreateMenu;
	itemInfo.fMask = MIIM_STRING | MIIM_SUBMENU;
	itemInfo.dwTypeData = L"Create";
	InsertMenuItem( (HMENU)m_ToolsPopupMenu, 0, TRUE, &itemInfo );

	// Insert the layout menu
	itemInfo.hSubMenu = hLayoutMenu;
	itemInfo.fMask = MIIM_STRING | MIIM_SUBMENU;
	itemInfo.dwTypeData = L"Layouts";
	InsertMenuItem( (HMENU)m_ToolsPopupMenu, 1, TRUE, &itemInfo );

	// Insert the control edit menu if there is one
	if( hCtrlEditMenu )
	{
		itemInfo.hSubMenu = hCtrlEditMenu;
		itemInfo.fMask = MIIM_STRING | MIIM_SUBMENU;
		itemInfo.dwTypeData = L"Selected Control";
		InsertMenuItem( (HMENU)m_ToolsPopupMenu, 2, TRUE, &itemInfo );
	}

	ApplicationWin* pAppWin = static_cast<ApplicationWin*>( ApplicationBase::Get() );
	
	// Get the mouse position in screen coordinates
	POINT screenPos = { mousePos.x, mousePos.y };
	ClientToScreen( pAppWin->GetHWnd(), &screenPos );

	// Pop the menu up
	int itemID = TrackPopupMenuEx( (HMENU)m_ToolsPopupMenu,
						TPM_LEFTALIGN | TPM_RETURNCMD,
						screenPos.x,
						screenPos.y,
						pAppWin->GetHWnd(),
						NULL );


	// If the user selected to create an item and we have a layout
	if( itemID & ITEM_MASK_CREATE && m_pCurLayout )
	{
		// Get the control ID to create by removing the mask
		int selID = itemID & ~ITEM_MASK_CREATE;

		// Create the control at the position
		GUIControl* pNewCtrl = GUIControl::CreateControl( (GUIDefines::EControlType)selID, m_pCurLayout );
		if( pNewCtrl )
		{
			pNewCtrl->SetPos( mousePos );
			m_pCurLayout->AddControl( pNewCtrl );
		}	
		else
			MsgLogger::Get().Output( MsgLogger::MI_Error, L"Failed to create control." );
	}
	// Else if the user selected to save the layout
	else if( itemID & ITEM_MASK_LAYOUT )
	{
		// Get if we are saving
		bool isSaving = (itemID & ~ITEM_MASK_LAYOUT) == LI_Save;
		bool isLoading = (itemID & ~ITEM_MASK_LAYOUT) == LI_Load;

		// Create the stream
		std::wstring sFileName = ApplicationBase::GetResourcePath() + L"gui.rdb";
		if( isSaving )
		{
			// Don't allow saving on the InGame layout
			if( m_pCurLayout->GetLayoutID() == GameDefines::MS_InGame1Player )
				MessageBox( NULL, L"You can't save on the In Game layout, please select another, such as the Title Screen, then save. This is because the In Game layout has the temporary GameManager control", L"Please Switch Layouts", MB_OK );	
			else
			{
				// Confirm saving
				if( MessageBox( NULL, L"Are you sure you want to save?", L"Save Confirmation", MB_YESNO ) == IDYES )
				{
					TCBase::MakeFileCopy( sFileName.c_str(), (ApplicationBase::GetResourcePath() + L"gui.AutoBackrdb").c_str() );

					// Open the file for writing
					std::ofstream outFile( sFileName.c_str(), std::ios_base::binary | std::ios_base::out );
					if( outFile.good() )
					{
						NetSafeSerializer s( &outFile );

						// Transfer the data
						bool saved = GameGUILayout::TransferGUIResourceData( s );

						// Close the file
						outFile.close();

						if( saved )
							MsgLogger::Get().Output( MsgLogger::MI_Note, L"GUI.rdb file saved." );
						else
							MsgLogger::Get().Output( MsgLogger::MI_Note, L"GUI.rdb file failed to save." );
					}
					else
						MsgLogger::Get().Output( MsgLogger::MI_Error, L"Failed to open gui.rdb file for saving." );
				}
			}
		}
		// Else load the file
		else if( isLoading )
		{
			// Confirm loading
			if( MessageBox( NULL, L"Are you sure you want to load?", L"Load confirmation", MB_YESNO ) == IDYES )
			{
				GameGUILayout::LoadLayouts( sFileName );

				SetActiveLayout( GameDefines::MS_TitleScreen );
			}
		}
		// Else we are switching to a new layout
		else
		{
			// Get the ID
			int32 selLayoutID = (itemID & ~ITEM_MASK_LAYOUT);
			selLayoutID -= LAYOUT_ID_OFFSET;

			SetActiveLayout( selLayoutID );
		}
	}
	// Else if the user decided to edit a control
	else if( itemID & ITEM_MASK_EDITCTRL )
	{
		// Get the menu ID selected
		int selID = itemID & ~ITEM_MASK_EDITCTRL;

		// If we are sending to back
		if( selID == ECI_SendToBack )
			m_pCurLayout->SendCtrlToBack( GUIEditDlg::Get().GetReferenceControl() );
		// Else if the control is coming up front
		else if( selID == ECI_BringToFront )
			m_pCurLayout->BringCtrlToFront( GUIEditDlg::Get().GetReferenceControl() );
		// Else if the control is coming up front
		else if( selID == ECI_MoveBack )
			m_pCurLayout->MoveCtrlBack( GUIEditDlg::Get().GetReferenceControl() );
		// Else if the control is coming up front
		else if( selID == ECI_MoveForward )
			m_pCurLayout->MoveCtrlForward( GUIEditDlg::Get().GetReferenceControl() );
		// Else if the control is to be removed
		else if( selID == ECI_DeleteControl )
		{
			m_pCurLayout->DeleteControl( GUIEditDlg::Get().GetReferenceControl() );
			GUIEditDlg::Get().SetReferenceControl( NULL );
		}
		// Else if the control is coming up front
		else if( selID == ECI_CenterHorz )
			m_pCurLayout->CenterCtrlHorz( GUIEditDlg::Get().GetReferenceControl() );
		// Else if the control is coming up front
		else if( selID == ECI_CenterVert )
			m_pCurLayout->CenterCtrlVert( GUIEditDlg::Get().GetReferenceControl() );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::HandleGUIMessage()  Public
///
///	\param pMsg The message to handle
///	\returns True if the message was handled, false otherwise
///
///	Allow the GUI editor to handle a GUI message.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUIMgr::HandleGUIMessage( GUIMessage* pMsg )
{
	// If the GUI edit dialog is not visible then we are not handling messages
	if( !GUIEditDlg::Get().IsVisible() )
		return false;

	// Is this a mouse down message
	if( pMsg->GetType() == GUIDefines::GMT_MouseDown )
	{
		GUIMessageMouseDown* pMouseDownMsg = reinterpret_cast<GUIMessageMouseDown*>( pMsg );
	
		// If we have a layout to work with
		if( m_pCurLayout )
		{
			// If the left mouse button was pressed
			if( pMouseDownMsg->GetButton() == GUIDefines::MB_Left )
			{
				// Get the item we are over
				GUIControl* pCtrl = m_pCurLayout->GetControlAtPoint( pMouseDownMsg->GetPos() );

				// Select the item
				GUIEditDlg::Get().SetReferenceControl( pCtrl );
			}
			// Else if the right mouse button was pressed then pop-up and handle the context menu
			else if( pMouseDownMsg->GetButton() == GUIDefines::MB_Right )
				PopupToolsContextMenu( pMouseDownMsg->GetPos() );
		}

		// We handled the message
		return true;
	}

	// It was not handled
	return false;
}

void GUIMgr::SaveLayouts( Serializer& writer )
{
	if( writer.InReadMode() )
	{
		TCBREAKX(L"Can't save with reader");
		return;
	}

	for( LayoutMap::iterator iterLayout = m_LayoutMap.begin(); iterLayout != m_LayoutMap.end(); ++iterLayout )
	{
		GUILayout* pLayout = iterLayout->second;
		if( pLayout )
		{
			// Store the ID
			uint32 layoutID = pLayout->GetLayoutID();
			writer.AddData( layoutID );

			// Get the current byte offset and write a uint32 placeholder for the value
			uint32 layoutSizeOffset = writer.GetOffset();
			writer.AddData( layoutSizeOffset );

			// Store the layout
			pLayout->TransferLayout( writer );

			// Get the size of the layout data
			uint32 curStreamPos = writer.GetOffset();
			uint32 layoutSizeData = curStreamPos - (layoutSizeOffset + 4);

			// Store the size and seek back to the end of the stream
			writer.Seek( layoutSizeOffset );
			writer.AddData( layoutSizeData );
			writer.Seek( curStreamPos );
		}
	}

	// Go through the compressed layouts
	for( CompressedLayoutMap::iterator iterLayout = m_CompLayoutMap.begin(); iterLayout != m_CompLayoutMap.end(); ++iterLayout )
	{
		// Write the layout ID
		uint32 layoutID = iterLayout->first;
		writer.AddData( layoutID );
		
		// Write the data size
		writer.AddData( iterLayout->second.dataSize );

		// Write the data
		writer.AddRawData( (uint8*)iterLayout->second.pLayoutData, iterLayout->second.dataSize );
	}	
}

#endif



///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::AddNewGUIMsg()  Public
///
///	\param pMsg The GUI message to handle
///
///	Add a message to the list of messages to be handles
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::AddNewGUIMsg( GUIMessage* pMsg )
{
	// Ensure a valid message and an active GUI
	bool allowMsg = pMsg && m_IsActive && (m_pCurLayout || m_pCurMsgBox);
	if( !allowMsg )
	{
		delete pMsg;
		return;
	}

	// Add the message to the list in a thread-safe manner
	g_msgListCriticalSection.Enter();
	m_GUIMessageList.push_back( pMsg );
	g_msgListCriticalSection.Leave();
}

/// Turn off the GUI
void GUIMgr::Deactivate()
{
	// The GUI manager is no longer active
	m_IsActive = false;
	
	// Clear the GUI messages
	g_msgListCriticalSection.Enter();
	for( GUIMessageList::iterator iterMsg = m_GUIMessageList.begin(); iterMsg != m_GUIMessageList.end(); ++iterMsg )
		delete *iterMsg;
	m_GUIMessageList.clear();
	g_msgListCriticalSection.Leave();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::PopupMessageBox()  Public
///
///	\param pMsgBox The message box to display
///	\returns True if the box was displayed, false if there is already a message box displayed
///
///	Display a message box.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUIMgr::PopupMessageBox( MsgBox* pMsgBox )
{
	if( !pMsgBox || m_pCurMsgBox )
		return false;

	// Store the message box and place it at the front of the message handlers
	m_pCurMsgBox = pMsgBox;
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::CloseMessageBox()  Public
///
///	\param pMsgBox The message box to close
///
///	Close the specified message box.  If the passed-in message box is not the currently displayed
///	message box then nothing is changed.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::CloseMessageBox( MsgBox* pMsgBox )
{
	// Ensure we are closing the proper message box
	if( pMsgBox != m_pCurMsgBox )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Note, L"Attempting to close message box that is not open." );
		return;
	}

	// Stop referencing the message box
	m_pCurMsgBox = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::IsKeyDown()  Public
///
///	\param windowsKeyCode The windows virtual key code
///
///	Test if a key is down.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
//bool GUIMgr::IsKeyDown( unsigned int windowsKeyCode )
//{
//	//TODO
//#ifdef WINDOWS
//	return (GetKeyState( windowsKeyCode ) & 0x8000) == 0x8000;
//#else
//	ApplicationSFML* pApp = static_cast<ApplicationSFML*>( ApplicationBase::Get() );
//
//	return pApp->GetRenderWindow()->GetInput().IsKeyDown(
//#endif
//}

bool GUIMgr::IsShiftDown()
{
	//TODO
#ifdef WINDOWS
	return (GetKeyState( VK_SHIFT ) & 0x8000) == 0x8000;
#else
	return sf::Keyboard::isKeyPressed( sf::Keyboard::LShift )
			|| sf::Keyboard::isKeyPressed( sf::Keyboard::RShift );
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::OnKeyDown()  Public
///
///	\param windowsKeyCode The windows virtual key code
///
///	Handle a key being pressed down.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::OnKeyDown( unsigned int windowsKeyCode )
{
	AddNewGUIMsg( GUIMessageMgr::Get().CreateKeyDown( windowsKeyCode ) );
}

/// Handle a key being released
void GUIMgr::OnKeyUp( unsigned int windowsKeyCode )
{
	AddNewGUIMsg( GUIMessageMgr::Get().CreateKeyUp( windowsKeyCode ) );
}

/// Handle a keyboard character pressed and released
void GUIMgr::OnKeyChar( wchar_t charKey )
{
	AddNewGUIMsg( GUIMessageMgr::Get().CreateKeyChar( charKey ) );
}

/// Handle a mouse button being pressed
void GUIMgr::OnMouseDown( GUIDefines::EMouseButtons mouseButton )
{
	// Store the position
	AddNewGUIMsg( GUIMessageMgr::Get().CreateMouseDown( mouseButton, GetCursorPos() ) );
}

/// Handle a mouse button being released
void GUIMgr::OnMouseUp( GUIDefines::EMouseButtons mouseButton )
{
	// Store the position
	AddNewGUIMsg( GUIMessageMgr::Get().CreateMouseUp( mouseButton, GetCursorPos() ) );
}

/// Get if a mouse button is currently pressed down
bool GUIMgr::IsMouseDown( GUIDefines::EMouseButtons mouseButton )
{
#ifdef WINDOWS
	int virtualKeyCode = VK_LBUTTON;
	
	// Get if the left and mouse buttons are swapped in the system settings. GetAsyncKeyState will
	// return the state of the physical button specified, so VK_LBUTTON means the acutal left
	// button and does not take the system swap mouse button setting into account
	if( GetSystemMetrics(SM_SWAPBUTTON) != 0 )
	{
		if( mouseButton == GUIDefines::MB_Left )
			mouseButton = GUIDefines::MB_Right;
		else if( mouseButton == GUIDefines::MB_Right )
			mouseButton = GUIDefines::MB_Left;
	}

	if( mouseButton == GUIDefines::MB_Left )
		virtualKeyCode = VK_LBUTTON;
	else if( mouseButton == GUIDefines::MB_Right )
		virtualKeyCode = VK_RBUTTON;
	else if( mouseButton == GUIDefines::MB_Middle )
		virtualKeyCode = VK_MBUTTON;

	SHORT buttonState = GetAsyncKeyState( virtualKeyCode );

	return (buttonState & 0x8000) != 0;
#elif SFML
	sf::Mouse::Button testButton;
	if( mouseButton == GUIDefines::MB_Left )
		testButton = sf::Mouse::Left;
	else if( mouseButton == GUIDefines::MB_Right )
		testButton = sf::Mouse::Right;
	else
		testButton = sf::Mouse::Middle;

	return sf::Mouse::isButtonPressed(testButton);
#endif
}


/// Get a layout from ID
GUILayout* GUIMgr::GetLayoutByID( uint32 layoutID )
{
	// If the layout is not in the layout map
	if( m_LayoutMap.find( layoutID ) == m_LayoutMap.end() )
	{
		// If the layout is not in the compressed layout map
		if( m_CompLayoutMap.find( layoutID ) == m_CompLayoutMap.end() )
			return NULL;

		// Activate the compressed layout
		CompressedLayout compLayout = m_CompLayoutMap[layoutID];
		DataBlock layoutData( compLayout.pLayoutData, compLayout.dataSize );
		NetSafeSerializer layoutSerializer( &layoutData );
		compLayout.pLayout->TransferLayout( layoutSerializer );
		compLayout.pLayout->HookupCtrls();

		// Store the layout
		m_LayoutMap[ layoutID ] = compLayout.pLayout;

		// Free the compressed data
		delete [] compLayout.pLayoutData;
		m_CompLayoutMap.erase( layoutID );
	}

	// Return the layout
	return m_LayoutMap.find( layoutID )->second;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::SetActiveLayout()  Public
///
///	\param pLayout The layout to make active
///
///	Make a new layout active.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::SetActiveLayout( uint32 layoutID )
{
	// If this layout is unknown
	GUILayout* pLayout = GetLayoutByID( layoutID );
	if( !pLayout )
	{
		TCBREAKX( L"Trying to set unknown layout." );
		return;
	}

	// Store the pending layout
	m_pPendingActiveLayout = pLayout;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::AddLayout()  Public
///
///	\param pLayout The layout to add to the GUI manager
///
///	Add a layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::AddLayout( GUILayout* pNewLayout )
{
	if( !pNewLayout )
		return;

	// See if a layout with this ID already exists
	if( m_LayoutMap.find( pNewLayout->GetLayoutID() ) != m_LayoutMap.end() )
	{
		TCBREAKX( L"Attempting to add a layout with an ID that is already in the layout map." );
		return;
	}
	if( m_CompLayoutMap.find( pNewLayout->GetLayoutID() ) != m_CompLayoutMap.end() )
	{
		TCBREAKX( L"Attempting to add a layout with an ID that is already in the compressed layout map." );
		return;
	}

	// Store the layout
	m_LayoutMap[ pNewLayout->GetLayoutID() ] = pNewLayout;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::AddCompressedLayout()  Public
///
///	\param layout The layout to store
///
/// Add a compressed layout to be populated when needed
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUIMgr::AddCompressedLayout( const CompressedLayout& layout )
{
	if( !layout.pLayout )
		return;

	// See if a layout with this ID already exists
	if( m_LayoutMap.find( layout.pLayout->GetLayoutID() ) != m_LayoutMap.end() )
	{
		TCBREAKX( L"Attempting to add a compressed layout with an ID that is already in the layout map." );
		return;
	}
	if( m_CompLayoutMap.find( layout.pLayout->GetLayoutID() ) != m_CompLayoutMap.end() )
	{
		TCBREAKX( L"Attempting to add a compressed layout with an ID that is already in the compressed layout map." );
		return;
	}

	// Store the layout
	m_CompLayoutMap[ layout.pLayout->GetLayoutID() ] = layout;
}

