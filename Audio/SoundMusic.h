//=================================================================================================
/*!
	\file AudioMgr.h
	Audio Library
	Sound and Music Header
	\author Taylor Clark
	\date March 30, 2010

	This file contains the definition for the audio manager class.
*/
//=================================================================================================

#pragma once
#ifndef __SoundMusic_h
#define __SoundMusic_h

#include "Resource/Resource.h"

#ifndef AUDIO_FMODEX
#define AUDIO_FMODEX
#endif

// Ensure one or the other is specified
#ifndef SFML
// Avoid redefinition
#ifndef AUDIO_FMODEX
#define AUDIO_FMODEX
#endif
#endif

#ifdef AUDIO_FMODEX
namespace FMOD
{
    class Sound;
    class Channel;
}
#endif

struct SoundInstance;
class AudioMgr;

#ifdef SFML
namespace sf
{
	class SoundBuffer;
	class Sound;
	class Music;
}
#endif

//-------------------------------------------------------------------------------------------------
/*!
	\class AudioSample
	\brief Represents the data source for sounds
*/
//-------------------------------------------------------------------------------------------------
struct AudioSample : public Resource
{
#ifdef AUDIO_FMOD
	void* _pFModHandle;
#elif defined AUDIO_FMODEX
    FMOD::Sound* _pSound;
#elif defined SFML
	sf::SoundBuffer* _pSoundBuffer;
#endif

	/// The default constructor
	AudioSample( ResourceID resID ) : Resource( resID ),
#ifdef AUDIO_FMOD
	_pFModHandle( 0 )
#elif defined AUDIO_FMODEX
	_pSound( 0 )
#elif defined SFML
	_pSoundBuffer( 0 )
#endif
	{}

	/// The destructor
	~AudioSample();

	/// Get the resource type
	virtual EResourceType GetResType() const { return RT_Sound; }

	SoundInstance Play();
};

/// A reference counted reference sprite
typedef RefCountHandle<AudioSample> AudioSampleHndl;


//-------------------------------------------------------------------------------------------------
/*!
	\class SoundInstance
	\brief Represents an instance of a AudioSample being played
*/
//-------------------------------------------------------------------------------------------------
struct SoundInstance
{
	friend struct AudioSample;
	friend class AudioMgr;

private:

	AudioSampleHndl _sourceSound;

#ifdef AUDIO_FMOD
	int32 _playingChannel;
#elif defined AUDIO_FMODEX
    FMOD::Channel* _pPlayingChannel;
#elif defined SFML
	sf::Sound* _pSound;
#endif

	/// Called by the audio manager to set the volume taking into account fading
	void SetVolume_Internal( float32 normalizedVolume );
public:	

#ifdef AUDIO_FMOD
	SoundInstance() : _playingChannel(-1)
#elif defined AUDIO_FMODEX
	SoundInstance() : _pPlayingChannel( 0 )
#elif defined SFML
	SoundInstance() : _pSound(0)
#endif
	{
	}

	bool IsValid() const;

	void SetVolume( float32 normalizedVolume );

	float32 GetVolume();

	void StopAndRelease();

	bool operator ==( const SoundInstance& rhs )
	{
#ifdef AUDIO_FMOD
	return _sourceSound == rhs._sourceSound && _playingChannel == rhs._playingChannel;
#elif defined AUDIO_FMODEX
	return _sourceSound == rhs._sourceSound && _pPlayingChannel == rhs._pPlayingChannel;
#elif defined SFML
	return _sourceSound == rhs._sourceSound && _pSound == rhs._pSound;
#endif
		
	}
};


//-------------------------------------------------------------------------------------------------
/*!
	\class MusicStream
	\brief Represents a song
*/
//-------------------------------------------------------------------------------------------------
struct MusicStream : public Resource
{
	friend class AudioMgr;
private:

	void SetVolume_Internal( float32 normalizedVolume );

public:
#ifdef AUDIO_FMOD
	/// The original frequency of the music stream
	int m_OrigFreq;

	/// The music data, which needs to be stored for FMOD to use it in a stream
	uint8* m_pMusicData;

	/// The FSOUND_STREAM handle
	void* m_pStreamHandle;

	/// The channel that this stream is playing on, -1 if the stream is not playing
	int m_PlayingChannel;
#elif defined AUDIO_FMODEX
    /// The original frequency of the music stream
	float _originalFrequency;

	/// The music data, which needs to be stored for FMOD to use it in a stream
	uint8* m_pMusicData;

	/// The FSOUND_STREAM handle
	FMOD::Sound* _pSound;

	/// The channel that this stream is playing on, NULL if the stream is not playing
	FMOD::Channel* _pPlayingChannel;
#elif defined SFML

	sf::Music* _pMusicHandle;

	float32 _origPitch;
#endif
	bool Play( bool loop = true, float timeToFullVolume = 0.0f );

	void Stop();

	void SetVolume( float32 normalizedVolume );

	float32 GetVolume();

	void SetFrequencyScale( float32 frequencyScale );

	/// The default constructor
	MusicStream( ResourceID resID ) : Resource( resID ),
#ifdef AUDIO_FMOD
										m_pMusicData( 0 ),
										m_pStreamHandle( 0 ),
										m_PlayingChannel( -1 ),
										m_OrigFreq( 0 )
#elif defined AUDIO_FMODEX
										m_pMusicData( 0 ),
										_pSound( 0 ),
										_pPlayingChannel( 0 ),
										_originalFrequency( 0 )
#elif defined SFML
										_pMusicHandle( 0 ),
										_origPitch( 1.0f )
#endif
	{}

	/// The destructor
	~MusicStream();

	/// Get the resource type
	virtual EResourceType GetResType() const { return RT_Music; }
};

/// A reference counted reference sprite
typedef RefCountHandle<MusicStream> MusicStreamHndl;

#endif // __SoundMusic_h