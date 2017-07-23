//=================================================================================================
/*!
	\file AudioMgrSFML.cpp
	Audio Library
	Audio Manager Header
	\author Taylor Clark
	\date March 29, 2010

	This source file contains the implementation for the audio manager class that uses SFML to
	play audio.
*/
//=================================================================================================

#include "../AudioMgr.h"
#include "Base/MsgLogger.h"
#include "Resource/ResourceMgr.h"
#include <fstream>
#include <SFML/Audio.hpp>


/// Set the master volume
void AudioMgr::SetMasterVolume( float32 normalizedVolume )
{
	sf::Listener::setGlobalVolume( normalizedVolume );
}

/// Get the master volume
float32 AudioMgr::GetMasterVolume() const
{
	return static_cast<float32>( sf::Listener::getGlobalVolume() );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AudioMgr::LoadSoundFromMemory  Static Public
///
///	\param resID The resource ID of the sample being loaded
///	\param dataBlock The data block object representing the audio data
///	\returns A pointer to the loaded audio sample, NULL on failure
///
///	Load a sound from memory
///
///////////////////////////////////////////////////////////////////////////////////////////////////
AudioSample* AudioMgr::LoadSoundFromMemory( ResourceID resID, DataBlock* pDataBlock )
{
	sf::SoundBuffer* pNewSound = new sf::SoundBuffer();
	uint32 dataLength = pDataBlock->GetRemainingBytes();
	if( !pNewSound->loadFromMemory( reinterpret_cast<const char*>(pDataBlock->ReadData(dataLength)), dataLength ) )
	{
		delete pNewSound;
		return NULL;
	}

	AudioSample* pRetAudio = new AudioSample(resID);
	pRetAudio->_pSoundBuffer = pNewSound;

	return pRetAudio;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AudioMgr::LoadMusicFromMemory  Static Public
///
///	\param resID The resource ID of the music being loaded
///	\param dataBlock The data block object representing the audio data
///	\returns A pointer to the loaded music, NULL on failure
///
///	Load a music stream from memory.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
MusicStream* AudioMgr::LoadMusicFromMemory( ResourceID resID, DataBlock* pDataBlock )
{
	sf::Music* pNewMusic = new sf::Music();
	uint32 dataLength = pDataBlock->GetRemainingBytes();
	if( !pNewMusic->openFromMemory( reinterpret_cast<const char*>(pDataBlock->ReadData(dataLength)), dataLength ) )
	{
		delete pNewMusic;
		return NULL;
	}

	MusicStream* pRetMusic = new MusicStream(resID);
	pRetMusic->_pMusicHandle = pNewMusic;

	return pRetMusic;
}

void AudioMgr::UpdateSFML()
{
	for( std::list<sf::Sound*>::iterator iterCurSound = _playingSounds.begin();
			iterCurSound != _playingSounds.end(); )
	{
		// If the sound has stopped then remove it from the list
		sf::Sound* pCurSound = *iterCurSound;
		if( pCurSound->getStatus() == sf::Sound::Stopped )
			iterCurSound = _playingSounds.erase( iterCurSound );
		else
			++iterCurSound;
	}
}

void AudioMgr::AddPlayingInstance(sf::Sound* pPlayingAudio)
{
	_playingSounds.push_back( pPlayingAudio );
}



bool AudioMgr::Init()
{
	// SFML doesn't have any special intialization
	return true;
}

void AudioMgr::Term()
{
	// Stop all audio resources
	for( std::list<sf::Sound*>::iterator iterCurSound = _playingSounds.begin();
			iterCurSound != _playingSounds.end();
			++iterCurSound )
	{
		sf::Sound* pCurSound = *iterCurSound;
		pCurSound->stop();
	}
}