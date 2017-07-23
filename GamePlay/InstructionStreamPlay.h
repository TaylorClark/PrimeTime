//=================================================================================================
/*!
	\file InstructionStreamPlay.h
	Game Play Library
	Instruction Stream Base Class Header
	\author Taylor Clark
	\date January 2, 2008

	This header contains the definition for the instruction stream base class.
*/
//=================================================================================================

#pragma once
#ifndef __InstructionStreamPlay_h
#define __InstructionStreamPlay_h

#include "InstructionStream.h"
#include <list>
#include <fstream>
#include "GameField.h"

class Serializer;
class DataBlockOut;
typedef std::list< GameFieldInstruction* > InstructionList;


//-------------------------------------------------------------------------------------------------
/*!
	\class InstructionStreamPlay
	\brief Represents a stream of game field instructions. This class is used during a game that
			is being played to handle instructions and record them for replay later.
*/
//-------------------------------------------------------------------------------------------------
class InstructionStreamPlay : public InstructionStream
{
private:

	/// The list of instructions to be processed at update
	InstructionList m_PendingInstructions;

	/// The stream of recorded instructions
	std::ofstream m_OutFile;
	Serializer* m_pOutStream;

public:

	/// The default constructor
	InstructionStreamPlay() : m_pOutStream( 0 )
	{
	}

	/// The destructor
	~InstructionStreamPlay();

	void Init( bool recordGame );

	/// Add an instruction to handled at the next possible time
	virtual void AddPendingInstruction( GameFieldInstruction* pInst ){ m_PendingInstructions.push_back( pInst ); }

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
	virtual void ProcessHandledInstruction( GameFieldInstruction* pInst );

	/// Get if there are any tasks to be handled during the current game update
	virtual bool HasPendingInstructions() const { return m_PendingInstructions.size() > 0; }
};

#endif // __InstructionStreamPlay_h