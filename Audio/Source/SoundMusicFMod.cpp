#include "../SoundMusic.h"
#include "../AudioMgr.h"
#include "../FMod/Fmod.h"


void SetChannelVolume( int32 channelIndex, float normalizedVolume )
{
	// Get the volume into an integer with a 0 to 255 range
	int volume = (int)(normalizedVolume * 255.0f);
	if( volume < 0 )
		volume = 0;
	else if( volume > 255 )
		volume = 255;

	// Set the channel volume
	FSOUND_SetVolume( (int)channelIndex, volume );
}


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

	// If the sound is not looping and not fading in then just play
	if( !loop && timeToFullVolume <= 0.0f )
	{
		SoundInstance retSound;
		retSound._playingChannel = FSOUND_PlaySound( FSOUND_FREE, (FSOUND_SAMPLE*)_pFModHandle );
		retSound._sourceSound = this;

		return retSound;
	}

	// Play the sound, but start it out paused
	int playChannel = FSOUND_PlaySoundEx( FSOUND_FREE, (FSOUND_SAMPLE*)_pFModHandle, NULL, 1 );
	if( playChannel == -1 )
		return SoundInstance();

	SoundInstance retSound;
	retSound._playingChannel = playChannel;
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
		FSOUND_SetVolume( playChannel, 0 );
	}

	// Set the looping and play
	if( loop )
		FSOUND_SetLoopMode( playChannel, FSOUND_LOOP_NORMAL );
	FSOUND_SetPaused( playChannel, 0 );

	return retSound;
}


bool SoundInstance::IsValid() const
{
	return _sourceSound.GetObj() != NULL && _playingChannel != -1;
}

float32 SoundInstance::GetVolume()
{
	return (float32)FSOUND_GetVolume( _playingChannel ) / 255.0f;
}

/// Called by the audio manager to set the volume taking into account fading
void SoundInstance::SetVolume_Internal( float32 normalizedVolume )
{
	SetChannelVolume( _playingChannel, normalizedVolume );
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
	FSOUND_StopSound( _playingChannel );

	_playingChannel = -1;

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
	// If the music is looping and not fading in then just play
	if( loop && timeToFullVolume <= 0.0f )
	{
		m_PlayingChannel = FSOUND_Stream_Play( FSOUND_FREE, (FSOUND_STREAM*)m_pStreamHandle );
		m_OrigFreq = FSOUND_GetFrequency( m_PlayingChannel );
		return m_PlayingChannel != -1;
	}

	// Play the sound, but start it out paused
	m_PlayingChannel = FSOUND_Stream_PlayEx( FSOUND_FREE, (FSOUND_STREAM*)m_pStreamHandle, NULL, 1 );
	if( m_PlayingChannel == -1 )
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
		FSOUND_SetVolume( m_PlayingChannel, 0 );
	}

	// Set the looping and play
	FSOUND_Stream_SetLoopCount( (FSOUND_STREAM*)m_pStreamHandle, loop ? -1 : 0 );
	FSOUND_SetPaused( m_PlayingChannel, 0 );
	m_OrigFreq = FSOUND_GetFrequency( m_PlayingChannel );

	return true;
}

void MusicStream::SetVolume( float32 normalizedVolume )
{
	AudioMgr::Get().SetVolume( this, normalizedVolume );
}

void MusicStream::SetVolume_Internal( float32 normalizedVolume )
{
	SetChannelVolume( m_PlayingChannel, normalizedVolume );
}

float32 MusicStream::GetVolume()
{
	return (float32)FSOUND_GetVolume( m_PlayingChannel ) / 255.0f;
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
	int newFreq = (int)((float32)m_OrigFreq * frequencyScale );
	FSOUND_SetFrequency( m_PlayingChannel, newFreq );
}

void MusicStream::Stop()
{
	// Stop the music from playing
	FSOUND_Stream_Stop( (FSOUND_STREAM*)m_pStreamHandle );
	FSOUND_Stream_SetPosition( (FSOUND_STREAM*)m_pStreamHandle, 0 );

	// Clear the playing channel since it is not playing
	m_PlayingChannel = -1;
}


AudioSample::~AudioSample()
{
	if( _pFModHandle )
		FSOUND_Sample_Free( (FSOUND_SAMPLE*)_pFModHandle );
	_pFModHandle = 0;
}

/// The destructor
MusicStream::~MusicStream()
{
	// Free the stream
	if( m_pStreamHandle )
	{
		FSOUND_Stream_Close( (FSOUND_STREAM*)m_pStreamHandle );
		m_pStreamHandle = 0;
	}

	// Free the music data
	if( m_pMusicData )
	{
		delete [] m_pMusicData;
		m_pMusicData = 0;
	}
}