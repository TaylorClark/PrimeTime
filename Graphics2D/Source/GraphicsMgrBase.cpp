#include "../GraphicsMgr.h"
#include "../TCImage.h"
#include "../TCFont.h"
#include "../PrivateInclude/RefSpriteImpl.h"
#include "Math/Point2i.h"
#include "../PrivateInclude/TCFontImpl.h"
#include "../CachedFontDraw.h"
#include "Base/MsgLogger.h"

const wchar_t SPACE_CHAR = L' ';
const wchar_t NEWLINE_CHAR = L'\n';


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::DrawFontText  Public
///	\param pFont The font to use for drawing
///	\param szText The NULL-terminated string to display
///	\param destPos The top-left position at which the text is to be displayed
///
///	Draw text.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrBase::DrawFontText( const TCFont* pFont, const wchar_t* szText, const Point2i& destPos, uint32 colorTint )
{
	// Verify the pointers
	if( !pFont || !szText )
		return;

	// Get the string length
	size_t strLen = wcslen( szText );
	if( strLen < 1 )
		return;

	// Go through each character
	TCFontImpl* pFontImpl = (TCFontImpl*)pFont;
	Point2i curPos = destPos;
	for( size_t charIndex = 0; charIndex < strLen; ++charIndex )
	{
		// Get the character
		wchar_t curChar = szText[ charIndex ];

		// If this is a space
		if( curChar == SPACE_CHAR )
		{
			const int32 SPACE_WIDTH = (pFont->GetCharHeight() * 3) / 4;
			curPos.x += SPACE_WIDTH;
			continue;
		}
		// Else if this is a new line character
		else if( curChar == NEWLINE_CHAR )
		{
			// Step down a line
			const int32 NEWLINE_HEIGHT = pFont->GetCharHeight() + (pFont->GetCharHeight() / 2);
			curPos.y += NEWLINE_HEIGHT;
			curPos.x = destPos.x;
			continue;
		}

		TCFontImpl::GlyphMap::const_iterator iterChar = pFontImpl->m_GlyphMap.find( curChar );
		if( iterChar == pFontImpl->m_GlyphMap.end() )
			continue;

		// Get the image rectangle
		const Box2i& charImgRect = iterChar->second;

		// Draw the character
		DrawImage( curPos, (TCImage*)pFontImpl->m_Image.GetObj(), charImgRect, colorTint );

		// Step to the next characters
		curPos.x += charImgRect.size.x + pFontImpl->GetCharSpacing();
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::DrawFontTextClipped  Public
///	\param pFont The font to use for drawing
///	\param szText The NULL-terminated string to display
///	\param destRect The top-left position for the text and the clipping dimensions
///
///	Draw text clipped to a rectangle.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrBase::DrawFontTextClipped( const TCFont* pFont, const wchar_t* szText, const Box2i& destRect )
{
	DrawFontTextClipped( pFont, szText, destRect.pos, destRect );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::DrawFontTextClipped  Public
///	\param pFont The font to use for drawing
///	\param szText The NULL-terminated string to display
///	\param destPos The position to draw at
///	\param clipRect The top-left position for the clipping dimensions
///
///	Draw text clipped to a rectangle.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrBase::DrawFontTextClipped( const TCFont* pFont, const wchar_t* szText, const Point2i& destPos, const Box2i& clipRect )
{
	// Verify the pointers
	TCFontImpl* pFontImpl = (TCFontImpl*)pFont;
	if( !pFontImpl || !szText || clipRect.size.x < 1 || clipRect.size.y < 1 || !pFontImpl->m_Image.GetObj() )
		return;
	const TCImage* pFontImage = pFontImpl->m_Image.GetObj();

	// Get the string length
	size_t strLen = wcslen( szText );
	if( strLen < 1 )
		return;

	// If the destination position is below the clip box then bail
	if( destPos.y > clipRect.Bottom() )
		return;

	const int32 bottom = clipRect.pos.y + clipRect.size.y;
	const int32 right = clipRect.pos.x + clipRect.size.x;
	const int32 NEWLINE_HEIGHT = pFontImpl->GetCharHeight() + (pFontImpl->GetCharHeight() / 2);
	const int32 SPACE_WIDTH = (pFontImpl->GetCharHeight() * 3) / 4;
	
	// Clip the height
	int32 modCharHeight = pFontImpl->GetCharHeight();
	int32 srcRectYOffset = 0;
	if( destPos.y < clipRect.pos.y )
	{
		srcRectYOffset = clipRect.pos.y - destPos.y;
		modCharHeight -= srcRectYOffset;
	}
	if( destPos.y + modCharHeight > bottom )
		modCharHeight -= (destPos.y + modCharHeight) - bottom;

	// Go through each character
	Point2i curPos = clipRect.pos;
	bool lookingForNewLine = false;
	Box2i fillRect( curPos, Vector2i( 0, pFontImpl->GetCharHeight() ) );
	for( size_t charIndex = 0; charIndex < strLen; ++charIndex )
	{
		// Get the character
		wchar_t curChar = szText[ charIndex ];

		// If we have exceeded the right boundry then skip drawing until a new-line character
		// is found
		if( lookingForNewLine )
		{
			// If this character is not a new-line character then keep searching
			if( curChar != NEWLINE_CHAR )
				continue;

			// We found a new line so stop looking
			lookingForNewLine = false;
		}

		// If this is a space
		if( curChar == SPACE_CHAR )
		{
			curPos.x += SPACE_WIDTH;
			continue;
		}
		// Else if this is a new line character
		else if( curChar == NEWLINE_CHAR )
		{
			// Color the text
			//m_pDDrawObj->FillRectDestKey( fillRect, 0x00FF0000 );

			// Step down a line
			curPos.y += NEWLINE_HEIGHT;
			curPos.x = clipRect.pos.x;
			fillRect.pos.y = curPos.y;
			fillRect.size.x = 0;

			// If we are off the bottom then we are done
			if( curPos.y >= bottom )
				break;

			// Clip the height in case this line is off the bottom
			modCharHeight = pFontImpl->GetCharHeight();
			if( curPos.y + modCharHeight > bottom )
				modCharHeight -= (curPos.y + modCharHeight) - bottom;
			continue;
		}

		// Get the character glyph
		TCFontImpl::GlyphMap::const_iterator iterChar = pFontImpl->m_GlyphMap.find( curChar );
		if( iterChar == pFontImpl->m_GlyphMap.end() )
			continue;

		// Get the image rectangle
		Box2i charImgRect = iterChar->second;
		charImgRect.size.y = modCharHeight;
		charImgRect.pos.y += srcRectYOffset;

		// Clip the character
		if( curPos.x + charImgRect.size.x > right )
			charImgRect.size.x -= (curPos.x + charImgRect.size.x) - right;

		// Draw the character
		DrawImage( curPos, pFontImage, charImgRect );

		// Step to the next characters
		curPos.x += charImgRect.size.x + pFontImpl->GetCharSpacing();
		fillRect.size.x = fillRect.pos.x - curPos.x;
		if( curPos.x > right )
		{
			lookingForNewLine = true;
			continue;
		}
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::CacheFontText  Public
///	\param pFont The font to use for drawing
///	\param szText The NULL-terminated string to display
///	\param destRect The top-left position for the text and the clipping dimensions
///	\param fontScale The amount to scale the text, defaults to 1.0 (No scale)
///	\returns A data structure containing optimized data for displaying strings
///
///	Cache font text that is to be drawn.
///////////////////////////////////////////////////////////////////////////////////////////////////
CachedFontDraw GraphicsMgrBase::CacheFontText( TCFont* pFont, const wchar_t* szText, const Box2i& destRect, float32 fontScale )
{
	CachedFontDraw retVal;

	//TODO Scaled fonts look bad due to smooth blending and alpha keying. Remove this and the code in Draw to enable
	fontScale = 1.0f;

	// Verify the pointers
	TCFontImpl* pFontImpl = (TCFontImpl*)pFont;
	if( !pFontImpl || !szText || destRect.size.x < 1 || destRect.size.y < 1 )
		return retVal;
	retVal.m_Font = pFont;

	// Get the string length
	size_t strLen = wcslen( szText );
	if( strLen < 1 )
		return retVal;

	const int32 bottom = destRect.pos.y + destRect.size.y;
	const int32 right = destRect.pos.x + destRect.size.x;
	const int32 NEWLINE_HEIGHT = pFontImpl->GetCharHeight() + (pFontImpl->GetCharHeight() / 2);
	const int32 SPACE_WIDTH = (pFontImpl->GetCharHeight() * 3) / 4;
	
	// Clip the height to fit in the destination rectangle
	int32 modCharHeight = (int32)(pFontImpl->GetCharHeight() * 1.0f);
	if( destRect.pos.y + modCharHeight > bottom )
		modCharHeight -= (destRect.pos.y + modCharHeight) - bottom;
	retVal.m_Size.y += modCharHeight;

	// Resize with enough room to compensate for new line characters
	retVal.Chars.resize( strLen * 2 );

	// Go through each character
	Point2i curPos = destRect.pos;
	bool lookingForNewLine = false;
	for( size_t charIndex = 0; charIndex < strLen; ++charIndex )
	{
		// Get the character
		wchar_t curChar = szText[ charIndex ];

		// If we have exceeded the right boundry then skip drawing until a new-line character
		// is found
		if( lookingForNewLine )
		{
			// If this character is not a new-line character then keep searching
			if( curChar != NEWLINE_CHAR )
				continue;

			// We found a new line so stop looking
			lookingForNewLine = false;
		}

		// If this is a space
		if( curChar == SPACE_CHAR )
		{
			curPos.x += SPACE_WIDTH;
			continue;
		}
		// Else if this is a new line character
		else if( curChar == NEWLINE_CHAR )
		{
			// Step down a line
			curPos.y += NEWLINE_HEIGHT;
			curPos.x = destRect.pos.x;
			
			// If we are off the bottom then we are done
			if( curPos.y >= bottom )
				break;

			// Clip the height in case this line is off the bottom
			modCharHeight = (int32)(pFontImpl->GetCharHeight() * 1.0f);
			if( curPos.y + modCharHeight > bottom )
				modCharHeight -= (curPos.y + modCharHeight) - bottom;
			
			// Update the cached height
			retVal.m_Size.y = (curPos.y + modCharHeight) - destRect.pos.y;

			continue;
		}

		// Get the character glyph
		TCFontImpl::GlyphMap::const_iterator iterChar = pFontImpl->m_GlyphMap.find( curChar );
		if( iterChar == pFontImpl->m_GlyphMap.end() )
			continue;

		// Get the image rectangle
		Box2i srcImgRect = iterChar->second;
		srcImgRect.size.y = modCharHeight;
		//TODO Known bug, scale text may not get clipped vertically properly
		Vector2i destCharSize = iterChar->second.size;
		destCharSize.x = (int32)(destCharSize.x * fontScale);
		destCharSize.y = (int32)(modCharHeight * fontScale);

		// Clip the character
		if( curPos.x + destCharSize.x > right )
		{
			int32 amountOver = (curPos.x + destCharSize.x) - right;
			destCharSize.x -= amountOver;

			srcImgRect.size.x -= (int32)(amountOver / fontScale);
		}

		// Store the character
		CachedFontDraw::Char newChar;
		newChar.screenPos = curPos;
		newChar.screenRect.Set( curPos.x, curPos.y, destCharSize.x, destCharSize.y );
		newChar.srcRect = srcImgRect;
		retVal.Chars[charIndex] = newChar;

		// Update the string width
		if( newChar.screenPos.x + newChar.srcRect.size.x > retVal.m_Size.x )
			retVal.m_Size.x = newChar.screenPos.x + newChar.srcRect.size.x;

		// Step to the next characters
		curPos.x += destCharSize.x + pFontImpl->GetCharSpacing();
		if( curPos.x > right )
		{
			lookingForNewLine = true;
			continue;
		}
	}

	// Convert the right most value to a width value
	retVal.m_Size.x -= destRect.pos.x;

	return retVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::DrawRect  Public
///	\param destPos The position of the top left of the sprite in screen coordinates
///	\param pSprite The sprite to draw
///
///	Draw a sprite.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrBase::DrawSprite( const Point2i& destPos, const RefSprite* pSprite )
{
	RefSpriteImpl* pSpr = (RefSpriteImpl*)pSprite;
	if( !pSpr || !pSpr->m_SrcImage.GetObj() )
		return;

	DrawImage( destPos, pSprite->GetImage(), pSprite->GetSrcRect() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::DrawSprite  Public
///	\param destPos The position of the top left of the sprite in screen coordinates
///	\param pSprite The sprite to draw
///	\param drawFx The draw effects to apply to the sprite from the EDrawEffectFlags enum
///	\param fAngle The angle to rotate the sprite when draw
///
///	Draw a sprite with effects.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrBase::DrawSprite( const Point2i& destPos, const RefSprite* pSprite, int32 drawFx )
{
	RefSpriteImpl* pSpr = (RefSpriteImpl*)pSprite;
	if( !pSpr || !pSpr->m_SrcImage.GetObj() )
		return;

	// Draw the sprite
	Box2i destRect( destPos, pSprite->GetSrcRect().size );
	DrawImageEx( destRect, pSpr->m_SrcImage.GetObj(), pSpr->GetSrcRect(), drawFx );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::DrawSpriteScaled  Public
///	\param destRect The rectangle to which the sprite is drawn
///	\param pSprite The sprite to draw
///
///	Draw a sprite.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrBase::DrawSpriteScaled( Box2i destRect, const RefSprite* const pSprite, int32 fxFlags )
{
	RefSpriteImpl* pSpr = (RefSpriteImpl*)pSprite;
	if( !pSpr || !pSprite->GetImage() )
		return;

	DrawImageEx( destRect, pSprite->GetImage(), pSpr->GetSrcRect(), fxFlags );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::DrawSpriteUnscaled  Public
///	\param destRect The rectangle to which to draw the sprite
///	\param pSprite The sprite to draw
///
///	Draw a sprite.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GraphicsMgrBase::DrawSpriteUnscaled( Box2i destRect, const RefSprite* const pSprite )
{
	RefSpriteImpl* pSpr = (RefSpriteImpl*)pSprite;
	if( !pSpr )
		return;

	// If the destination dimensions are larger than the source then scale it down
	Box2i srcRect = pSpr->GetSrcRect();
	if( destRect.size.x > srcRect.size.x )
		destRect.size.x = srcRect.size.x;
	if( destRect.size.y > srcRect.size.y )
		destRect.size.y = srcRect.size.y;

	// If the destination is smaller
	if( destRect.size.x < srcRect.size.x )
		srcRect.size.x = destRect.size.x;
	if( destRect.size.y < srcRect.size.y )
		srcRect.size.y = destRect.size.y;

	DrawImage( destRect.pos, pSpr->GetImage(), srcRect );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrBase::LoadFontFromMemory()  Public
///	\param resID The resource ID of this font
///	\param dataBlock The data for the font
///	\returns A pointer to the loaded font, NULL on failure
///
///	Load a font from memory.
///////////////////////////////////////////////////////////////////////////////////////////////////
TCFont* GraphicsMgrBase::LoadFontFromMemory( uint32 resID, DataBlock* pFontDataBlock )
{
	// Ensure there enough data for the basic data
	unsigned int dataNeeded = sizeof(int32) // Version
								+ sizeof(int32) //pRetFont->m_CharHeight
								+ sizeof(int32) //pRetFont->m_CharSpacing
								+ sizeof(ResourceID) // Source image
								+ sizeof(int32); // Number of characters
	if( pFontDataBlock->GetSize() < dataNeeded )
		return NULL;
	
	// Read in the font data version
	int fontDataVer = pFontDataBlock->ReadInt32();
	if( fontDataVer != 1 )
	{
		MsgLogger::Get().Output( MsgLogger::MI_Error, L"Unknown font file version." );
		return NULL;
	}

	TCFontImpl* pRetFont = new TCFontImpl( resID );

	// Read in the font data
	pRetFont->m_CharHeight = pFontDataBlock->ReadInt32();
	pRetFont->m_CharSpacing = pFontDataBlock->ReadInt32();
	pRetFont->m_ImageResID = pFontDataBlock->ReadUint32();
    int numChars = pFontDataBlock->ReadInt32();

	// Get the size of one character entry
	const uint32 charEntrySize = sizeof(uint16) + (sizeof(int) * 4);

	// Ensure there is enough memory left for the characters
	if( (charEntrySize * numChars) > pFontDataBlock->GetRemainingBytes() )
	{
		delete pRetFont;
		MsgLogger::Get().Output( MsgLogger::MI_Error, L"Font data is not large enough to hold the number characters specified by the font." );
		return NULL;
	}

	// Go through all of the characters
	for( int32 curCharIndex = 0; curCharIndex < numChars; ++curCharIndex )
	{
		// Read in the character
		wchar_t curChar = pFontDataBlock->ReadChar();

		// Read in the image rectangle
		Box2i imgRect;
		imgRect.pos.x = pFontDataBlock->ReadInt32();
		imgRect.pos.y = pFontDataBlock->ReadInt32();
		imgRect.size.x = pFontDataBlock->ReadInt32();
		imgRect.size.y = pFontDataBlock->ReadInt32();

		// Add the character to the map
		pRetFont->m_GlyphMap.insert( std::make_pair( curChar, imgRect ) );
	}

	// Return the font
	return pRetFont;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GraphicsMgrDX::LoadSpriteFromMemory()  Public
///	\param resID The resource ID of the sprite to load
///	\param dataBlock The block of memory from which to load
///	\returns A pointer to loaded sprite, NULL on failure.
///
///	Initialize a sprite object from memory.
///////////////////////////////////////////////////////////////////////////////////////////////////
RefSprite* GraphicsMgrBase::LoadSpriteFromMemory( uint32 resID, DataBlock* pSpriteDataBlock )
{
	// Ensure there is enough data for the first block of data
	uint32 dataNeeded = sizeof(int32) + sizeof(uint32) + sizeof(int32);
	if( pSpriteDataBlock->GetRemainingBytes() < dataNeeded )
	{
		return NULL;
	}

	// Read in the sprite data version
	int32 spriteDataVer = pSpriteDataBlock->ReadInt32();
	if( spriteDataVer != 1 )
	{
		MsgLogger::Get().Output( MsgLogger::MI_Error, L"Unknown sprite file version." );
		return NULL;
	}

	// Read in the source image ID
	uint32 srcImgResID = pSpriteDataBlock->ReadUint32();

	// Load the image
	//TCImageHndl srcImage = ResourceMgr::Get().GetTCImage( srcImgResID );

	// Read in the number of frames
	int32 numFrames = pSpriteDataBlock->ReadInt32();
	if( numFrames < 1 )
		return NULL;

	// If there is only 1 frame
	RefSpriteImpl* pRetSprite = new RefSpriteImpl( resID );
	if( !pRetSprite )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to create new sprite" );
		return NULL;
	}
	
	// Store the image ID
	pRetSprite->m_ImageResID = srcImgResID;

	// If there is only one frame
	if( numFrames == 1 )
	{
		// Read in the source rectangle
		int32 x = pSpriteDataBlock->ReadInt32();
		int32 y = pSpriteDataBlock->ReadInt32();
		int32 w = pSpriteDataBlock->ReadInt32();
		int32 h = pSpriteDataBlock->ReadInt32();

		// Set the source rectangle
		RefSprite::Frame newFrame;
		newFrame.srcCoords.Set(x, y, w, h );
		pRetSprite->m_AnimFrames[0].srcCoords.Set( x, y, w, h );
		pRetSprite->m_AnimFrames[0].frameDelay = 1000;
	}
	// Else it is a multi-frame animation
	else
	{
		// Read in the animation type
		pRetSprite->SetPlayType( (RefSprite::EAnimType)pSpriteDataBlock->ReadInt32() );

		// Read in the source rectangles
		pRetSprite->m_AnimFrames.resize( numFrames );
		for( int32 frameIndex = 0; frameIndex < numFrames; ++frameIndex )
		{
			RefSprite::Frame newFrame;

			// Read in the dimensions
			newFrame.srcCoords.pos.x = pSpriteDataBlock->ReadInt32();
			newFrame.srcCoords.pos.y = pSpriteDataBlock->ReadInt32();
			newFrame.srcCoords.size.x = pSpriteDataBlock->ReadInt32();
			newFrame.srcCoords.size.y = pSpriteDataBlock->ReadInt32();

			// Read in the frame delay and convert it from milliseconds to seconds
			newFrame.frameDelay = (float32)pSpriteDataBlock->ReadInt32() / 1000.0f;

			// Store the frame
			pRetSprite->m_AnimFrames[ frameIndex ] = newFrame;
		}

	}

	// Return the sprite
	return pRetSprite;
}


/// A helper method to create a single-frame sprite for an image
RefSprite* GraphicsMgrBase::CreateSpriteForImage( TCImage* pImage )
{
	RefSpriteImpl* pRetSprite = new	RefSpriteImpl(0);
	pRetSprite->CreateForImage( pImage );

	return pRetSprite;
}