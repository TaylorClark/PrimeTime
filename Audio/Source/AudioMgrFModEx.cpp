//=================================================================================================
/*!
\file AudioMgrFModEx.cpp
Audio Library
Audio Manager Header
\author Taylor Clark
\date April 4, 2013

This source file contains the implementation for the audio manager class that uses FMod to
play audio.
*/
//=================================================================================================

#include "../AudioMgr.h"
#include "Base/MsgLogger.h"
#include "Resource/ResourceMgr.h"
#include <fstream>
#include <fmod.hpp>
#include <fmod_errors.h>

const int32 MAX_CHANNELS = 32;

    

extern void ERRCHECK(FMOD_RESULT result)
{
    if (result != FMOD_OK)
    {
        printf("FMOD error! (%d) %s\n", result, FMOD_ErrorString(result));
        exit(-1);
    }
}


/// Set the master volume
void AudioMgr::SetMasterVolume( float32 normalizedVolume )
{
    FMOD::ChannelGroup* pMasterChannelGroup = NULL;
    FMOD_RESULT result = AudioMgr::Get()._pSystem->getMasterChannelGroup( &pMasterChannelGroup );
    ERRCHECK(result);

    result = pMasterChannelGroup->setVolume( normalizedVolume );
    ERRCHECK(result);
}

/// Get the master volume
float32 AudioMgr::GetMasterVolume() const
{
    FMOD::ChannelGroup* pMasterChannelGroup = NULL;
    FMOD_RESULT result = AudioMgr::Get()._pSystem->getMasterChannelGroup( &pMasterChannelGroup );
    ERRCHECK(result);

    float retVolume = 1.0f;
    result = pMasterChannelGroup->getVolume( &retVolume );
    ERRCHECK(result);

    return retVolume;
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

    FMOD_CREATESOUNDEXINFO exinfo;
    memset( &exinfo, 0, sizeof(exinfo) );
    exinfo.cbsize = sizeof(exinfo);
    exinfo.length = dataSize;
    
    FMOD_RESULT result = AudioMgr::Get()._pSystem->createSound(pData, FMOD_DEFAULT | FMOD_OPENMEMORY, &exinfo, &pRetVal->_pSound);
    ERRCHECK(result);

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
    FMOD_CREATESOUNDEXINFO exinfo;
    memset( &exinfo, 0, sizeof(exinfo) );
    exinfo.cbsize = sizeof(exinfo);
    exinfo.length = dataLength;
    FMOD_RESULT result = AudioMgr::Get()._pSystem->createStream( (const char*)pRetVal->m_pMusicData, FMOD_DEFAULT | FMOD_OPENMEMORY_POINT | FMOD_LOOP_NORMAL, &exinfo, &pRetVal->_pSound);
    ERRCHECK(result);

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
    FMOD_RESULT result;
    unsigned int version;
    int numdrivers;
    FMOD_SPEAKERMODE speakermode;
    FMOD_CAPS caps;
    char name[256];
    /*
    Create a System object and initialize.
    */
    result = FMOD::System_Create(&_pSystem);
    ERRCHECK(result);

    result = _pSystem->getVersion(&version);
    ERRCHECK(result);
    if (version < FMOD_VERSION)
    {
        printf("Error! You are using an old version of FMOD %08x. This program requires %08x\n",
            version, FMOD_VERSION);
        return 0;
    }
    result = _pSystem->getNumDrivers(&numdrivers);
    ERRCHECK(result);
    if (numdrivers == 0)
    {
        result = _pSystem->setOutput(FMOD_OUTPUTTYPE_NOSOUND);
        ERRCHECK(result);
    }
    else
    {
        result = _pSystem->getDriverCaps(0, &caps, 0, &speakermode);
        ERRCHECK(result);
        /*
        Set the user selected speaker mode.
        */
        result = _pSystem->setSpeakerMode(speakermode);
        ERRCHECK(result);
        if (caps & FMOD_CAPS_HARDWARE_EMULATED)
        {
            /*
            The user has the 'Acceleration' slider set to off! This is really bad
            for latency! You might want to warn the user about this.
            */
            result = _pSystem->setDSPBufferSize(1024, 10);
            ERRCHECK(result);
        }
        result = _pSystem->getDriverInfo(0, name, 256, 0);
        ERRCHECK(result);
        if (strstr(name, "SigmaTel"))
        {
            /*
            Sigmatel sound devices crackle for some reason if the format is PCM 16bit.
            PCM floating point output seems to solve it.
            */
            result = _pSystem->setSoftwareFormat(48000, FMOD_SOUND_FORMAT_PCMFLOAT, 0,0,
                FMOD_DSP_RESAMPLER_LINEAR);
            ERRCHECK(result);
        }
    }

    result = _pSystem->init(100, FMOD_INIT_NORMAL, 0);
    if (result == FMOD_ERR_OUTPUT_CREATEBUFFER)
    {
        /*
        Ok, the speaker mode selected isn't supported by this soundcard. Switch it
        back to stereo...
        */
        result = _pSystem->setSpeakerMode(FMOD_SPEAKERMODE_STEREO);
        ERRCHECK(result);
        /*
        ... and re-init.
        */
        result = _pSystem->init(100, FMOD_INIT_NORMAL, 0);
    }
    ERRCHECK(result);

    return true;
}

void AudioMgr::Term()
{
    // Release all sounds

    FMOD_RESULT result = _pSystem->close();
    ERRCHECK(result);

    result = _pSystem->release();
    ERRCHECK(result);
}