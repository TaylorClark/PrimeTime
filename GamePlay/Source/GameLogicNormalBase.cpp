/*=================================================================================================

	\file GameLogicNormalBase.cpp
	Game Play Library
	Base "Normal" Game Logic Source
	\author Taylor Clark
	\Date June 22, 2006

	This source file contains the implementation of the base game logic manager class. This
	"normal" manager includes logic for a field push blocks up and allows speed up.

=================================================================================================*/

#include "../GameLogicNormalBase.h"
#include "../GameField.h"
#include "../PIISetPushSpeed.h"
#include "../GameMgr.h"
#include "../GameFieldMultBase.h"


const float32 SPEED_KEY_SPEED = GameDefines::BLOCK_HEIGHT * 4.0f;

const ResourceID RESID_SND_RAISE = 130;


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogicNormalBase::SubclassUpdate  Private
///	\param frameTime The elapsed frame time
///
///	Update the game logic.
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicNormalBase::SubclassUpdate( float32 frameTime )
{
	// If the game is paused then bail before game updating
	if( GameMgr::Get().IsPaused() )
		return;

	GameFieldMultBase* pMultField = static_cast<GameFieldMultBase*>( GetGameField() );

	// If it is time to push up a new row
	if( pMultField->GetBottomMostBlockY() > (float32)(-GameDefines::BLOCK_HEIGHT / 2) )
	{
		GameFieldBlockMultBase** pBlocks = GenerateBlocksToFieldWidth();
		if( pBlocks && pBlocks[0] )
			pMultField->PushNewBlocks( pBlocks );
	}

	// Update the field
	pMultField->Update( frameTime );

	if( GameMgr::Get()._bgmOverlay.GetObj() )
		GameMgr::Get()._bgmOverlay.GetObj()->SetVolume( pMultField->GetDangerLevel() );

	// Speed up the music based on the danger level
	if( GameMgr::Get()._bgm.GetObj() )
		GameMgr::Get()._bgm.GetObj()->SetFrequencyScale( 1.0f + (pMultField->GetDangerLevel() * 0.5f) );

	if( GameMgr::Get()._bgmOverlay.GetObj() )
		GameMgr::Get()._bgmOverlay.GetObj()->SetFrequencyScale( 1.0f + (pMultField->GetDangerLevel() * 0.5f) );
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GameLogicProduct::OnSpeedButtonDown()  Public
///
/// A message handler called when the users presses the keyboard key to speed up block
/// production
///
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicNormalBase::OnSpeedButtonDown()
{
	// If we don't have a field or we are already accelerated then don't do anything
	if( !GetGameField() || m_IsPushAccelerated )
		return;

	m_IsPushAccelerated = true;
	
	GetGameField()->SendInstruction( new PIISetPushSpeed( SPEED_KEY_SPEED ) );

	// Play the raise sound
	if( _playingRaiseSound.IsValid() )
		_playingRaiseSound.StopAndRelease();

	AudioSampleHndl raiseSample = ResourceMgr::Get().GetAudioSample( RESID_SND_RAISE ).GetObj();
	if( raiseSample.GetObj() )
		_playingRaiseSound = raiseSample.GetObj()->Play();
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//  GameLogicProduct::OnSpeedButtonUp()  Public
///
/// A message handler called when the users release the keyboard key that speeds up block
/// production
///////////////////////////////////////////////////////////////////////////////////////////////////
void GameLogicNormalBase::OnSpeedButtonUp()
{
	if( !GetGameField() )
		return;

	m_IsPushAccelerated = false;

	GetGameField()->SendInstruction( new PIISetPushSpeed( GameLogic::GetPushUpSpeed(m_CurrentLevel) ) );

	// Stop the raise sound
	if( _playingRaiseSound.IsValid() )
		AudioMgr::Get().FadeOutAndReleaseSound( _playingRaiseSound, 0.25f );
}