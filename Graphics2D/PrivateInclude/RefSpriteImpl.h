//=================================================================================================
/*!
	\file RefSpriteImpl.h
	2D Game Engine
	Reference Sprite Header
	\author Taylor Clark
	\date February 2, 2006

	This file contains the definition for the reference sprite class.  Sprites reference this
	class for the graphical data.
*/
//=================================================================================================

#pragma once
#ifndef __RefSpriteImpl_h
#define __RefSpriteImpl_h

#include "../RefSprite.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class RefSpriteImpl
	\brief The class representing a sprite.

	This class defines the manager for all graphics related functionality.  It uses the singleton
	pattern so that there is one and only instance of the class.
*/
//-------------------------------------------------------------------------------------------------
class RefSpriteImpl : public RefSprite
{
public:

	/// The animation frame source rectangles
	std::vector<Frame> m_AnimFrames;

	/// The play type
	EAnimType m_PlayType;

	/// The image resource ID, needed since the resource manager loads the image after the sprite
	///	is loaded
	ResourceID m_ImageResID;

	/// The image to use for drawing
	TCImageHndl m_SrcImage;

	/// The default constructor
	RefSpriteImpl( ResourceID resID ) : RefSprite( resID ),
									m_PlayType( AT_NoLoop )
	{
		m_AnimFrames.resize( 1 );
	}

	virtual ~RefSpriteImpl()
	{
		m_AnimFrames.clear();
	}

	/// A helper method to create an animated sprite object for an image
	virtual void CreateForImage( TCImage* pImage )
	{
		// Set the one frame
		Frame oneFrame;
		oneFrame.frameDelay = 0;
		oneFrame.srcCoords = Box2i(0, 0, pImage->GetDims().x, pImage->GetDims().y );
		m_AnimFrames.clear();
		m_AnimFrames.push_back( oneFrame );

		m_PlayType = AT_NoLoop;

		m_ImageResID = 0;

		m_SrcImage = TCImageHndl(pImage);
	}

	/// Get the source image coordinates
	virtual const Box2i& GetSrcRect( uint32 frameIndex = 0 ) const { return m_AnimFrames[frameIndex].srcCoords; }

	/// Get the resource type
	virtual EResourceType GetResType() const { return RT_Sprite; }

	/// Get the image
	virtual const TCImage* GetImage() const { return m_SrcImage.GetObj(); }

	/// Get the image
	virtual TCImage* GetImageNonConst(){ return m_SrcImage.GetObj(); }

	/// Get the frames
	virtual const std::vector<Frame>& GetFrames() const { return m_AnimFrames; }

	/// Get the number of frames
	virtual uint32 GetNumFrames() const { return (uint32)m_AnimFrames.size(); }

	/// Set the play type
	void SetPlayType( EAnimType playType )
	{
		m_PlayType = playType;
	}

	/// Get the play type
	virtual EAnimType GetPlayType() const { return m_PlayType; }

	/// Release the resources, if any, used by this resource
	virtual void ReleaseSubResources()
	{
		m_SrcImage.ReleaseHandle();
	}

	/// Get the resource ID of the image used by this font
	virtual uint32 GetImgResID() const { return m_ImageResID; }

	/// Set the image used by this font
	virtual void SetImage( TCImageHndl img ) { m_SrcImage = img; }
};

/// A reference counted reference sprite
typedef RefCountHandle<RefSprite> RefSpriteHndl;

#endif // __RefSpriteImpl_h