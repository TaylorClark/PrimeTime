//=================================================================================================
/*!
	\file TCFont.h
	2D Graphics Engine
	Font Header
	\author Taylor Clark
	\date January 25, 2006

	This header contains the definition for the font class.
*/
//=================================================================================================

#pragma once
#ifndef __TCFontImpl_h
#define __TCFontImpl_h

#include "../TCFont.h"

class GraphicsMgrDX;
class GraphicsMgrSFML;
class GraphicsMgrBase;
class ResourceMgr;


//-------------------------------------------------------------------------------------------------
/*!
	\class TCFontImpl
	\brief The drawable font class.

	The TCFont object stores the data needed to render a font to the screen.
*/
//-------------------------------------------------------------------------------------------------
class TCFontImpl : public TCFont
{
	friend class GraphicsMgrDX;
	friend class GraphicsMgrSFML;
	friend class GraphicsMgrBase;
	friend class ResourceMgr;

	typedef std::map< wchar_t, Box2i > GlyphMap;

public:

	/// The map of characters to image rectangles
	GlyphMap m_GlyphMap;

	/// The height of each character in pixels
	int32 m_CharHeight;

	/// The spacing between characters
	int32 m_CharSpacing;

	/// The image to use
	TCImageHndl m_Image;

	/// The image resource ID, needed since the resource manager loads the image after the font
	///	is loaded
	ResourceID m_ImageResID;

	/// The default constructor private so the user must load fonts from files
	TCFontImpl( ResourceID resID ) : TCFont( resID ),
				m_CharHeight(0),
				m_CharSpacing(0)
	{
	}

public:

	/// The destructor
	virtual ~TCFontImpl() {}

	//static TCFont* FromFile( const wchar_t* szFilePath );
	static TCFont* FromMemory( ResourceID resID, DataBlock* pDataBlock );

	/// Get the resource type
	virtual EResourceType GetResType() const { return RT_Font; }

	/// Get the character height for the font
	virtual int32 GetCharHeight() const { return m_CharHeight; }

	/// Calculate the width of a string in pixels
	virtual int32 CalcStringWidth( const wchar_t* szText ) const;

	/// Get the intra-character spacing
	virtual int32 GetCharSpacing() const { return m_CharSpacing; }

	/// Release the resources, if any, used by this resource
	virtual void ReleaseSubResources()
	{
		m_Image.ReleaseHandle();
	}

	/// Make a string word wrap within a bounding box
	virtual std::wstring GetWordWrapString( const wchar_t* szStr, const Box2i& boundBox ) const;

	/// Get a word wrap string's height
	virtual int32 GetWrapStringHeight( const wchar_t* szText ) const;

	/// Get the resource ID of the image used by this font
	virtual uint32 GetImgResID() const{ return m_ImageResID; }

	/// Set the image used by this font
	virtual void SetImage( TCImageHndl img ){ m_Image = img; }
};

#endif // __TCFontImpl_h