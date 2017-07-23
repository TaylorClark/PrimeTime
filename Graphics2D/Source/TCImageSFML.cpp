//=================================================================================================
/*!
	\file TCImageSFML.cpp
	2D Graphics Engine
	SFML Image Source
	\author Taylor Clark
	\date March 10, 2010

	This source file contains the implementation for the SFML image class.
*/
//=================================================================================================

#include "../TCImage.h"
#include "Base/Types.h"
#include "Math/Vector2i.h"
#include "Base/TCAssert.h"
#include <SFML/Graphics/Image.hpp>


//-------------------------------------------------------------------------------------------------
/*!
	\class TCImageSFML
	\brief The class representing an image.

	This class defines an object that stores an image that can be used for drawing.
*/
//-------------------------------------------------------------------------------------------------
class TCImageSFML : public TCImage
{
public:

	/// The DirectDraw surface object
	sf::Image* _pSFMLImage;

	/// The default constructor is private so we can't instantiate this class
	TCImageSFML( ResourceID resID ) : TCImage( resID ),
									_pSFMLImage( NULL )
	{}

	/// The destructor
	virtual ~TCImageSFML();

	/// Get the dimensions of the image
	virtual Vector2i GetDims() const;

	/// Get the image data
	virtual void* GetImageData() const { return _pSFMLImage; }

	/// Set the image data
	virtual void SetImageData( void* pImageData )
	{
		_pSFMLImage = (sf::Image*)pImageData;
	}
};


TCImage* TCImage::Create( ResourceID resID )
{
	return new TCImageSFML( resID );
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//	TCImageSFML::~TCImageSFML()  Public
///
///	The destructor.
///////////////////////////////////////////////////////////////////////////////////////////////////
TCImageSFML::~TCImageSFML()
{
	if( _pSFMLImage )
		delete _pSFMLImage;
	_pSFMLImage = NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//	TCImageSFML::GetDims()  Public
///	\returns The dimensions of the image
///
///	Get the width and height of the image.
///////////////////////////////////////////////////////////////////////////////////////////////////
Vector2i TCImageSFML::GetDims() const
{
	// If we don't have a surface then bail
	TCASSERTX( _pSFMLImage != NULL, L"Trying to access surface dimensions from uncreated surface," );
	if( !_pSFMLImage )
	{
		static Vector2i nullVector;
		return nullVector;
	}

	// Return the dimensions of the DirectDraw surface
	return Vector2i( _pSFMLImage->getSize().x, _pSFMLImage->getSize().y );
}
