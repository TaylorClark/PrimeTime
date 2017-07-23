//=================================================================================================
/*!
	\file AudioMgr.cpp
	Audio Library
	Audio Manager Header
	\author Taylor Clark
	\date March 29, 2010

	This source file contains the implementation for the audio manager class.
*/
//=================================================================================================

#include "../AudioMgr.h"
#include "Base/MsgLogger.h"
#include "Resource/ResourceMgr.h"
#include <fstream>

#ifdef AUDIO_FMODEX
#include <fmod.hpp>
#endif

/// Update the audio manager
void AudioMgr::Update( float32 frameTime )
{
	// Go through channels that need to fade out
	FadeList::iterator prevIter = m_FadeList.end();
	for( FadeList::iterator iterSound = m_FadeList.begin(); iterSound != m_FadeList.end(); ++iterSound )
	{
		// If there is a previous iterator
		if( prevIter != m_FadeList.end() )
		{
			// If the sound has faded then remove the iterator
			if( prevIter->elapsedTime >= prevIter->timeToFade )
			{
				m_FadeList.erase( prevIter );
				prevIter = m_FadeList.end();
			}
		}

		// Update the time elapsed
		iterSound->elapsedTime += frameTime;

		// Get the new volume amount
		float32 volume = 0.0f;
		if( iterSound->isFadeIn )
			volume = iterSound->elapsedTime / iterSound->timeToFade;
		else
			volume = (iterSound->timeToFade - iterSound->elapsedTime) / iterSound->timeToFade;
		if( volume < 0.0f )
			volume = 0.0f;
		else if( iterSound->isFadeIn && volume > iterSound->fadeToVolume )
			volume = iterSound->fadeToVolume;
		else if( volume > 1.0f )
			volume = 1.0f;

		// Scale by the initial volume
		volume *= iterSound->initialVolumeScale;

		// Set the channel's volume
		if( iterSound->m_MusicStream.GetObj() )
			iterSound->m_MusicStream.GetObj()->SetVolume_Internal( volume );
		else
			iterSound->_sound.SetVolume_Internal( volume );

		// If the sound is fading out and it is now silent then stop it
		if( !iterSound->isFadeIn && volume <= 0.0f )
		{
			if( iterSound->m_MusicStream.GetObj() )
				iterSound->m_MusicStream.GetObj()->Stop();
			else
				iterSound->_sound.StopAndRelease();
		}
		// Else if the sound is fading in and it has reached or surpassed the end volume
		else if( iterSound->isFadeIn && volume >= iterSound->fadeToVolume )
			iterSound->elapsedTime = iterSound->timeToFade;

		// Store this iterator
		prevIter = iterSound;
	}

	// If there is a previous fade out iterator
	if( prevIter != m_FadeList.end() )
	{
		// If the sound has faded then remove the iterator
		if( prevIter->elapsedTime >= prevIter->timeToFade )
			m_FadeList.erase( prevIter );
	}

#ifdef SFML
	//UpdateSFML();
#endif

#ifdef AUDIO_FMODEX
    _pSystem->update();
#endif
}

void AudioMgr::FadeOutAndReleaseSound( SoundInstance& sound, float32 timeToFade )
{
	ScheduledFade newFadeOut;
	newFadeOut._sound = sound;
	newFadeOut.timeToFade = timeToFade;
	newFadeOut.isFadeIn = false;
	newFadeOut.initialVolumeScale = sound.GetVolume();

	// Add the new fade out
	m_FadeList.push_back( newFadeOut );

	sound._sourceSound.ReleaseHandle();
}

/// Fade out and stop a music stream
void AudioMgr::FadeOutMusic( MusicStream* pStream, float32 timeToFade )
{
	if( !pStream )
		return;

	// Initialize the fade information
	ScheduledFade newFadeOut;
	newFadeOut.timeToFade = timeToFade;
	newFadeOut.isFadeIn = false;
	newFadeOut.m_MusicStream = pStream;
	newFadeOut.initialVolumeScale = pStream->GetVolume();

	// Add the new fade out
	m_FadeList.push_back( newFadeOut );
}


/// Set the background music
void AudioMgr::SetBGMusic( MusicStream* pStream )
{
	// Fade out the current music
    if( m_CurBGM.GetObj() )
	{
		FadeOutMusic( m_CurBGM.GetObj(), 1.0f );
		m_CurBGM.ReleaseHandle();
	}

	// If a valid sample was passed in then fade it in
	if( pStream )
	{
		m_CurBGM = pStream;
		pStream->Play( true, 1.0f );
	}
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AudioMgr::SetVolume  Private
///
///	\param channelIndex The channel index used to set the volume
///
///	Set the volume for a channel.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void AudioMgr::SetVolume( SoundInstance& sound, float32 normalizedVolume )
{
	// If the channel is an invalid index then bail
	if( !sound.IsValid() )
		return;

	// If this volume is in the fade list then update the goal volume
	FadeList::iterator prevIter = m_FadeList.end();
	for( FadeList::iterator iterSound = m_FadeList.begin(); iterSound != m_FadeList.end(); ++iterSound )
	{
		if( iterSound->_sound == sound && iterSound->isFadeIn )
		{
			iterSound->fadeToVolume = normalizedVolume;
			break;
		}
	}

	// Set the sound's volume
	sound.SetVolume_Internal( normalizedVolume );
}

void AudioMgr::SetVolume( MusicStream* pMusic, float32 normalizedVolume )
{
	// If the channel is an invalid index then bail
	if( !pMusic )
		return;

	// If this music is in the fade list then update the goal volume
	FadeList::iterator prevIter = m_FadeList.end();
	for( FadeList::iterator iterSound = m_FadeList.begin(); iterSound != m_FadeList.end(); ++iterSound )
	{
		if( iterSound->m_MusicStream.GetObj() == pMusic && iterSound->isFadeIn )
		{
			iterSound->fadeToVolume = normalizedVolume;
			break;
		}
	}

	// Set the sound's volume
	pMusic->SetVolume_Internal( normalizedVolume );
}
