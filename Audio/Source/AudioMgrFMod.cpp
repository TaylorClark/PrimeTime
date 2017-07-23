//=================================================================================================
/*!
	\file AudioMgrFMod.cpp
	Audio Library
	Audio Manager Header
	\author Taylor Clark
	\date March 29, 2010

	This source file contains the implementation for the audio manager class that uses FMod to
	play audio.
*/
//=================================================================================================

#include "../AudioMgr.h"
//#include "../FMod/Fmod.h"
#include <Fmod.hpp>
#include "Base/MsgLogger.h"
#include "Resource/ResourceMgr.h"
#include <fstream>

const int32 MAX_CHANNELS = 32;


/// Set the master volume
void AudioMgr::SetMasterVolume( float32 normalizedVolume )
{
	// Get the value into FMod's range
	int volVal = (int)(normalizedVolume * 255.0f);
	if( volVal < 0 )
		volVal = 0;
	else if( volVal > 255 )
		volVal = 255;

	// Set the sound effect volume
	FSOUND_SetSFXMasterVolume( volVal );
}

/// Get the master volume
float32 AudioMgr::GetMasterVolume() const
{
	return (float32)FSOUND_GetSFXMasterVolume() / 255.0f;
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
	AudioSample* pRetVal = new AudioSample( resID );

	// Get the sound data
	int dataSize = (int)pDataBlock->GetRemainingBytes();
	const char* pData = (const char*)pDataBlock->ReadData(dataSize);

	// Load the sample
	pRetVal->_pFModHandle = FSOUND_Sample_Load( FSOUND_FREE, pData, FSOUND_LOADMEMORY | FSOUND_STEREO | FSOUND_16BITS | FSOUND_SIGNED, 0, dataSize);
	
	return pRetVal;
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
	MusicStream* pRetVal = new MusicStream( resID );

	// Store the stream
	uint32 dataLength = pDataBlock->GetRemainingBytes();
	pRetVal->m_pMusicData = (uint8*)pDataBlock->ReadData( dataLength );

	// Open the stream
	pRetVal->m_pStreamHandle = FSOUND_Stream_Open( (const char*)pRetVal->m_pMusicData, FSOUND_LOADMEMORY | FSOUND_LOOP_NORMAL, 0, (int)dataLength );

	// Return the resource
	return pRetVal;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  AudioMgr::IsChannelPlaying  Public
///
///	\param channelIndex The channel to test
///	\returns True if a sample is playing on the passed-in channel, otherwise false
///
///	Determine if a sample is playing.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
//bool AudioMgr::IsChannelPlaying( int32 channelIndex )
//{
//	// Get the sample playing on the channel
//	FSOUND_SAMPLE* pSample = FSOUND_GetCurrentSample( (int)channelIndex );
//	if( !pSample )
//		return false;
//
//	// Get the sample total length
//	uint32 sampleLen = (uint32)FSOUND_Sample_GetLength( pSample );
//
//	// Get the play position
//	uint32 curPos = (uint32)FSOUND_GetCurrentPosition( channelIndex );
//
//	// If the sample is at the end then it is not playing
//	if( curPos == 0 || curPos == sampleLen )
//		return false;
//
//	// The sample is playing
//	return true;
//}

/// Set a channel's pan
//void AudioMgr::SetPan( int32 channelIndex, float32 panAmt )
//{
//	// If the channel is an invalid index then bail
//	if( channelIndex < 0 )
//		return;
//
//	// Get the -1.0 to 1.0 range into the FMod range
//	int fmodPan = (int)( (panAmt * 128.0f) + 128.0f );
//	if( fmodPan < 0 )
//		fmodPan = 0;
//	else if( fmodPan > 255 )
//		fmodPan = 255;
//
//	// Set the pan
//	FSOUND_SetPan( (int)channelIndex, fmodPan );
//}


/// Set a channel's pan
//void AudioMgr::ResetPan( int32 channelIndex )
//{
//	// If the channel is an invalid index then bail
//	if( channelIndex < 0 )
//		return;
//
//	// Reset the pan
//	FSOUND_SetPan( (int)channelIndex, FSOUND_STEREOPAN );
//}


/// Set a channel's frequency
//void AudioMgr::SetFrequency( int32 channelIndex, int freq )
//{
//	// If the channel is an invalid index then bail
//	if( channelIndex < 0 )
//		return;
//
//	// Set the frequency
//	FSOUND_SetFrequency( (int)channelIndex, freq );
//}


bool AudioMgr::Init()
{
	if (FSOUND_GetVersion() < FMOD_VERSION)
    {
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Wrong DLL version of FMod detected, should be using FMOD %.02f\n", FMOD_VERSION);
		return false;
	}

	// Initialize using CD quality sound
	if( !FSOUND_Init( 44100, MAX_CHANNELS, 0 ) )
	{
		MSG_LOGGER_OUT( MsgLogger::MI_Error, L"Failed to You are using the wrong DLL version! You should be using FMOD %.02f\n", FMOD_VERSION);
		
	}

	return true;
}

void AudioMgr::Term()
{
	FSOUND_Close();
}