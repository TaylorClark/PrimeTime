//=================================================================================================
/*!
	\file RefSprite.h
	2D Game Engine
	Reference Sprite Header
	\author Taylor Clark
	\date February 2, 2006

	This file contains the definition for the reference sprite class.  Sprites reference this
	class for the graphical data.
*/
//=================================================================================================

#pragma once
#ifndef __RefSprite_h
#define __RefSprite_h

#include "Math/Box2i.h"
#include "Resource/Resource.h"
#include "Base/DataBlock.h"
#include "TCImage.h"
#include <vector>

class GraphicsMgrDX;
class ResourceMgr;
class AnimSprite;


//-------------------------------------------------------------------------------------------------
/*!
	\class RefSprite
	\brief The class representing a sprite.

	This class defines the manager for all graphics related functionality.  It uses the singleton
	pattern so that there is one and only instance of the class.
*/
//-------------------------------------------------------------------------------------------------
class RefSprite : public Resource
{
	friend class GraphicsMgrDX;
	friend class ResourceMgr;
	friend class AnimSprite;

public:

	/// The animation frame structure
	struct Frame
	{
		Frame() : frameDelay( 0.0f )
		{}

		Box2i srcCoords;
		float32 frameDelay;
	};

	/// The different play types
	enum EAnimType
	{
		AT_NoLoop = 0,
		AT_Loop,
		AT_PingPong
	};

protected:

	/// The default constructor
	RefSprite( ResourceID resID ) : Resource( resID )
	{
	}

	virtual ~RefSprite() {}

public:

	/// A helper method to create an animated sprite object for an image
	virtual void CreateForImage( TCImage* pImage ) = 0;

	/// Get the source image coordinates
	virtual const Box2i& GetSrcRect( uint32 frameIndex = 0 ) const = 0;

	/// Get the resource type
	virtual EResourceType GetResType() const { return RT_Sprite; }

	/// Get the image, constant
	virtual const TCImage* GetImage() const = 0;

	/// Get the image
	virtual TCImage* GetImageNonConst() = 0;

	/// Get the frames
	virtual const std::vector<Frame>& GetFrames() const = 0;

	/// Get the number of frames
	virtual uint32 GetNumFrames() const = 0;

	/// Set the play type
	virtual void SetPlayType( EAnimType playType ) = 0;

	/// Get the play type
	virtual EAnimType GetPlayType() const = 0;

	/// Release the resources, if any, used by this resource
	virtual void ReleaseSubResources() = 0;

	/// Get the resource ID of the image used by this font
	virtual uint32 GetImgResID() const = 0;

	/// Set the image used by this sprite
	virtual void SetImage( TCImageHndl img ) = 0;
};

/// A reference counted reference sprite
typedef RefCountHandle<RefSprite> RefSpriteHndl;

#endif // __RefSprite_h