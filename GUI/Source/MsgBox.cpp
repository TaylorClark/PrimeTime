//=================================================================================================
/*!
	\file MsgBox.cpp
	GUI Library
	Message Box Source
	\author Taylor Clark
	\date July 18, 2006

	This source file contains the implementation for the GUI layout class.
*/
//=================================================================================================

#include "../MsgBox.h"
#include "Graphics2D/GraphicsMgr.h"
#include "../GUICtrlLabel.h"
#include "../GUICtrlButton.h"
#include "../GUICtrlTextBox.h"
#include "Base/MsgLogger.h"
#include "GamePlay/GameDefines.h"
#include "../GUIMessages.h"
#include "Base/TCAssert.h"
#include "Math/MathDefines.h"

static const wchar_t* MSGBOX_CTRL_TEXTBOX = L"msgbox_textbox";
static const wchar_t* MSGBOX_CTRL_OK_BTN = L"msgbox_ok";
static const wchar_t* MSGBOX_CTRL_CANCEL_BTN = L"msgbox_cancel";
static const wchar_t* MSGBOX_CTRL_YES_BTN = L"msgbox_yes";
static const wchar_t* MSGBOX_CTRL_NO_BTN = L"msgbox_no";

static const uint32 MSBBOX_MAX_BUTTONS = 3;
static const wchar_t* MSGBOX_CTRL_OPT1_BTN = L"msgbox_option1";
static const wchar_t* MSGBOX_CTRL_OPT2_BTN = L"msgbox_option2";
static const wchar_t* MSGBOX_CTRL_OPT3_BTN = L"msgbox_option3";

