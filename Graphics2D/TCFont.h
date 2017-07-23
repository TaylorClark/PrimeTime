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
#ifndef __TCFont_h
#define __TCFont_h

#include <map>
#include "Math/Box2i.h"
#include "TCImage.h"
#include "Resource/Resource.h"
#include "Base/DataBlock.h"

class GraphicsMgrDX;
class ResourceMgr;


//-------------------------------------------------------------------------------------------------
/*!
	\class TCFont
	\brief The drawable font class.

	The TCFont object stores the data needed to render a font to the screen.
*/
//-------------------------------------------------------------------------------------------------
class TCFont : public Resource
{
	friend class GraphicsMgrDX;
	friend class ResourceMgr;

protected:

	/// The default constructor private so the user must load fonts from files
	TCFont( ResourceID resID ) : Resource( resID )
	{
	}

public:

	/// The destructor
	virtual ~TCFont() {}

	//static TCFont* FromFile( const wchar_t* szFilePath );
	static TCFont* FromMemory( ResourceID resID, DataBlock* pDataBlock );

	/// Get the resource type
	virtual EResourceType GetResType() const = 0;

	/// Get the character height for the font
	virtual int32 GetCharHeight() const = 0;

	/// Calculate the width of a string in pixels
	virtual int32 CalcStringWidth( const wchar_t* szText ) const = 0;

	/// Get the intra-character spacing
	virtual int32 GetCharSpacing() const = 0;

	/// Release the resources, if any, used by this resource
	virtual void ReleaseSubResources() = 0;

	/// Make a string word wrap within a bounding box
	virtual std::wstring GetWordWrapString( const wchar_t* szStr, const Box2i& boundBox ) const = 0;

	/// Get a word wrap string's height
	virtual int32 GetWrapStringHeight( const wchar_t* szText ) const = 0;

	/// Get the resource ID of the image used by this font
	virtual uint32 GetImgResID() const = 0;

	/// Set the image used by this font
	virtual void SetImage( TCImageHndl img ) = 0;
};

/// A reference counted reference sprite
typedef RefCountHandle<TCFont> TCFontHndl;

#endif // __TCFont_h