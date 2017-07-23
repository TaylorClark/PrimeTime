//=================================================================================================
/*!
	\file InstructionStream.h
	Game Play Library
	Instruction Stream Base Class Header
	\author Taylor Clark
	\date January 2, 2008

	This header contains the definition for the instruction stream base class.
*/
//=================================================================================================

#pragma once
#ifndef __InstructionStream_h
#define __InstructionStream_h

#include "Base/Types.h"

class GameFieldInstruction;
class GameField;


//-------------------------------------------------------------------------------------------------
/*!
	\class InstructionStream
	\brief Represents a stream of game field instructions. This class is used to manager
			instructions, record gameplay, and replay gameplay.
*/
//-------------------------------------------------------------------------------------------------
class InstructionStream
{
public:

	GameField* m_pGameField;

	/// The default constructor
	InstructionStream() : m_pGameField(0)
	{
	}

	/// The destructor, virtual to ensure derived classes are freed properly
	virtual ~InstructionStream() {}

	// Disable the unused formal parameter warning
	#pragma warning( push )
	#pragma warning( disable:4100 )

	/// Allow for calculation based on frame detlta
	virtual void Update( float32 fieldTime, float32 frameTime ){}
	#pragma warning( pop )

	/// Add an instruction to handled at the next possible time
	virtual void AddPendingInstruction( GameFieldInstruction* pInst ) = 0;

	/// Get the next pending game field instruction
	virtual GameFieldInstruction* PopPendingInstruction() = 0;

	/// Perform any processing on a handled instruction. Instructions should be freed in this
	/// method if they are no longer needed.
	virtual void ProcessHandledInstruction( GameFieldInstruction* pInst ) = 0;

	/// Get if there are any tasks to be handled during the current game update
	virtual bool HasPendingInstructions() const = 0;
};

#endif // __InstructionStream_h