static const int32 MSGBOX_BRDR_OFFSET_X = 30;
static const int32 MSGBOX_BRDR_OFFSET_Y = 28;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MsgBox::ButtonCallback  Public
///
///	\param pButton The button that invoked this callback function
///
///	The message handler for when a button is pressed on a message box
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MsgBox::ButtonCallback( GUICtrlButton* pButton )
{
	if( !pButton || !pButton->GetContainingLayout() )
		return;

	// Close the message box by setting the return value
	MsgBox* pMsgBox = reinterpret_cast<MsgBox*>( pButton->GetContainingLayout() );

	if( pButton->GetName() == MSGBOX_CTRL_OK_BTN )
		pMsgBox->m_RetVal = MsgBox::MBR_OK;
	else if( pButton->GetName() == MSGBOX_CTRL_CANCEL_BTN )
		pMsgBox->m_RetVal = MsgBox::MBR_Cancel;
	else if( pButton->GetName() == MSGBOX_CTRL_YES_BTN )
		pMsgBox->m_RetVal = MsgBox::MBR_Yes;
	else if( pButton->GetName() == MSGBOX_CTRL_NO_BTN )
		pMsgBox->m_RetVal = MsgBox::MBR_No;
	else if( pButton->GetName() == MSGBOX_CTRL_OPT1_BTN )
		pMsgBox->m_RetVal = MsgBox::MBR_Option_1;
	else if( pButton->GetName() == MSGBOX_CTRL_OPT2_BTN )
		pMsgBox->m_RetVal = MsgBox::MBR_Option_2;
	else if( pButton->GetName() == MSGBOX_CTRL_OPT3_BTN )
		pMsgBox->m_RetVal = MsgBox::MBR_Option_3;
	else
		pMsgBox->m_RetVal = MsgBox::MBR_OK;

	// Store the text if there is a text box
	GUICtrlTextBox* pTextBox = reinterpret_cast<GUICtrlTextBox*>( pMsgBox->GetCtrlByName( MSGBOX_CTRL_TEXTBOX ) );
	if( pTextBox )
		pMsgBox->m_sEnteredText = pTextBox->GetText();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MsgBox::GetTextBoxCtrl  Public
///
///	\return A pointer to the message box's text box control, NULL if there is none
///
/// Get the text box control if there is one.  This can be used to set the properties of text entry
///	in the message box.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlTextBox* MsgBox::GetTextBoxCtrl()
{
	return reinterpret_cast<GUICtrlTextBox*>( GetCtrlByName( MSGBOX_CTRL_TEXTBOX ) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MsgBox::CreateMsgBox  Static Public
///
///	\param szMsg The message to display
///	\param layout The type of layout to display
///
///	Display a message box.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MsgBox* MsgBox::CreateMsgBox( const wchar_t* szMsg, EButtonLayout layout )
{
	// Create the new message box
	MsgBox* pRetMsgBox = new MsgBox();
	if( !pRetMsgBox )
		return NULL;

	// Get the message box dimensions
	int32 MSGBOX_HEIGHT = 300;
	int32 MSGBOX_WIDTH = 511;//(int32)(MathDefines::GOLDEN_RATIO * (double)MSGBOX_HEIGHT);
	if( GameDefines::GetMsgBoxBGSpr().GetObj() )
	{
		MSGBOX_WIDTH = GameDefines::GetMsgBoxBGSpr().GetObj()->GetSrcRect().size.x;
		MSGBOX_HEIGHT = GameDefines::GetMsgBoxBGSpr().GetObj()->GetSrcRect().size.y;
	}

	// Set the message box position relative to the screen
	pRetMsgBox->m_WinRect.Set( (g_pGraphicsMgr->GetDisplayDims().x - MSGBOX_WIDTH) / 2, (g_pGraphicsMgr->GetDisplayDims().y - MSGBOX_HEIGHT) / 2, MSGBOX_WIDTH, MSGBOX_HEIGHT ); 
	pRetMsgBox->m_ClientRect.Set( pRetMsgBox->m_WinRect.pos.x + MSGBOX_BRDR_OFFSET_X, pRetMsgBox->m_WinRect.pos.y + MSGBOX_BRDR_OFFSET_Y, MSGBOX_WIDTH - (MSGBOX_BRDR_OFFSET_X * 2), MSGBOX_HEIGHT - (MSGBOX_BRDR_OFFSET_Y * 2) );
	
	// Create the message label
	GUICtrlLabel* pMsgLabel = (GUICtrlLabel*)GUIControl::CreateControl( GUIDefines::CT_Label, pRetMsgBox );
	if( !pMsgLabel )
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to create label for message box." );
	else
	{
		// Wrap the text
		std::wstring sMsg = GameDefines::GetDefaultGameFont()->GetWordWrapString( szMsg, pRetMsgBox->m_ClientRect );

		// Set the message box text and font
		pMsgLabel->SetData( sMsg.c_str(), GameDefines::GetDefaultGameFont() );

		// Position the text
		int32 left = pRetMsgBox->m_ClientRect.pos.x + ((pRetMsgBox->m_ClientRect.size.x - pMsgLabel->GetBoundBox().size.x) / 2);
		int32 top = pRetMsgBox->m_ClientRect.pos.y;
		pMsgLabel->SetPos( Point2i(left,top) );
		pMsgLabel->ReformatControl();
		pRetMsgBox->AddControl( pMsgLabel );
	}

	// Handle the type of button layout
	pRetMsgBox->m_ButtonLayoutType = layout;
	bool moveText = true;
	GUICtrlButton* pOKButton = 0;
	switch( pRetMsgBox->m_ButtonLayoutType )
	{
	case BL_TextEntry:
		{
			// Move the message label up
			Point2i msgPos = pMsgLabel->GetBoundBox().pos;
			msgPos.y = pRetMsgBox->m_ClientRect.pos.y + 10;
			pMsgLabel->SetPos(msgPos);

			// Create the text box object
			GUICtrlTextBox* pTextBox = (GUICtrlTextBox*)GUIControl::CreateControl( GUIDefines::CT_TextBox, pRetMsgBox );
			TCASSERT( pTextBox );
			
			// Calculate the rectangle for the text box
			Box2i textBoxRect;
			textBoxRect.size.x = (pRetMsgBox->m_ClientRect.size.x * 3) / 4;
			textBoxRect.size.y = GameDefines::GetDefaultGameFont()->GetCharHeight() + 6;
			textBoxRect.pos.x = pRetMsgBox->m_ClientRect.pos.x + ((pRetMsgBox->m_ClientRect.size.x - textBoxRect.size.x) / 2);
			textBoxRect.pos.y = pMsgLabel->GetBoundBox().Bottom() + (pMsgLabel->GetBoundBox().size.y / 2);

			// Set the text box data
			pTextBox->SetData( MSGBOX_CTRL_TEXTBOX, GameDefines::GetDefaultGameFont(), textBoxRect );
			pTextBox->ReformatControl();

			// Store the control
			pRetMsgBox->AddControl( pTextBox );

			// Set the focus
			pRetMsgBox->SetFocusCtrl( pTextBox );
			moveText = false;
		}
		// Purposefully fall through to get an OK and cancel button

	case BL_OKCancel:
		{
			//TODO localize
			pOKButton = pRetMsgBox->AddButton( L"OK", MSGBOX_CTRL_OK_BTN, 1, 2 );
			pRetMsgBox->AddButton( L"Cancel", MSGBOX_CTRL_CANCEL_BTN, 2, 2 );
		}
		break;

	case BL_OK:
		{
			//TODO localize
			pOKButton = pRetMsgBox->AddButton( L"OK", MSGBOX_CTRL_OK_BTN, 1, 1 );
		}
		break;

	case BL_YesNo:
		{
			//TODO localize
			pOKButton = pRetMsgBox->AddButton( L"Yes", MSGBOX_CTRL_YES_BTN, 1, 2 );
			pRetMsgBox->AddButton( L"No", MSGBOX_CTRL_NO_BTN, 2, 2 );
		}
		break;

	default:
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Unknown button layout specified (id: %d) when creating message box.", (int32)layout );
	}

	// Center the message vertically
	if( pMsgLabel && moveText && pOKButton )
	{
		const int32 clientRectHeight = pRetMsgBox->m_ClientRect.size.y - pOKButton->GetBoundBox().size.y;
		int32 msgPosY = (clientRectHeight - pMsgLabel->GetBoundBox().size.y) / 2;
		pMsgLabel->SetPos( Point2i(pMsgLabel->GetPos().x, pRetMsgBox->m_ClientRect.pos.y + msgPosY) );
	}

	return pRetMsgBox;
}

/// Create the message box
MsgBox* MsgBox::CreateMsgBoxCustom( const wchar_t* szMsg, const wchar_t* szOption1, const wchar_t* szOption2, const wchar_t* szOption3 )
{
	if( !szOption1 )
		return 0;

	// Get the number of buttons
	int32 numButtons = 1;
	if( szOption2 && szOption3 )
		numButtons = 3;
	else if( szOption2 )
		numButtons = 2;

	// Create the new message box
	MsgBox* pRetMsgBox = new MsgBox();
	if( !pRetMsgBox )
		return 0;
	pRetMsgBox->m_ButtonLayoutType = BL_Custom;

	// Setup the dimensions
	const int32 MSGBOX_HEIGHT = 300;
	const int32 MSGBOX_WIDTH = 511;//(int32)(MathDefines::GOLDEN_RATIO * (double)MSGBOX_HEIGHT);
	pRetMsgBox->m_WinRect.Set( (g_pGraphicsMgr->GetDisplayDims().x - MSGBOX_WIDTH) / 2, (g_pGraphicsMgr->GetDisplayDims().y - MSGBOX_HEIGHT) / 2, MSGBOX_WIDTH, MSGBOX_HEIGHT ); 
	pRetMsgBox->m_ClientRect.Set( pRetMsgBox->m_WinRect.pos.x + MSGBOX_BRDR_OFFSET_X, pRetMsgBox->m_WinRect.pos.y + MSGBOX_BRDR_OFFSET_Y, MSGBOX_WIDTH - (MSGBOX_BRDR_OFFSET_X * 2), MSGBOX_HEIGHT - (MSGBOX_BRDR_OFFSET_Y * 2) );
	
	// Create the message label
	GUICtrlLabel* pMsgLabel = (GUICtrlLabel*)GUIControl::CreateControl( GUIDefines::CT_Label, pRetMsgBox );
	if( !pMsgLabel )
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to create label for message box." );
	else
	{
		// Wrap the text
		std::wstring sMsg = GameDefines::GetDefaultGameFont()->GetWordWrapString( szMsg, pRetMsgBox->m_ClientRect );

		// Set the message box text and font
		pMsgLabel->SetData( sMsg.c_str(), GameDefines::GetDefaultGameFont() );

		// Position the text
		int32 left = pRetMsgBox->m_ClientRect.pos.x + ((pRetMsgBox->m_ClientRect.size.x - pMsgLabel->GetBoundBox().size.x) / 2);
		int32 top = pRetMsgBox->m_ClientRect.pos.y;
		pMsgLabel->SetPos( Point2i(left,top) );
		pRetMsgBox->AddControl( pMsgLabel );
	}

	// Create the option 1 button
	pRetMsgBox->AddButton( szOption1, MSGBOX_CTRL_OPT1_BTN, 1, numButtons );

	// Create the option 2 button
	if( numButtons > 1 )
		pRetMsgBox->AddButton( szOption2, MSGBOX_CTRL_OPT2_BTN, 2, numButtons );

	// Create the option 3 button
	if( numButtons == 3 )
		pRetMsgBox->AddButton( szOption3, MSGBOX_CTRL_OPT3_BTN, 3, numButtons );

	return pRetMsgBox;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MsgBox::AddButton  Private
///
///	\param szText The text on the button
///	\param szCtrlName The name of the button control
///	\param buttonIndex The one-based index of which button is being placed
///	\param numButtons The number of buttons on this message box
///	\returns A pointer to the created button or NULL on failure
///
///	Add a button to the message box.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlButton* MsgBox::AddButton( const wchar_t* szText, const wchar_t* szCtrlName, int32 buttonIndex, int32 numButtons )
{
	// Create the button
	GUICtrlButton* pButton = (GUICtrlButton*)GUIControl::CreateControl( GUIDefines::CT_Button, this );
	TCASSERT( pButton );
	if( !pButton )
		return 0;

	// Set the button's text and images
	pButton->SetData( szCtrlName, GameDefines::GetNormalMsgBoxButtonSpr(), GameDefines::GetOverMsgBoxButtonSpr(), GameDefines::GetPressedMsgBoxButtonSpr(), szText, GameDefines::GetDefaultGameFont() );

	Vector2i buttonOffset;
	buttonOffset.y = (m_ClientRect.size.y - pButton->GetBoundBox().size.y) - 3;
	const int32 buttonWidth = pButton->GetBoundBox().size.x;
	
	// If this is the only button then center it
	if( numButtons == 1 )
		buttonOffset.x = (m_ClientRect.size.x - buttonWidth) / 2;
	// Else if there are a pair of buttones
	else if( numButtons == 2 )
	{
		int thirdWidth = (m_ClientRect.size.x / 3);

		// If this button is the left button
		if( buttonIndex == 1 )
			buttonOffset.x = thirdWidth - (buttonWidth / 2);
		// Else this is the right button
		else
			buttonOffset.x = (thirdWidth * 2) - (buttonWidth / 2);
	}
	// Else if there are 3 buttons
	else if( numButtons == 3 )
	{
		// Get the total width of the message box minus the button widths
		int32 emptySpace = m_ClientRect.size.x - (buttonWidth * 3);

		// Get the space between the buttons and the midpoint of the client rectangle
		int32 middleSpace = emptySpace / 3;
		int32 midPoint = m_ClientRect.size.x / 2;

		// If this is the left button
		if( buttonIndex  == 1 )
			buttonOffset.x = midPoint - (buttonWidth / 2) - middleSpace - buttonWidth;
		// Else if this is the middle button
		else if( buttonIndex == 2 )
			buttonOffset.x = midPoint - (buttonWidth / 2);
		// Else this is the right button
		else
			buttonOffset.x = midPoint + (buttonWidth / 2) + middleSpace;
	}
	// Else there is an error
	else
		TCBREAKX( L"Invalid number of message box buttons." );

	// Set the position
	pButton->SetPos( m_ClientRect.pos + buttonOffset );

	// Set the callback and add the button to the layout
	pButton->SetClickCallback( ButtonCallback );
	AddControl( pButton );

	// Return the button
	return pButton;
}

/// Handle a GUI message
bool MsgBox::HandleMessage( GUIMessage* pMsg )
{
	if( !pMsg )
		return false;

	// If the message is a key down message
	if( pMsg->GetType() == GUIDefines::GMT_KeyDown )
	{
		GUIMessageKeyDown* pKeyMsg = reinterpret_cast<GUIMessageKeyDown*>(pMsg);

		// VK_RETURN = 0x0D = 13
		const uint32 KEYCODE_ENTER = 13;

		// VK_ESCAPE = 0x1B = 27
		const uint32 KEYCODE_ESCAPE = 27;

		const uint32 KEYCODE_Y = (uint32)L'Y';
		const uint32 KEYCODE_N = (uint32)L'N';

		switch( m_ButtonLayoutType )
		{
		case BL_OK:
			if( pKeyMsg->GetKeyCode() == KEYCODE_ENTER || pKeyMsg->GetKeyCode() == KEYCODE_ESCAPE )
			{
				m_RetVal = MsgBox::MBR_OK;
				return true;
			}
			break;

		case BL_OKCancel:
		case BL_TextEntry:
			{
				// Store the text if there is a text box
				GUICtrlTextBox* pTextBox = reinterpret_cast<GUICtrlTextBox*>( GetCtrlByName( MSGBOX_CTRL_TEXTBOX ) );
				if( pTextBox )
					m_sEnteredText = pTextBox->GetText();

				if( pKeyMsg->GetKeyCode() == KEYCODE_ENTER )
				{
					m_RetVal = MsgBox::MBR_OK;
					return true;
				}
				if( pKeyMsg->GetKeyCode() == KEYCODE_ESCAPE )
				{
					m_RetVal = MsgBox::MBR_Cancel;
					return true;
				}
			}
			break;

		case BL_YesNo:
			if( pKeyMsg->GetKeyCode() == KEYCODE_ENTER || pKeyMsg->GetKeyCode() == KEYCODE_Y )
			{
				m_RetVal = MsgBox::MBR_Yes;
				return true;
			}
			if( pKeyMsg->GetKeyCode() == KEYCODE_ESCAPE || pKeyMsg->GetKeyCode() == KEYCODE_N )
			{
				m_RetVal = MsgBox::MBR_No;
				return true;
			}
			break;

		case BL_Custom:
			{
				// Find the buttons
				GUICtrlButton* pButtons[MSBBOX_MAX_BUTTONS] = {0,0,0};
				uint32 firstLetters[MSBBOX_MAX_BUTTONS] = {0,0,0};
				pButtons[0] = (GUICtrlButton*)GetCtrlByName( MSGBOX_CTRL_OPT1_BTN );
				pButtons[1] = (GUICtrlButton*)GetCtrlByName( MSGBOX_CTRL_OPT2_BTN );
				pButtons[2] = (GUICtrlButton*)GetCtrlByName( MSGBOX_CTRL_OPT3_BTN );

				// See if each button has a different first letter in the text
				bool uniqueFirstLetters = true;
				for( uint32 btnIndex = 0; btnIndex < MSBBOX_MAX_BUTTONS && uniqueFirstLetters; ++btnIndex )
				{
					// If this string is empty
					if( !pButtons[btnIndex] || pButtons[btnIndex]->GetText().length() == 0 )
						continue;

					// Store the first letter, cast to a uint32 to match the key code type
					firstLetters[btnIndex] = (uint32)pButtons[btnIndex]->GetText()[0];

					// Test against the other buttons
					for( uint32 btnTestIndex = btnIndex + 1; btnTestIndex < MSBBOX_MAX_BUTTONS; ++btnTestIndex )
					{
						// If this test string is empty
						if( !pButtons[btnTestIndex] || pButtons[btnTestIndex]->GetText().length() == 0 )
							continue;

						// If the first letters match
						if( firstLetters[btnIndex] == pButtons[btnTestIndex]->GetText()[0] )
						{
							uniqueFirstLetters = false;
							break;
						}
					}
				}

				// If the first letters are unique amongst the buttons
				if( uniqueFirstLetters )
				{
					// See if the key pressed is one of the button letters
					EMsgBoxReturn retVals[MSBBOX_MAX_BUTTONS] = { MBR_Option_1, MBR_Option_2, MBR_Option_3 };
					for( uint32 btnIndex = 0; btnIndex < MSBBOX_MAX_BUTTONS; ++btnIndex )
					{
						if( pKeyMsg->GetKeyCode() == firstLetters[btnIndex] )
						{
							m_RetVal = retVals[ btnIndex ];
							return true;
						}
					}
				}
			}
                
            default:
			break;
		}
	}

	return GUILayout::HandleMessage( pMsg );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  MsgBox::Draw  Public
///
///	Draw the message box.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void MsgBox::Draw() const
{
	// If the background sprite failed to load then draw a black rectangle
	if( !GameDefines::GetMsgBoxBGSpr().GetObj() )
	{
		g_pGraphicsMgr->FillRect( m_WinRect, 0x00000000 );
		g_pGraphicsMgr->DrawRect( m_WinRect, 0x00FF0000 );
	}
	else
		g_pGraphicsMgr->DrawSprite( m_WinRect.pos, GameDefines::GetMsgBoxBGSpr().GetObj() );

	// Allow drawing of the rest of the layout
	GUILayout::Draw();
}