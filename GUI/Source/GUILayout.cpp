//=================================================================================================
/*!
	\file GUILayout.cpp
	2D Graphics Engine
	GUI Layout Source
	\author Taylor Clark
	\date January 31, 2006

	This source file contains the implementation for the GUI layout class.
*/
//=================================================================================================

#include "../GUILayout.h"
#include "../GUIControl.h"
#include "../GUIMessages.h"
#include "../GUIMgr.h"
#include "Base/DataBlock.h"
#include "Base/TCAssert.h"
#include "Base/Serializer.h"
#include "Base/MsgLogger.h"
#include "Graphics2D/GraphicsMgr.h"
#include "../GUICtrlButton.h"
#include "../GUICtrlSprite.h"
#include "Resource/ResourceMgr.h"

const wchar_t* GUILayout::BG_CTRL_NAME = L"background";


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::HandleMessage  Public
///
///	\param pMsg The GUI message to handle
///
///	Handle a GUI message in the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUILayout::HandleMessage( GUIMessage* pMsg )
{
	if( !pMsg )
		return false;

	// Handle the message type
	bool msgHandled = false;

	// If this message is a key down message
	if( pMsg->GetType() == GUIDefines::GMT_KeyDown )
	{
		GUIMessageKeyDown* pKeyDownMsg = reinterpret_cast<GUIMessageKeyDown*>( pMsg );

		// If the escape key or backspace was pressed (VK_ESCAPE == 0x1b == 27) (VK_BACK == 8)
		if( pKeyDownMsg->GetKeyCode() == 27 || pKeyDownMsg->GetKeyCode() == 8 )
		{
			// See if there is a back button to press
			GUICtrlButton* pButton = reinterpret_cast<GUICtrlButton*>( GetCtrlByName( L"back_button" ) );
			if( pButton )
			{
				// By default, escape executes the back button
				pButton->ExecuteCallback();
				msgHandled = true;
			}
		}
	}

	// If there is a focused control and we haven't handled the message yet then handle the key messages
	if( m_pFocusCtrl && !msgHandled )
	{
		switch( pMsg->GetType() )
		{
		case GUIDefines::GMT_KeyDown:
			{
				GUIMessageKeyDown* pKeyDownMsg = reinterpret_cast<GUIMessageKeyDown*>( pMsg );
				msgHandled = m_pFocusCtrl->OnKeyDown( pKeyDownMsg->GetKeyCode() );
			}
			break;

		case GUIDefines::GMT_KeyUp:
			{
				GUIMessageKeyUp* pKeyUpMsg = reinterpret_cast<GUIMessageKeyUp*>( pMsg );
				msgHandled = m_pFocusCtrl->OnKeyUp( pKeyUpMsg->GetKeyCode() );
			}
			break;

		case GUIDefines::GMT_KeyChar:
			{
				GUIMessageKeyChar* pKeyCharMsg = reinterpret_cast<GUIMessageKeyChar*>( pMsg );
				msgHandled = m_pFocusCtrl->OnKeyChar( pKeyCharMsg->GetChar() );
			}
            break;
                
        default:
            // Stop warnings on mac
            break;
		}
	}

	// If the message wasn't handled yet then see if a control can handle the mouse message
	if( !msgHandled )
	{
		switch( pMsg->GetType() )
		{
		case GUIDefines::GMT_MouseDown:
			{
				// Get the mouse message and find the control at the mouse message position
				GUIMessageMouseDown* pMouseDownMsg = reinterpret_cast<GUIMessageMouseDown*>( pMsg );
				GUIControl* pMouseCtrl = GetControlAtPoint( pMouseDownMsg->GetPos() );

				// Store the mouse down control
				m_pMouseDownControl[pMouseDownMsg->GetButton()] = pMouseCtrl;

				// If the user pressed the mouse button over control
				if( pMouseCtrl )
				{
					// If the button pressed was the left mouse button then set the focus control
					if( pMouseDownMsg->GetButton() == GUIDefines::MB_Left && pMouseCtrl->CanReceiveFocus() )
						m_pFocusCtrl = pMouseCtrl;
					
					// Finally send the mouse down message
					msgHandled = pMouseCtrl->OnMouseDown( pMouseDownMsg->GetButton(), pMouseDownMsg->GetPos() );
				}
			}
			break;

		case GUIDefines::GMT_MouseUp:
			{
				// Get the mouse message and find the control at the mouse message position
				GUIMessageMouseUp* pMouseUpMsg = reinterpret_cast<GUIMessageMouseUp*>( pMsg );
				GUIControl* pMouseCtrl = GetControlAtPoint( pMouseUpMsg->GetPos() );

				// Update the mouse down control
				GUIControl* pMouseDownCtrl = m_pMouseDownControl[pMouseUpMsg->GetButton()];
				if( pMouseDownCtrl && pMouseCtrl != pMouseDownCtrl )
					pMouseDownCtrl->OnMouseUpNotOver( pMouseUpMsg->GetButton(), pMouseUpMsg->GetPos() );
								
				// If there is a control at the mouse position then send a message
				if( pMouseCtrl )
					msgHandled = pMouseCtrl->OnMouseUp( pMouseUpMsg->GetButton(), pMouseUpMsg->GetPos() );

				// Clear the mouse down variable for this mouse button
				m_pMouseDownControl[pMouseUpMsg->GetButton()] = 0;
			}
            break;
                
        default:
            // Stop warnings on mac
            break;
		}
	}

	// Return if the message was handled
	return msgHandled;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::HookupCtrls  Public
///
///	Link the control objects to their corresponding object found in the layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::HookupCtrls()
{
	// Go through the expected controls
	for( ExpecCtrlList::iterator iterCtrl = m_ExpectedCtrls.begin(); iterCtrl != m_ExpectedCtrls.end(); ++iterCtrl )
	{
		if( !iterCtrl->ppCtrl )
			continue;

		// Get the control
		GUIControl* pCtrl = GetCtrlByName( iterCtrl->sCtrlName.c_str() );
		*(iterCtrl->ppCtrl) = pCtrl;

		// If it is of the wrong type then complain and clear the data
		if( pCtrl && pCtrl->GetCtrlType() != iterCtrl->type )
		{
			MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Hooking up control %s as wrong type.", iterCtrl->sCtrlName.c_str() );
			*(iterCtrl->ppCtrl) = 0;
			continue;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::OnActivate  Public
///
///	A message handler called when the layout is activated.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::OnActivate()
{
	m_Controls = m_FullControlList;

	// Go through the controls and play sprites
	for( ControlList::iterator iterCtrl = m_Controls.begin(); iterCtrl != m_Controls.end(); ++iterCtrl )
	{
		if( (*iterCtrl)->GetCtrlType() == GUIDefines::CT_Sprite )
		{
			GUICtrlSprite* pSpriteCtrl = reinterpret_cast<GUICtrlSprite*>( *iterCtrl );
			pSpriteCtrl->m_Sprite.Play();
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::Draw  Public
///
///	Draw the layout
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::Draw() const
{
	// Draw each control
	for( std::list< GUIControl* >::const_iterator iterCtrl = m_Controls.begin();
			iterCtrl != m_Controls.end();
			++iterCtrl )
	{
		if( (*iterCtrl)->IsVisible() )
			(*iterCtrl)->Draw();

#ifdef _DEBUG
		g_pGraphicsMgr->DrawRect( (*iterCtrl)->GetBoundBox(), 0xFF00FF00 );
#endif
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::GetDrawableBG  Private
///
/// Get the background sprite if it is a valid target for drawing.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlSprite* GUILayout::GetDrawableBG()
{
	GUIControl* pBGCtrl = GetCtrlByName( BG_CTRL_NAME );
	if( !pBGCtrl || pBGCtrl->GetCtrlType() != GUIDefines::CT_Sprite )
		return NULL;
	GUICtrlSprite* pBGSprite = (GUICtrlSprite*)pBGCtrl;

	// Ensure the sprite has a valid image, it is static, and it is the back-most control
	if( !pBGSprite->m_Sprite.GetImage() || !pBGSprite->IsStaticVisual() || pBGSprite != m_Controls.front() )
		return NULL;

	return pBGSprite;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::DrawStaticsToBackground  Public
///
/// Draw the static items to the background
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::DrawStaticsToBackground()
{
#ifdef PREVENT_GUI_OPTIMIZE
	return;
#else
	// Get the background sprite if there is one
	m_pBGSprite = GetDrawableBG();
	if( !m_pBGSprite )
		return;

	// Set the background image as the render target
	g_pGraphicsMgr->SetTempRenderTarget( m_pBGSprite->m_Sprite.GetSprite()->GetImageNonConst() );

	// Starting from the back (one control forward to pass the background), go through each
	// control
	m_Controls.clear();
	m_Controls.push_back( m_pBGSprite );
	ControlList::iterator iterCtrl = m_FullControlList.begin();
	++iterCtrl;
	for( ; iterCtrl != m_FullControlList.end(); ++iterCtrl )
	{
		// If this control is not static then add it to the list and continue on
		if( !(*iterCtrl)->IsStaticVisual() )
		{
			m_Controls.push_back( *iterCtrl );
			continue;
		}

		// Draw the control
		if( (*iterCtrl)->IsVisible() )
			(*iterCtrl)->Draw();
	}

	// Return the drawing to the back buffer
	g_pGraphicsMgr->ClearTempRenderTarget();
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::UndoBackgroundMods  Public
///
/// Undo any background changes necessary
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::UndoBackgroundMods()
{
	// If there is no background then there was no optimization
	if( !m_pBGSprite )
		return;

	// Get the background image
	TCImage* pImg = ResourceMgr::Get().GetTCImage( m_pBGSprite->m_Sprite.GetSprite()->GetImgResID() ).GetObj();

	// Reload the image data
	ResourceMgr::Get().ReloadImage( pImg, false );
	m_pBGSprite = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::Update  Public
///
///	\param frameTime The elapsed frame time
///
///	Update the layout
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::Update( float32 frameTime )
{
	// Update each control
	for( std::list< GUIControl* >::const_iterator iterCtrl = m_Controls.begin();
			iterCtrl != m_Controls.end();
			++iterCtrl )
	{
		(*iterCtrl)->Update( frameTime );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::GetControlAtPoint  Public
///
///	\param testPt The point to use for testing
///	\returns A pointer to the control under the specified point, NULL if there is none.
///
///	Find a control at a specified point.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUIControl* GUILayout::GetControlAtPoint( const Point2i& testPt )
{
	// Go through the controls
	for( ControlList::reverse_iterator iterCtrl = m_Controls.rbegin();
			iterCtrl != m_Controls.rend();
			++iterCtrl )
	{
		// If the control is not visible then skip it
		if( !(*iterCtrl)->IsVisible() )
			continue;

		if( (*iterCtrl)->GetBoundBox().IsPointInside( testPt ) )
			return *iterCtrl;
	}

	// The test point was not in any of the controls so return NULL
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUIMgr::UpdateMouseEnterLeaveMsgs  Public
///
///	Update the mouse enter and leave messages.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::UpdateMouseEnterLeaveMsgs()
{
	// Get the mouse position
	Point2i cursorPos = GUIMgr::Get().GetCursorPos();

	// Get the control we are currently over
	GUIControl* pOverCtrl = GetControlAtPoint( cursorPos );

	// If we are currently over a control
	if( m_pMouseOverControl )
	{
		// If we are not over the mouse over control then leave it
		if( pOverCtrl != m_pMouseOverControl )
			m_pMouseOverControl->OnMouseLeave();
		// Else nothing has changed so bail
		else
			return;
	}

	// Store the control we are over
	m_pMouseOverControl = pOverCtrl;	
	
	// If we are over a new control
	if( m_pMouseOverControl )
		m_pMouseOverControl->OnMouseEnter();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::Clear  Public
///
///	Clear the data held by this layout
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::Clear()
{
	// Go through each control and free it
	for( std::list< GUIControl* >::iterator iterCtrl = m_FullControlList.begin(); iterCtrl != m_FullControlList.end(); ++iterCtrl )
		delete *iterCtrl;
	m_FullControlList.clear();
	m_Controls.clear();

	// Clear the member variables
	m_pFocusCtrl = NULL;
	m_pMouseOverControl = NULL;
	for( int mouseButtonIndex = 0; mouseButtonIndex < GUIDefines::MB_COUNT; ++mouseButtonIndex )
		m_pMouseDownControl[mouseButtonIndex] = 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::IsArrowKey  Static Public
///
///	\param winKey The windows key code
///
///	Tests if a key code is an arrow key.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUILayout::IsArrowKey( unsigned int winKey )
{
#ifndef VK_LEFT
	const int VK_LEFT = 0x25;
	const int VK_UP = 0x26;
	const int VK_RIGHT = 0x27;
	const int VK_DOWN = 0x28;
#endif

	if( winKey == VK_UP || winKey == VK_DOWN || winKey == VK_LEFT || winKey == VK_RIGHT )
		return true;

	// This is not an arrow key
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::TransferLayout  Public
///
///	\param serializer The serializer used to transfer the data
///
///	Load/save a layout.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::TransferLayout( Serializer& serializer )
{
	// If we are loading then clear our data
	if( serializer.InReadMode() )
		Clear();

	// Transfer the layout ID
	uint32 layoutID = m_LayoutID;
	serializer.AddData( layoutID );
	m_LayoutID = layoutID;

	// Read in the number of controls
	uint32 numCtrls = (uint32)m_FullControlList.size();
	serializer.AddData( numCtrls );

	// Transfer each control
	for( uint32 curCtrlIndex = 0; curCtrlIndex < numCtrls; ++curCtrlIndex )
	{
		// If we are reading
		if( serializer.InReadMode() )
		{
			// Read in the type
			int32 ctrlTypeInt = (int32)GUIDefines::CT_Error;
			serializer.AddData( ctrlTypeInt );
			
			// The game manager should not be saved
			if( ctrlTypeInt == (int)GUIDefines::CT_GameMgr )
				continue;

			// Create the control
			GUIControl* pCtrl = GUIControl::CreateControl( (GUIDefines::EControlType)ctrlTypeInt, this );
			if( !pCtrl )
			{
				MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to create control with type: %d", ctrlTypeInt );
				continue;
			}

			// Read in the control
			pCtrl->TransferData( serializer );
			
			// Add the control
			m_FullControlList.push_back( pCtrl );
		}
		// Else we are writing
		else
		{
			// Get to the control based on its index
			uint32 ctrlIndex = 0;
			ControlList::iterator iterCtrl;
			for( iterCtrl = m_FullControlList.begin();
					(iterCtrl != m_FullControlList.end()) && (ctrlIndex < curCtrlIndex);
					++iterCtrl, ++ctrlIndex )
			{}

			// If we couldn't get to the control
			int32 ctrlTypeVal = (int32)GUIDefines::CT_Error;
			if( ctrlIndex != curCtrlIndex || iterCtrl == m_FullControlList.end() )
			{
				MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to save control with index %d", curCtrlIndex );

				// Write an invalid type
				serializer.AddData( ctrlTypeVal );

				// Continue to the next control
				continue;
			}

			// Write the type
			ctrlTypeVal = (int32)(*iterCtrl)->GetCtrlType();
			serializer.AddData( ctrlTypeVal );

			// Store the control
			(*iterCtrl)->TransferData( serializer );
		}
	}

	// Update each control
	for( ControlList::iterator iterCtrl = m_FullControlList.begin(); iterCtrl != m_FullControlList.end(); ++iterCtrl )
		(*iterCtrl)->ReformatControl();

	// If the layout is being loaded then store the control list
	m_Controls = m_FullControlList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::GetCtrlByName  Public
///
///	\param szCtrlName The control name
///	\returns A pointer to the control that has the passed in name or NULL if none can be found
///
///	Get a control by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUIControl* GUILayout::GetCtrlByName( const wchar_t* szCtrlName )
{
	// Go through the controls
	for(ControlList::iterator iterCtrl = m_Controls.begin(); iterCtrl != m_Controls.end(); ++iterCtrl )
	{
		// If this control has the right name
		if( (*iterCtrl)->GetName() == szCtrlName )
			return *iterCtrl;
	}

	// We couldn't find the control
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::GetCtrlByName  Public
///
///	\param szCtrlName The control name
///	\returns A pointer to the control that has the passed in name or NULL if none can be found
///
///	Get a control by name.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
const GUIControl* GUILayout::GetCtrlByName( const wchar_t* szCtrlName ) const
{
	// Go through the controls
	for(std::list< GUIControl* >::const_iterator iterCtrl = m_Controls.begin(); iterCtrl != m_Controls.end(); ++iterCtrl )
	{
		// If this control has the right name
		if( (*iterCtrl)->GetName() == szCtrlName )
			return *iterCtrl;
	}

	// We couldn't find the control
	return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayout::AddControl  Public
///	\param pNewCtrl The control to add to the layout
///
///	Add a control to the layout that is on top of all others.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::AddControl( GUIControl* pNewCtrl )
{
	// Don't allow NULL controls
	TCASSERTX( pNewCtrl != NULL, L"Can not add NULL control to layout." );
	if( !pNewCtrl )
		return;

	// Ensure the control is not already in the layout
	for( ControlList::iterator iterCtrl = m_FullControlList.begin(); iterCtrl != m_FullControlList.end(); ++iterCtrl )
	{
		if( *iterCtrl == pNewCtrl )
		{
			TCBREAKX( L"Can't add a control to a layout twice." );
			return;
		}
	}

	// Add the control
	m_FullControlList.push_back( pNewCtrl );
	m_Controls.push_back( pNewCtrl );
	pNewCtrl->SetContainingLayout( this );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::RemoveControl  Public
///
///	\param pCtrl The control to remove
///
/// Remove a control from the layout, but do not free the memory
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUILayout::RemoveControl( GUIControl* pCtrl )
{
	// Find the control in the full list first
	ControlList::iterator iterCtrl = GetCtrlIter( pCtrl, true );
	if( iterCtrl == m_FullControlList.end() )
		return false;

	// Remove it from the full list
	m_FullControlList.erase( iterCtrl );

	// Get the control's iterator and confirm it is in the control list
	iterCtrl = GetCtrlIter( pCtrl );
	if( iterCtrl != m_Controls.end() )
	{
		// Remove it from the list
		m_Controls.erase( iterCtrl );
	}

	// Clear any references to the control
	if( m_pMouseOverControl == pCtrl )
		m_pMouseOverControl = NULL;
	for( uint32 mouseBtnIndex = 0; mouseBtnIndex < GUIDefines::MB_COUNT; ++mouseBtnIndex )
		if( m_pMouseDownControl[mouseBtnIndex] == pCtrl )
			m_pMouseDownControl[mouseBtnIndex] = NULL;
	if( m_pFocusCtrl == pCtrl )
		m_pFocusCtrl = NULL;

	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GUILayout::GetAllControlsOfType()  Private
///	\param controlType The type of control for which to search
///
///	Get all the controls of a type in a layout
///////////////////////////////////////////////////////////////////////////////////////////////////
GUILayout::ControlList GUILayout::GetAllControlsOfType( GUIDefines::EControlType controlType )
{
	std::list< GUIControl* > retList;

	for( std::list< GUIControl* >::iterator iterCtrl = m_FullControlList.begin(); iterCtrl != m_FullControlList.end(); ++iterCtrl )
	{
		if( (*iterCtrl)->GetCtrlType() == controlType )
			retList.push_back( *iterCtrl );
	}

	return retList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::GetCtrlIter()  Private
///
///	\param pCtrl The control for which to find an iterator
///	\param useFullList If true then the full control list will be searched for the control and the
///						iterator returned will be for the full list, not the m_Controls list
///
///	A helper function to get the itereator within the controls list for this layour for a specific
///	control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUILayout::ControlList::iterator GUILayout::GetCtrlIter( GUIControl* pCtrl, bool useFullList )
{
	// NULL values are not allowed in the list
	if( !pCtrl )
	{
		if( useFullList )
			return m_FullControlList.end();
		return m_Controls.end();
	}

	// Find the control in the list
	ControlList::iterator iterCtrl = m_Controls.begin();
	ControlList::iterator endIter = m_Controls.end();
	if( useFullList )
	{
		iterCtrl = m_FullControlList.begin();
		endIter = m_FullControlList.end();
	}

	// Find the control
	while( iterCtrl != endIter )
	{
		if( *iterCtrl == pCtrl )
			return iterCtrl;
		++iterCtrl;
	}

	// We did not find the control so return the end iterator
	return endIter;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::CenterCtrlHorz()  Public
///
///	\param pCtrl The control to center
///
/// Center the control horizontally
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::CenterCtrlHorz( GUIControl* pCtrl )
{
	// Ensure a valid control
	if( !pCtrl )
		return;

	// Center the control
	Point2i ctrlPos = pCtrl->GetPos();
	ctrlPos.x = (g_pGraphicsMgr->GetDisplayDims().x - pCtrl->GetBoundBox().size.x) / 2;
	pCtrl->SetPos( ctrlPos );
}

/// Center the control vertically
void GUILayout::CenterCtrlVert( GUIControl* pCtrl )
{
	// Ensure a valid control
	if( !pCtrl )
		return;

	// Center the control
	Point2i ctrlPos = pCtrl->GetPos();
	ctrlPos.x = (g_pGraphicsMgr->GetDisplayDims().y - pCtrl->GetBoundBox().size.y) / 2;
	pCtrl->SetPos( ctrlPos );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::BringCtrlToFront()  Public
///
///	\param pCtrl The control to move to the front
///
///	Move a control to the front of the layout, in front of the rest of the controls.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::BringCtrlToFront( GUIControl* pCtrl )
{
	// Get the control's iterator and confirm it is in the control list
	ControlList::iterator iterCtrl = GetCtrlIter( pCtrl, true );
	if( iterCtrl == m_FullControlList.end() )
		return;

	// Remove it from the list
	m_FullControlList.erase( iterCtrl );

	// Add it to the back of the list, the top-most layer
	m_FullControlList.push_back( pCtrl );
	
	// Update the active list
	m_Controls = m_FullControlList;
}


#ifdef _DEBUG
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::SendCtrlToBack()  Public
///
///	\param pCtrl The control to move to the back
///
///	Move a control to the back of the layout, behind the rest of the controls.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::SendCtrlToBack( GUIControl* pCtrl )
{
	// Get the control's iterator and confirm it is in the control list
	ControlList::iterator iterCtrl = GetCtrlIter( pCtrl, true );
	if( iterCtrl == m_FullControlList.end() )
		return;

	// Remove it from the list
	m_FullControlList.erase( iterCtrl );

	// Add it to the front of the list, the back of the layout
	m_FullControlList.push_front( pCtrl );
	
	// Update the active list
	m_Controls = m_FullControlList;
}


/// Move a control back in the Z order
void GUILayout::MoveCtrlBack( GUIControl* pCtrl )
{
	// Get the control's iterator and confirm it is in the control list
	ControlList::iterator iterCtrl = GetCtrlIter( pCtrl, true );
	if( iterCtrl == m_FullControlList.end() || iterCtrl == m_FullControlList.begin() )
		return;

	// Step back
	ControlList::iterator prevIter = iterCtrl;
	--prevIter;

	// Step back and insert the control
	m_FullControlList.insert( prevIter, pCtrl );

	// Remove the original iterator from the list
	m_FullControlList.erase( iterCtrl );
	
	// Update the active list
	m_Controls = m_FullControlList;
}


/// Move a control forward in the Z order
void GUILayout::MoveCtrlForward( GUIControl* pCtrl )
{
	// Get the control's iterator and confirm it is in the control list
	ControlList::iterator iterCtrl = GetCtrlIter( pCtrl, true );
	if( iterCtrl == m_FullControlList.end() )
		return;

	// Step forward
	ControlList::iterator nextIter = iterCtrl;
	++nextIter;
	if( nextIter == m_FullControlList.end() )
		return;
	++nextIter;
	if( nextIter == m_FullControlList.end() )
		return;

	// Insert the control
	m_FullControlList.insert( nextIter, pCtrl );

	// Remove the original iterator from the list
	m_FullControlList.erase( iterCtrl );
	
	// Update the active list
	m_Controls = m_FullControlList;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUILayout::DeleteControl()  Public
///
///	\param pCtrl The control to remove
///
///	Remove a control from the layout and free the memory.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUILayout::DeleteControl( GUIControl* pCtrl )
{
	if( RemoveControl(pCtrl) )
		delete pCtrl;
}

#endif // _DEBUG