//=================================================================================================
/*!
	\file InstructionStreamReplay.h
	Game Play Library
	Instruction Stream Base Class Header
	\author Taylor Clark
	\date January 2, 2008

	This header contains the definition for the instruction stream base class.
*/
//=================================================================================================

#pragma once
#ifndef __InstructionStreamReplay_h
#define __InstructionStreamReplay_h

#include "InstructionStream.h"
#include <list>
#include <fstream>
#include "Base/TCAssert.h"

class Serializer;
class DataBlockOut;
typedef std::list< GameFieldInstruction* > InstructionList;


//-------------------------------------------------------------------------------------------------
/*!
	\class InstructionStreamReplay
	\brief Represents a stream of game field instructions loaded from a recorded game.
*/
//-------------------------------------------------------------------------------------------------
class InstructionStreamReplay : public InstructionStream
{
private:

	/// The list of instructions to be processed at update
	InstructionList m_PendingInstructions;

	InstructionList m_AllInstructions;

	InstructionList::iterator m_PrevFrameIter;

public:

	/// The default constructor
	InstructionStreamReplay()
	{
		m_PrevFrameIter = m_AllInstructions.begin();
	}

	void Init( Serializer& serializer );

	/// Add an instruction to handled at the next possible time
	virtual void AddPendingInstruction( GameFieldInstruction* pInst );

	/// Allow for calculation based on frame detlta
	virtual void Update( float32 fieldTime, float32 frameTime );

	/// Get the next pending game field instruction
	virtual GameFieldInstruction* PopPendingInstruction()
	{
		if( m_PendingInstructions.size() > 0 )
		{
			GameFieldInstruction* pNextInst = m_PendingInstructions.front();
			m_PendingInstructions.pop_front();
			return pNextInst;
		}

		return 0;
	}

	/// Perform any processing on a handled instruction. Instructions should be freed in this
	/// method if they are no longer needed.
	virtual void ProcessHandledInstruction( GameFieldInstruction* ){}

	virtual bool HasPendingInstructions() const { return m_PendingInstructions.size() > 0; }
};

#endif // __InstructionStreamReplay_h