//=================================================================================================
/*!
	\file SoundMusicSFML.cpp
	Audio Library
	SFML Sound and Music Source
	\author Taylor Clark
	\date April 1, 2010

	This source file contains the implementation for the audio manager class that uses SFML to
	play audio.
*/
//=================================================================================================

#include "../SoundMusic.h"
#include "../AudioMgr.h"
#include <SFML/Audio.hpp>


///////////////////////////////////////////////////////////////////////////////////////////////////
//  AudioMgr::PlaySample  Private
///
///	\param sample The sample to play
///	\param loop If the sample is to loop, defaults to false
///	\param timeToFullVolume The time, in seconds, it takes for the sound to reach full volume,
///							defaults to no fade in
///	\returns The channel on which the sample was played, -1 on play failure
///
///	Play a sound sample.
///////////////////////////////////////////////////////////////////////////////////////////////////
SoundInstance AudioSample::Play()
{
	bool loop = false;
	float32 timeToFullVolume = 0.0f;

	SoundInstance retSound;
	retSound._pSound = new sf::Sound( *_pSoundBuffer );
	retSound._pSound->setLoop( loop );

	AudioMgr::Get().AddPlayingInstance( retSound._pSound );

	// If the sound is not looping and not fading in then just play
	if( timeToFullVolume <= 0.0f )
	{
		retSound._pSound->play();

		return retSound;
	}

	retSound._pSound->setVolume( 0.0f );

	// If the sound is fading in
	if( timeToFullVolume > 0.0f )
	{
		// Create the fade object
		AudioMgr::ScheduledFade newFadeIn;
		newFadeIn._sound = retSound;
		newFadeIn.timeToFade = timeToFullVolume;
		newFadeIn.isFadeIn = true;
		newFadeIn.fadeToVolume = 1.0f;

		// Add the new fade out
		AudioMgr::Get().m_FadeList.push_back( newFadeIn );
	}

	retSound._pSound->play();

	return retSound;
}

AudioSample::~AudioSample()
{
	if( _pSoundBuffer )
		delete _pSoundBuffer;
	_pSoundBuffer = NULL;
}

bool SoundInstance::IsValid() const
{
	return _sourceSound.GetObj() != NULL && _pSound != NULL;
}

float32 SoundInstance::GetVolume()
{
	return _pSound->getVolume() / 100.0f;
}

/// Called by the audio manager to set the volume taking into account fading
void SoundInstance::SetVolume_Internal( float32 normalizedVolume )
{
	_pSound->setVolume( normalizedVolume * 100.0f );
}

void SoundInstance::SetVolume( float32 normalizedVolume )
{
	AudioMgr::Get().SetVolume( *this, normalizedVolume );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  SoundInstance::StopAndRelease  Public
///
///	Stop a sound instance.
///////////////////////////////////////////////////////////////////////////////////////////////////
void SoundInstance::StopAndRelease()
{
	if( _pSound )
	{
		_pSound->stop();

		delete _pSound;
		_pSound = NULL;
	}

	_sourceSound.ReleaseHandle();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  MusicStream::PlayMusic  Public
///	\param loop If the music is to loop, defaults to false
///	\param timeToFullVolume The time, in seconds, it takes for the music to reach full volume,
///							defaults to no fade in
///	\returns If the music played or not
///
///	Play music.
///////////////////////////////////////////////////////////////////////////////////////////////////
bool MusicStream::Play( bool loop, float timeToFullVolume )
{
	_pMusicHandle->setLoop( loop );

	AudioMgr::Get().AddPlayingInstance( (sf::Sound*)_pMusicHandle );

	// If the music is looping and not fading in then just play
	if( timeToFullVolume <= 0.0f )
	{		
		_pMusicHandle->play();
		_origPitch = _pMusicHandle->getPitch();

		return _pMusicHandle->getStatus() == sf::Sound::Playing;
	}

	// Create the fade object
	AudioMgr::ScheduledFade newFadeIn;
	newFadeIn.timeToFade = timeToFullVolume;
	newFadeIn.isFadeIn = true;
	newFadeIn.fadeToVolume = 1.0f;
	newFadeIn.m_MusicStream = this;

	// Add the new fade out
	AudioMgr::Get().m_FadeList.push_back( newFadeIn );

	// Set the volume to 0 initially
	_pMusicHandle->setVolume( 0.0f );

	// Set the looping and play
	_pMusicHandle->play();
	_origPitch = _pMusicHandle->getPitch();

	return true;
}

void MusicStream::SetVolume( float32 normalizedVolume )
{
	AudioMgr::Get().SetVolume( this, normalizedVolume );
}

void MusicStream::SetVolume_Internal( float32 normalizedVolume )
{
	_pMusicHandle->setVolume( normalizedVolume * 100.0f );
}

float32 MusicStream::GetVolume()
{
	return (float32)_pMusicHandle->getVolume() / 100.0f;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//  MusicStream::ScaleMusicFreq  Private
///	\param pStream The music stream to modify
///	\param freqScale The frequency scale, 1.0 meaning none, 2.0 being double, etc.
///
/// Scale a music stream's frequency relative to the original frequency (i.e 1 would reset the
/// frequency to normal)
///////////////////////////////////////////////////////////////////////////////////////////////////
void MusicStream::SetFrequencyScale( float32 frequencyScale )
{
	// Update the frequency
	float32 newFreq = (float32)((float32)_origPitch * frequencyScale );
	_pMusicHandle->setPitch( newFreq );
}

void MusicStream::Stop()
{
	_pMusicHandle->stop();
}


/// The destructor
MusicStream::~MusicStream()
{
	// Free the stream
	if( _pMusicHandle )
	{
		_pMusicHandle->stop();
		delete _pMusicHandle;
		_pMusicHandle = 0;
	}
}