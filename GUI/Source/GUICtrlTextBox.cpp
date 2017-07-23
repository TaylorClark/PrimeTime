//=================================================================================================
/*!
	\file GUICtrlTextBox.cpp
	2D Graphics Engine
	Graphical User Interface Button Control Header
	\author Taylor Clark
	\date February 2, 2006

	This source file contains the implementation for the GUI text box control class.
*/
//=================================================================================================

#include "../GUICtrlTextBox.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Resource/ResourceMgr.h"
#include <ctype.h>
#include "../GUILayout.h"

// The rate at which the caret blinks in seconds
static const float BLINK_SPEED = 1.0f;

/// The maximum number of characters allowed in the text box
static const uint32 TEXTBOX_MAX_CHARS = 64;

static const int32 TEXT_VCENTER_OFFSET = 3;


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::GUICtrlLabel()  Public
///
///	The default constructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
GUICtrlTextBox::GUICtrlTextBox() : m_Font( NULL ),
									m_CaretPos( 0 ),
									m_Dims( 200, 30 ),
									m_FirstVisCharIndex( 0 ),
									m_pTextEditCallback( 0 ),
									m_InputFilter( IF_None ),
									m_MaxLen( TEXTBOX_MAX_CHARS )
{
#ifdef _DEBUG
	
	// Add the text
	m_sName = L"Unnamed Text Box";
	GUICtrlProperty* pNewProp = new GUICtrlPropString( L"Initial Text", &m_Text );
	m_PropList.push_back( pNewProp );

	// Add the font property
	pNewProp = new GUICtrlPropFont( L"Font", &m_Font );
	m_PropList.push_back( pNewProp );

	// Add the dimensions property
	pNewProp = new GUICtrlPropInt( L"Width", &m_Dims.x );
	m_PropList.push_back( pNewProp );
	
#endif
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::InternalGetBoundBox()  Public
///
///	\returns The rectangle the encloses the control
///
///	Get the rectangle the encloses the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
Box2i GUICtrlTextBox::InternalGetBoundBox() const
{
	// Return the box
	return Box2i( m_Pos, m_Dims );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::SubclassTransferData()  Public
///
/// \param serializer The serializer object that is used to read or write data
///
///	Load/save the sub-class control data.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlTextBox::SubclassTransferData( Serializer& serializer )
{
	// The text
	serializer.AddData( m_Text );

	// The font
	ResourceID resID = 0;
	if( m_Font.GetObj() )
		resID = m_Font->GetResID();
	serializer.AddData( resID );

	// If we are loading then load the sprite
	if( serializer.InReadMode() )
		m_Font = ResourceMgr::Get().GetTCFont( resID );

	// The width
	serializer.AddData( m_Dims.x );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::OnKeyDown()  Public
///
///	\param windowsKeyCode The windows virtual key code of the key pressed
///	\returns If the key message was handled
///
///	Handle a key being pressed down.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlTextBox::OnKeyDown( unsigned int windowsKeyCode )
{
	// If the user pressed backspace and the caret is not at the beginning VK_BACK = 8
	if( windowsKeyCode == 8 && m_CaretPos > 0 )
	{
		m_Text.erase( --m_CaretPos, 1 );

		// Scroll back if we are at the left most position
		//if( m_CaretPos <= m_FirstVisCharIndex )
		//	m_FirstVisCharIndex = (m_FirstVisCharIndex > 1) ? m_FirstVisCharIndex - 2 : 0;
		if( m_FirstVisCharIndex > 0 )
			--m_FirstVisCharIndex;
	}
	// If the user pressed the delete key and the caret is not at the end VK_DELETE = 0x2e = 46
	else if( windowsKeyCode == 46 && m_CaretPos < m_Text.size() )
	{
		m_Text.erase( m_CaretPos, 1 );

		// If we have a font
		//if( m_Font.GetObj() != NULL )
		//{
		//	// See if we can fit more text in
		//	std::wstring sVisString();
		//	int32 visStrWidth = 0;
		//	do
		//	{
		//		// Scroll back a character to find out if we can fit more text in the visible
		//		// range
		//		--m_FirstVisCharIndex;

		//		// Get the string that is displayed 
		//		sVisString = m_Text.substr( m_FirstVisCharIndex );
		//		visStrWidth = m_Font->CalcStringWidth( sVisString.c_str() );

		//	}while(  > m_Dims.x && m_CaretPos >= m_FirstVisCharIndex );
		//	++m_FirstVisCharIndex;
		//}
	}
	// VK_LEFT = 0x25 = 37
	else if( windowsKeyCode == 37 && m_CaretPos > 0 )
		--m_CaretPos;
	// VK_RIGHT = 0x27 = 39
	else if( windowsKeyCode == 39 && m_CaretPos < m_Text.size() )
		m_CaretPos++;
	// VK_END = 0x23 = 35
	else if( windowsKeyCode == 35 )
		m_CaretPos = (uint32)m_Text.size();
	// VK_HOME = 0x24 = 36
	else if( windowsKeyCode == 36 )
		m_CaretPos = 0;

	// Make the cursor visible
	m_CursorVisTime = 0;

	// Clamp the caret range and scroll if needed
	if( m_CaretPos <= m_FirstVisCharIndex )
		m_FirstVisCharIndex = (m_CaretPos > 0) ? m_CaretPos - 1 : 0;
	while( GetCaretXOffset() > m_Dims.x && m_FirstVisCharIndex < m_Text.size() )
		++m_FirstVisCharIndex;

	// If there is a callback then invoke it
	if( m_pTextEditCallback )
		m_pTextEditCallback( this );

	return false;
}
	
	
///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::OnKeyChar  Public
///
///	\param charKey The character entered
///	\returns If the key message was handled
///
///	Handle a keyboard character pressed and released.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlTextBox::OnKeyChar( wchar_t charKey )
{
	// Check if the character is valid
	if( !IsCharValid( charKey ) )
		return true;

	// If we have too many characters
	if( m_Text.size() >= m_MaxLen )
		return true;

	// If the caret is at the back
	if( m_CaretPos >= m_Text.size() )
		m_Text += charKey;
	// Else insert the new character
	else
		m_Text.insert( m_CaretPos, 1, charKey );

	// Step the caret forward
	m_CaretPos++;

	// Determine if we need to scroll over
	while( GetCaretXOffset() >= m_Dims.x )
		m_FirstVisCharIndex++;

	// If there is a callback then invoke it
	if( m_pTextEditCallback )
		m_pTextEditCallback( this );

	// Get the caret position
	return true;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::SetMaxAllowedLength  Private
///
///	\param maxLen The maximum number of characters allowed to be entered in this text box
///
/// Set the maximum length
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlTextBox::SetMaxAllowedLength( int32 maxLen )
{
	// Clamp the range
	if( maxLen < 1 )
		maxLen = TEXTBOX_MAX_CHARS;
	else if( maxLen >= TEXTBOX_MAX_CHARS )
		maxLen = TEXTBOX_MAX_CHARS;

	// Store the value
	m_MaxLen = maxLen;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::SetText  Private
///
///	\param filterType The type of filtering to impose
///
///	Set the text in the text box
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlTextBox::SetText( const wchar_t* szText, bool callCallback )
{
	// If the string is invalid
	if( !szText )
	{
		m_Text.clear();
	}
	// Else the string is valid
	else
	{
		// Ensure each character is valid
		uint32 textLen = (uint32)wcslen( szText );
		for( uint32 charIndex = 0; charIndex < textLen; ++charIndex )
		{
			if( !IsCharValid( szText[ charIndex ] ) )
				return;
		}

		// Store the text
		m_Text = szText;
	}

	// Set the caret to the end of the text
	m_CaretPos = (uint32)m_Text.size();

	// Ensure each character is valid
	if( callCallback && m_pTextEditCallback )
		m_pTextEditCallback( this );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::SetInputFilter()  Private
///
///	\param filterType The type of filtering to impose
///
///	Set the input filter type.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlTextBox::SetInputFilter( InputFilter filterType )
{
	m_InputFilter = filterType;

	// Go through the text and remove any invalid characters
	for( uint32 charIndex = 0; charIndex < (uint32)m_Text.size(); ++charIndex )
	{
		// If this character is invalid
		if( !IsCharValid( m_Text[charIndex] ) )
		{
			// Remove the character
			m_Text.erase( charIndex, 1 );
			charIndex--;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::IsCharValid()  Private
///
///	\param inputChar The character to test for valid input
///	\returns Whether or not the character is valid for input
///
///	Check if a character is valid for input to this text box.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
bool GUICtrlTextBox::IsCharValid( wchar_t inputChar )
{
	// If there is no filter
	if( m_InputFilter == IF_None )
	{
		// If the character is a space then allow it if it is not the first character
		if( inputChar == L' ' && m_CaretPos > 0 )
			return true;

		// Allow alphanumeric characters
		if( iswalnum( (wint_t)inputChar ) )
			return true;
	}
	// Else if only whole numbers are allowed
	else if( m_InputFilter == IF_PositiveWholeNumbers )
	{
		// Only allow numbers
		if( iswdigit( (wint_t)inputChar ) )
			return true;
	}

	// Return the character is invalid
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::GetCaretXOffset()  Private
///
///	\returns The offset, in pixels, from the left of the text box
///
///	Get the offset from the left of the caret.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 GUICtrlTextBox::GetCaretXOffset() const
{
	if( !m_Font.GetObj() )
		return 0;

	// Get the width of the string up to the point of the caret
	std::wstring prevCaretVisStr;
	if( m_Text.size() > 0 )
		prevCaretVisStr = m_Text.substr( m_FirstVisCharIndex, m_CaretPos - m_FirstVisCharIndex );
	
	return m_Font->CalcStringWidth( prevCaretVisStr.c_str() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::ReformatControl()  Public
///
///	Update the control based on its parameters so it displays correctly.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlTextBox::ReformatControl()
{
	if( m_Font.GetObj() )
		m_Dims.y = m_Font.GetObj()->GetCharHeight() + (TEXT_VCENTER_OFFSET * 2);
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GUICtrlTextBox::Draw()  Public
///
///	Draw the control.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GUICtrlTextBox::Draw() const
{
	if( !m_Font.GetObj() )
		return;

	// Draw the outline
	Box2i boundBox( m_Pos, m_Dims );
	g_pGraphicsMgr->DrawRect( boundBox, 0x00FF0000 );

	// Update the blink time
	m_CursorVisTime += sm_FrameTime;
	m_CursorVisTime = fmod( m_CursorVisTime, BLINK_SPEED );
	bool showCursor = (m_CursorVisTime < (BLINK_SPEED / 2.0f));	
	showCursor &= (this == m_pContainingLayout->GetFocusCtrl() );

	// If we are displaying the caret
	if( showCursor )
	{
		// Set the X position of the caret and draw it
		int32 caretXPos = GetCaretXOffset();
		caretXPos += m_Pos.x;
		Box2i caretBox( caretXPos, m_Pos.y, 1, m_Dims.y );
		g_pGraphicsMgr->DrawRect( caretBox, 0x00FFFFFF );
	}

	// If there is text then draw it
	if( m_Text.size() > 0 )
	{	
		boundBox.pos.y += TEXT_VCENTER_OFFSET;
		boundBox.size.y -= TEXT_VCENTER_OFFSET;

		// Determine if we are displaying text not from the first character
		if( m_FirstVisCharIndex != 0 )
		{
			std::wstring dispStr = m_Text.substr( m_FirstVisCharIndex );
			g_pGraphicsMgr->DrawFontTextClipped( m_Font.GetObj(), dispStr.c_str(), boundBox );
		}
		// Else draw the text normally
		else
			g_pGraphicsMgr->DrawFontTextClipped( m_Font.GetObj(), m_Text.c_str(), boundBox );
	}
}