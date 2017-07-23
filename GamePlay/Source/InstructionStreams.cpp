#include "../InstructionStreamPlay.h"
#include "../InstructionStreamReplay.h"
#include "../GameFieldInstruction.h"
#include "Base/Serializer.h"
#include "Base/DataBlockOut.h"
#include "../GameField.h"


///////////////////////////////////////////////////////////////////////////////////////////////////
//  InstructionStreamPlay::Init  Public
///	\param recordGame True to record the game instructions, otherwise false
///
///	Initialize the instruction stream.
///////////////////////////////////////////////////////////////////////////////////////////////////
void InstructionStreamPlay::Init( bool recordGame )
{
	if( recordGame )
	{
		m_OutFile.open( "c:\\TestGame.dat", std::ios_base::out | std::ios_base::binary );

		if( m_OutFile.good() )
			m_pOutStream = new Serializer( &m_OutFile );
	}
}


void InstructionStreamReplay::Init( Serializer& serializer )
{
	uint32 fileLen = serializer.GetInputLength();
	while( serializer.GetOffset() < fileLen )
	{
		// Read the instruction type ID
		uint8 instuctionID = 0;
		serializer.AddData( instuctionID );

		GameFieldInstruction* pNewInst = GameFieldInstruction::CreateInstruction( (EGameFieldInstruction)instuctionID );
		if( !pNewInst )
		{
		}

		pNewInst->Serialize( serializer );

		m_AllInstructions.push_back( pNewInst );
	}

	m_PrevFrameIter = m_AllInstructions.begin();
}

/// Allow for calculation based on frame detlta
void InstructionStreamReplay::Update( float32 fieldTime, float32 frameTime )
{
	m_PendingInstructions.clear();

	// Starting from the instruction after the last executed instruction, find instructions that
	// should be executed this frame
	while( m_PrevFrameIter != m_AllInstructions.end() && (*m_PrevFrameIter)->executionTime < (fieldTime + frameTime) )
	{
		m_PendingInstructions.push_back( *m_PrevFrameIter );
		++m_PrevFrameIter;
	}
}

/// Add an instruction to handled at the next possible time
void InstructionStreamReplay::AddPendingInstruction( GameFieldInstruction* pInst )
{
	delete pInst;
}

/// The destructor
InstructionStreamPlay::~InstructionStreamPlay()
{
	if( m_pOutStream )
	{
		delete m_pOutStream;
		m_OutFile.close();
		m_pOutStream = 0;
	}
}

/// Perform any processing on a handled instruction. Instructions should be freed in this
/// method if they are no longer needed.
void InstructionStreamPlay::ProcessHandledInstruction( GameFieldInstruction* pInst )
{
	pInst->executionTime = m_pGameField->GetFieldElapsedTime();

	if( m_pOutStream )
		pInst->Serialize( *m_pOutStream );
	
	delete pInst;
}