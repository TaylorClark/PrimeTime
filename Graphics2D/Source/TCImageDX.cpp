//=================================================================================================
/*!
	\file TCImageDX.cpp
	2D Graphics Engine
	DirectX Image Source
	\author Taylor Clark
	\date January 25, 2006

	This source file contains the implementation for the font class.
*/
//=================================================================================================

#include "../TCImage.h"
#include "Base/Types.h"
#include "Math/Vector2i.h"
#include "Base/TCAssert.h"
#include "../PrivateInclude/DrawInterface.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class TCImageDX
	\brief The class representing a DirectX image.

	This class defines an object that stores an image that can be used for drawing using DirectX.
*/
//-------------------------------------------------------------------------------------------------
class TCImageDX : public TCImage
{
public:

	/// The DirectDraw surface object
	DDrawSurface* m_pDDrawSurface;

	/// The default constructor is private so we can't instantiate this class
	TCImageDX( ResourceID resID) : TCImage( resID ),
									m_pDDrawSurface( NULL )
	{}

	/// The destructor
	virtual ~TCImageDX();

	/// Get the dimensions of the image
	virtual Vector2i GetDims() const;

	/// Get the image data
	virtual void* GetImageData() const { return m_pDDrawSurface; }

	/// Set the image data
	virtual void SetImageData( void* pImageData )
	{
		m_pDDrawSurface = (DDrawSurface*)pImageData;
	}
};


TCImage* TCImage::Create( ResourceID resID )
{
	return new TCImageDX( resID );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	TCImageDX::~TCImageImpl()  Public
///
///	The destructor.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
TCImageDX::~TCImageDX()
{
	if( m_pDDrawSurface )
		delete m_pDDrawSurface;
	m_pDDrawSurface = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	TCImageDX::GetDims()  Public
///	\returns The dimensions of the image
///
///	Get the width and height of the image.
///////////////////////////////////////////////////////////////////////////////////////////////////
Vector2i TCImageDX::GetDims() const
{
	// If we don't have a surface then bail
	TCASSERTX( m_pDDrawSurface != NULL, L"Trying to access surface dimensions from uncreated surface," );
	if( !m_pDDrawSurface )
	{
		static Vector2i nullVector;
		return nullVector;
	}

	// Return the dimensions of the DirectDraw surface
	return m_pDDrawSurface->m_Dims;
}