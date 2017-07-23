//=================================================================================================
/*!
	\file AnimSprite.h
	2D Game Engine
	Animated Sprite Header
	\author Taylor Clark
	\date July 2, 2006

	This file contains the definition for the animated sprite class.  This stores a reference
	animated sprite and the information needed to play correctly.
*/
//=================================================================================================

#pragma once
#ifndef __AnimSprite_h
#define __AnimSprite_h

#include "Graphics2D/RefSprite.h"
class GUICtrlSprite;


//-------------------------------------------------------------------------------------------------
/*!
	\class AnimSprite
	\brief The class representing an animated sprite.

	This class defines the manager for all graphics related functionality.  It uses the singleton
	pattern so that there is one and only instance of the class.
*/
//-------------------------------------------------------------------------------------------------
class AnimSprite
{
	friend class GUICtrlSprite;
private:

	/// The animated sprite resource
	RefSpriteHndl m_RefSprite;

	/// The currently displayed frame
	uint32 m_CurFrameIndex;

	/// The time that has elapsed in this frame
	float32 m_FrameTime;

	/// If the animation is playing forward (It could go backwards in a ping-pong sprite)
	bool m_IsPlayingForward;

	/// If the animation is playing
	bool m_IsPlaying;

public:

	/// The default constructor
	AnimSprite( RefSpriteHndl refAnimSprite = RefSpriteHndl(0) ) : m_RefSprite( refAnimSprite ),
												m_CurFrameIndex( 0 ),
												m_FrameTime( 0.0f ),
												m_IsPlayingForward( true ),
												m_IsPlaying( false )
	{}

	/// Update the animation
	virtual void Update( float frameTime );

	/// Start playing the animation
	virtual void Play();

	/// Get the source coordinates
	virtual Box2i GetSrcCoords() const;

	/// Get the image
	virtual const TCImage* GetImage() const;

	/// Determine if this animation is valid
	virtual bool IsValid() const { return m_RefSprite.GetObj() != NULL; }

	/// Get the sprite
	const RefSprite* GetSprite() const { return m_RefSprite.GetObj(); }

	/// Get the sprite
	RefSprite* GetSprite() { return m_RefSprite.GetObj(); }
};

#endif // __AnimSprite_h