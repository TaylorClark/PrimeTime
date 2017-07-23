//=================================================================================================
/*!
	\file TCFont.cpp
	2D Graphics Engine
	Font Source
	\author Taylor Clark
	\date January 25, 2006

	This source file contains the implementation for the font class.
*/
//=================================================================================================

#include <fstream>
#include "Base/Types.h"
#include "../PrivateInclude/TCFontImpl.h"
#include "Base/StringFuncs.h"
#include "Graphics2D/GraphicsMgr.h"
#include "Base/MsgLogger.h"

const wchar_t CHAR_SPACE = L' ';
const wchar_t CHAR_NEWLINE = L'\n';


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TCFontImpl::CalcStringWidth()  Public
///
///	\param szText The text to measure
///	\returns The width of the string in pixels.
///
///	Calculate the width of a string in pixels.  New line characters are ignored.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 TCFontImpl::CalcStringWidth( const wchar_t* szText ) const
{
	int32 retWidth = 0;

	// Get the string length
	size_t strLen = wcslen( szText );
	if( strLen < 1 )
		return retWidth;

	// Go through each character
	int32 curLineWidth = 0;
	const int32 SPACE_WIDTH = (m_CharHeight * 3) / 4;
	for( size_t charIndex = 0; charIndex < strLen; ++charIndex )
	{
		// Get the character
		wchar_t curChar = szText[ charIndex ];

		// If this is a new-line character
		if( curChar == CHAR_NEWLINE )
		{
			// If the line width is wider than the current value then store it
			if( curLineWidth > retWidth )
				retWidth = curLineWidth;
			curLineWidth = 0;
		}
		// Else if this is a space
		else if( curChar == CHAR_SPACE )
		{
			curLineWidth += SPACE_WIDTH + m_CharSpacing;
			continue;
		}

		TCFontImpl::GlyphMap::const_iterator iterChar = m_GlyphMap.find( curChar );
		if( iterChar == m_GlyphMap.end() )
			continue;

		// Get the image rectangle
		const Box2i& charImgRect = iterChar->second;

		// Step to the next characters
		curLineWidth += charImgRect.size.x + m_CharSpacing;
	}

	// If the final line's width is wider than the current value then store it
	if( curLineWidth > retWidth )
		retWidth = curLineWidth;

	return retWidth;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TCFontImpl::GetWordWrapString()  Public
///
///	\param szStr The string to word wrap
///	\param boundBox The box to attempt to fit the text within
///	\returns A string with \n's inserted to indicate line breaks
///
///	Make a string word wrap within a bounding box.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
std::wstring TCFontImpl::GetWordWrapString( const wchar_t* szStr, const Box2i& boundBox ) const
{
	std::wstring sRetString;

	// Get the string length
	std::wstring sOrigString( szStr );
	size_t strLen = wcslen( szStr );
	if( strLen < 1 )
		return sRetString;

	// Go through each character
	int curLineWidth = 0;
	int curTextHeight = m_CharHeight;

	// Find the end of the first word
	const int32 SPACE_WIDTH = (m_CharHeight * 3) / 4;
	const wchar_t* BREAK_CHARS = L" \n";
	std::wstring::size_type wordStartIndex = 0;
	std::wstring::size_type wordEndIndex = sOrigString.find_first_of( BREAK_CHARS );
	for(;;)
	{
		// Get the word
		std::wstring sCurWord;
		if( wordEndIndex != std::wstring::npos )
			sCurWord = sOrigString.substr( wordStartIndex, wordEndIndex - wordStartIndex );
		else if( sOrigString.length() - wordStartIndex > 0 )
			sCurWord = sOrigString.substr( wordStartIndex, sOrigString.length() - wordStartIndex );
		else
			break;

		// Get the width of the word
		int32 wordWidth = CalcStringWidth( sCurWord.c_str() );

		// Get the width of the new line
		int32 newLineWidth = curLineWidth + wordWidth;
		if( sRetString.length() > 0 && curLineWidth > 0 )
			newLineWidth += SPACE_WIDTH;

		// If this word is too long for the line
		if( newLineWidth > boundBox.size.x )
		{
			// Update the string
			sRetString += CHAR_NEWLINE;
			curLineWidth = wordWidth;
			sRetString += sCurWord;

			// Update the height
			curTextHeight += m_CharHeight + (m_CharHeight / 2);

			// If the text extends past the bottom then there is no point to continue to add to
			// the string
			if( curTextHeight >= boundBox.size.y )
				break;
		}
		else
		{
			// If this is not the first word added then add a space before adding this word
			if( sRetString.length() > 0 && curLineWidth > 0 )
			{
				sRetString += CHAR_SPACE;
				curLineWidth += SPACE_WIDTH;
			}

			// Add the word width and word
			curLineWidth += wordWidth;
			sRetString += sCurWord;
		}

		// If there is no text left to traverse
		if( wordEndIndex == std::wstring::npos )
			break;

		// If there is a new line at the end of this word
		if( sOrigString[ wordEndIndex ] == CHAR_NEWLINE )
		{
			sRetString += CHAR_NEWLINE;
			curLineWidth = 0;
		}

		// Find the next word
		wordStartIndex = sOrigString.find_first_not_of( BREAK_CHARS, wordEndIndex + 1 );
		if( wordStartIndex == std::wstring::npos )
			break;
		wordEndIndex = sOrigString.find_first_of( BREAK_CHARS, wordStartIndex );
	}

	return sRetString;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  TCFontImpl::GetWrapStringHeight()  Public
///
///	\param szText The word-wrapped string to measure
///	\returns The height of the string in pixels.
///
///	Get a word wrap string's height.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int32 TCFontImpl::GetWrapStringHeight( const wchar_t* szText ) const
{
	// Get the string length
	size_t strLen = wcslen( szText );
	if( strLen < 1 )
		return 0;

	int32 retHeight = m_CharHeight;

	// Find new-line characters
	const int32 NEWLINE_HEIGHT = m_CharHeight + (m_CharHeight / 2);
	for( size_t charIndex = 0; charIndex < strLen; ++charIndex )
	{
		// If this is a new-line character (And it is not the last character) then add the height
		if( szText[ charIndex ] == CHAR_NEWLINE && charIndex != (strLen - 1) )
			retHeight += NEWLINE_HEIGHT;
	}

	return retHeight;
}