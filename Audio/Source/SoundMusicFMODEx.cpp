#include "../SoundMusic.h"
#include "../AudioMgr.h"
#include <fmod.hpp>
#include <fmod_errors.h>


extern void ERRCHECK(FMOD_RESULT result);

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
    FMOD_RESULT result;

	// If the sound is not looping and not fading in then just play
	if( !loop && timeToFullVolume <= 0.0f )
	{
		SoundInstance retSound;
        result = AudioMgr::Get()._pSystem->playSound( FMOD_CHANNEL_FREE, _pSound, false, &retSound._pPlayingChannel );
        ERRCHECK(result);
		retSound._sourceSound = this;

		return retSound;
	}

	// Play the sound, but start it out paused
	FMOD::Channel* pPlayChannel = NULL;
    result = AudioMgr::Get()._pSystem->playSound( FMOD_CHANNEL_FREE, _pSound, true, &pPlayChannel );
    ERRCHECK(result);
	if( result != FMOD_OK )
		return SoundInstance();

	SoundInstance retSound;
	retSound._pPlayingChannel = pPlayChannel;
	retSound._sourceSound = this;

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

		// Set the volume to 0 initially
        retSound._pPlayingChannel->setVolume( 0 );
	}

	// Set the looping and play
	if( loop )
        retSound._pPlayingChannel->setLoopCount( -1 );
    retSound._pPlayingChannel->setPaused( false );

	return retSound;
}


bool SoundInstance::IsValid() const
{
	return _sourceSound.GetObj() != NULL && _pPlayingChannel != NULL;
}

float32 SoundInstance::GetVolume()
{
    float volume = 0.0f;

    if( !_pPlayingChannel )
        return volume;

    FMOD_RESULT result = _pPlayingChannel->getVolume( &volume );
    ERRCHECK( result );

    return volume;
}

/// Called by the audio manager to set the volume taking into account fading
void SoundInstance::SetVolume_Internal( float32 normalizedVolume )
{
    if( !_pPlayingChannel )
        return;

    _pPlayingChannel->setVolume( normalizedVolume );
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
    if( !_pPlayingChannel )
        return;

    _pPlayingChannel->stop();
    _pPlayingChannel = NULL;

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
    FMOD_RESULT result;

	// If the music is looping and not fading in then just play
	if( loop && timeToFullVolume <= 0.0f )
	{
        result = AudioMgr::Get()._pSystem->playSound( FMOD_CHANNEL_FREE, _pSound, false, &_pPlayingChannel );
        ERRCHECK(result);
        
        if( result != FMOD_OK )
            return false;

        result = _pPlayingChannel->getFrequency( &_originalFrequency );
        ERRCHECK(result);

		return true;
	}

	// Play the sound, but start it out paused
    result = AudioMgr::Get()._pSystem->playSound( FMOD_CHANNEL_FREE, _pSound, true, &_pPlayingChannel );
	if( result != FMOD_OK )
        return false;

	// If the sound is fading in
	if( timeToFullVolume > 0.0f )
	{
		// Create the fade object
		AudioMgr::ScheduledFade newFadeIn;
		newFadeIn.timeToFade = timeToFullVolume;
		newFadeIn.isFadeIn = true;
		newFadeIn.fadeToVolume = 1.0f;
		newFadeIn.m_MusicStream = this;

		// Add the new fade out
		AudioMgr::Get().m_FadeList.push_back( newFadeIn );

		// Set the volume to 0 initially
        _pPlayingChannel->setVolume( 0.0f );
	}

	// Set the looping and play
    _pPlayingChannel->setLoopCount( loop ? -1 : 0 );
    _pPlayingChannel->setPaused( false );
	
    result = _pPlayingChannel->getFrequency( &_originalFrequency );
    ERRCHECK(result);

	return true;
}

void MusicStream::SetVolume( float32 normalizedVolume )
{
	AudioMgr::Get().SetVolume( this, normalizedVolume );
}

void MusicStream::SetVolume_Internal( float32 normalizedVolume )
{
    if( !_pPlayingChannel )
        return;

    _pPlayingChannel->setVolume( normalizedVolume );
}

float32 MusicStream::GetVolume()
{
    float volume = 0.0f;

    _pPlayingChannel->getVolume( &volume );

    return volume;
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
    if( !_pPlayingChannel )
        return;

	// Update the frequency
    _pPlayingChannel->setFrequency( _originalFrequency * frequencyScale );
}

void MusicStream::Stop()
{
    if( !_pPlayingChannel )
        return;

    _pPlayingChannel->stop();
    //_pPlayingChannel->setPosition( 0, FMOD_TIMEUNIT_MS );

	// Clear the playing channel since it is not playing
    _pPlayingChannel = NULL;
}


AudioSample::~AudioSample()
{
	if( _pSound )
        _pSound->release();
	_pSound = 0;
}

/// The destructor
MusicStream::~MusicStream()
{
	// Free the stream
	if( _pSound )
    {
        _pSound->release();
    	_pSound = 0;
    }

	// Free the music data
	if( m_pMusicData )
	{
		delete [] m_pMusicData;
		m_pMusicData = 0;
	}
}