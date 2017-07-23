//=================================================================================================
/*!
	\file AudioMgr.h
	Audio Library
	Audio Manager Header
	\author Taylor Clark
	\date June 3, 2006

	This file contains the definition for the audio manager class.
*/
//=================================================================================================

#pragma once
#ifndef __AudioMgr_h
#define __AudioMgr_h

// Ensure one or the other is specified
#ifndef SFML
// Avoid redefinition
#ifndef AUDIO_FMODEX
#define AUDIO_FMODEX
#endif
#endif

#include "Base/Types.h"
#include "SoundMusic.h"
#include <list>
#include "Base/DataBlock.h"

#ifdef AUDIO_FMODEX
//#include <Fmod.hpp>
namespace FMOD
{
    class System;
}
#endif


//-------------------------------------------------------------------------------------------------
/*!
	\class AudioMgr
	\brief The singleton class for audio related functionality.

	This class defines the manager for all audio related functionality.  It uses the singleton
	pattern so that there is only one instance of the class.
*/
//-------------------------------------------------------------------------------------------------
class AudioMgr
{
	friend struct AudioSample;
	friend struct SoundInstance;
	friend struct MusicStream;
private:
   
#ifdef AUDIO_FMODEX
    FMOD::System* _pSystem;
#endif

	/// A structure used to keep track of channels that need to be faded out
	struct ScheduledFade
	{
		bool isFadeIn;
		SoundInstance _sound;
		float32 timeToFade;
		float32 elapsedTime;
		float fadeToVolume;
		MusicStreamHndl m_MusicStream;

		float32 initialVolumeScale;

		/// The default constructor
		ScheduledFade() : isFadeIn( false ),
							timeToFade( 1.0f ),
							elapsedTime( 0.0f ),
							fadeToVolume( 0.0f ),
							initialVolumeScale( 1.0f )
		{}

	};

	/// The default constructor, private so no instances can be created
	AudioMgr()
#ifdef AUDIO_FMODEX
                : _pSystem(NULL)
#endif
	{}

	/// Channels to fade out
	typedef std::list< ScheduledFade > FadeList;
	FadeList m_FadeList;

	/// The current backgound music playing
	MusicStreamHndl m_CurBGM;

#ifdef SFML
	std::list<sf::Sound*> _playingSounds;
#endif

	/// Set a channel's volume with 0.0 being silent and 1.0 being full volume
	void SetVolume( SoundInstance& sound, float32 normalizedVolume );

	/// Set a channel's volume with 0.0 being silent and 1.0 being full volume
	void SetVolume( MusicStream* pMusic, float32 normalizedVolume );
public:

	/// Get the one and only instance of the audio manager
	static AudioMgr& Get()
	{
		static AudioMgr s_Mgr;
		return s_Mgr;
	}

	/// Initialize the audio manager
	bool Init();

	/// Close down the audio manager
	void Term();

	/// Update the audio manager
	void Update( float32 frameTime );

	/// Set the master volume
	void SetMasterVolume( float32 normalizedVolume );

	/// Get the master volume
	float32 GetMasterVolume() const;

	/// Set the background music
	void SetBGMusic( MusicStream* pStream );

	void FadeOutAndReleaseSound( SoundInstance& sound, float32 timeToFade );

	/// Fade out and stop a music stream
	void FadeOutMusic( MusicStream* pStream, float32 timeToFade = 1.0f );

#ifdef SFML
	void AddPlayingInstance(sf::Sound* pPlayingAudio);
	void UpdateSFML();
#endif

	/// Determine if a channel is currently playing a sample
	//bool IsChannelPlaying( int32 channelIndex );

	/// Set a channel's pan with -1.0 being full left and 1.0 being full right
	//void SetPan( int32 channelIndex, float32 panAmt );

	/// Set a channel's pan
	//void ResetPan( int32 channelIndex );

	/// Set a channel's frequency
	//void SetFrequency( int32 channelIndex, int freq );

	/// Free any resources used by the audio manager
	void FreeResources()
	{
		m_FadeList.clear();
		m_CurBGM.ReleaseHandle();
	}

	/// Load a sound from memory
	static AudioSample* LoadSoundFromMemory( ResourceID resID, DataBlock* pDataBlock );

	/// Load a music stream from memory
	static MusicStream* LoadMusicFromMemory( ResourceID resID, DataBlock* pDataBlock );
};

#endif // __AudioMgr_h
