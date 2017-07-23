//=================================================================================================
/*!
	\file TCImage.h
	2D Game Engine
	Graphics Manager Header
	\author Taylor Clark
	\date February 2, 2006

	This file contains the definition for the image base class.
*/
//=================================================================================================

#pragma once
#ifndef __TCImage_h
#define __TCImage_h

#include "Resource/Resource.h"

class Vector2i;


//-------------------------------------------------------------------------------------------------
/*!
	\class TCImage
	\brief The class representing an image.

	This class defines an object that stores an image that can be used for drawing.
*/
//-------------------------------------------------------------------------------------------------
class TCImage : public Resource
{
public:

	/// The default constructor is private so we can't instantiate this class
	TCImage( ResourceID resID) : Resource( resID )
	{}

	/// The destructor
	virtual ~TCImage() {}

	/// Get the resource type
	virtual EResourceType GetResType() const { return RT_Image; }

	/// Get the dimensions of the image
	virtual Vector2i GetDims() const = 0;

	/// Get the image data
	virtual void* GetImageData() const = 0;

	/// Set the image data
	virtual void SetImageData( void* pImageData ) = 0;

	/// Create an object
	static TCImage* Create( ResourceID resID );
};

/// A reference counted reference sprite
typedef RefCountHandle<TCImage> TCImageHndl;

#endif // __TCImage_h