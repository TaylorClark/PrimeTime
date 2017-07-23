//=================================================================================================
/*!
	\file AnimSprite.cpp
	2D Graphics Engine
	Animated Sprite Source
	\author Taylor Clark
	\date July 3, 2006

	This source file contains the implementation for the animated sprite class.
*/
//=================================================================================================

#include "../AnimSprite.h"
#include "Base/TCAssert.h"

/// Update the animation
void AnimSprite::Update( float frameTime )
{
	// If we aren't playing then bail
	RefSprite* pAnimSprite = (RefSprite*)m_RefSprite.GetObj();
	if( !m_IsPlaying || !pAnimSprite )
		return;

	// Update the frame time
	m_FrameTime += frameTime;

	// Advance as many frames as needed
	const std::vector< RefSprite::Frame >& animFrames = pAnimSprite->GetFrames();
	while( m_FrameTime > animFrames[ m_CurFrameIndex ].frameDelay )
	{
		m_FrameTime -= animFrames[ m_CurFrameIndex ].frameDelay;

		// If we are playing the animation forward
		if( m_IsPlayingForward )
		{
			// If this is the last frame
			if( m_CurFrameIndex + 1 == animFrames.size() )
			{
				// If we are looping
				if( pAnimSprite->GetPlayType() == RefSprite::AT_Loop )
					m_CurFrameIndex = 0;
				// Else if we are ping ponging
				else if( pAnimSprite->GetPlayType() == RefSprite::AT_PingPong )
				{
					m_CurFrameIndex--;
					m_IsPlayingForward = false;
				}
				// Else we just stop
				else
					m_IsPlaying = false;
			}
			// Else just advance a frame
			else
				m_CurFrameIndex++;
		}
		// Else we are playing backwards for a ping-pong animation
		else
		{
			// If we are on the first frame then reverse playing
			if( m_CurFrameIndex == 0 )
			{
				m_IsPlayingForward = true;
				m_CurFrameIndex++;
			}
			// Else just advance backwards a frame
			else
				m_CurFrameIndex--;
		}
	}
	// End of update
}

/// Start playing the animation
void AnimSprite::Play()
{
	// If we don't have a valid animation
	RefSprite* pAnimSprite = (RefSprite*)m_RefSprite.GetObj();
	if( !pAnimSprite )
		return;
	const std::vector< RefSprite::Frame >& animFrames = pAnimSprite->GetFrames();
	if( animFrames.size() < 2 )
		return;

	// Clear the data
	m_CurFrameIndex = 0;
	m_FrameTime = 0.0f;
	m_IsPlayingForward = true;
	m_IsPlaying = true;
}


/// Get the source coordinates
Box2i AnimSprite::GetSrcCoords() const
{
	RefSprite* pAnimSprite = (RefSprite*)m_RefSprite.GetObj();
	if( !pAnimSprite )
		return Box2i();
	const std::vector< RefSprite::Frame >& animFrames = pAnimSprite->GetFrames();

	return animFrames[ m_CurFrameIndex ].srcCoords;
}


const TCImage* AnimSprite::GetImage() const
{
	RefSprite* pAnimSprite = (RefSprite*)m_RefSprite.GetObj();
	if( !pAnimSprite )
		return NULL;
	return pAnimSprite->GetImage();
}