#include "../GFIRemoveBlocks.h"
#include "../GFIAddSummands.h"
#include "../GameFieldBlock.h"

#include "../GFIAddRow.h"
#include "../PIIToggleBlockSelect.h"
#include "../PIIClearSelection.h"
#include "../PIISetPushSpeed.h"
#include "../GFISetSum.h"

GameFieldInstruction* GameFieldInstruction::CreateInstruction( EGameFieldInstruction instTypeID )
{
	switch( instTypeID )
	{
	case GFI_AddRow:
		return 0;

	case GFI_RemoveBlocks:
		return new GFIRemoveBlocks();

	case GFI_SelectBlock:
		return new PIIToggleBlockSelect();
	
	case GFI_ClearSelection:
		return new PIIClearSelection();

	case GFI_SetPushSpeed:
		return new PIISetPushSpeed();

	case GFI_EndBlockFall:
		return 0;

	case GFI_SetSum:
		return new GFISetSum();

	case GFI_AddSummands:
		return new GFIAddSummands();
	}

	return 0;
}


/// The constructor to initialize the instruction to remove multiple blocks
GFIRemoveBlocks::GFIRemoveBlocks( std::list<GameFieldBlock*> blocks )
{
	for( std::list<GameFieldBlock*>::iterator iterBlock = blocks.begin();
			iterBlock != blocks.end();
			++iterBlock )
	{
		m_RemovedBlockIDs.push_back( (*iterBlock)->GetID() );
	}		
}

/// The constructor to initialize the instruction to remove a block
GFIRemoveBlocks::GFIRemoveBlocks( GameFieldBlock* pBlock )
{
	m_RemovedBlockIDs.push_back( pBlock->GetID() );
}

void GFIRemoveBlocks::SubclassSerialize( Serializer& serializer )
{
	// Write the number of blocks
	int numBlocks = (int)m_RemovedBlockIDs.size();
	serializer.AddData( numBlocks );

	if( serializer.InWriteMode() )
	{
		// Go through the removed blocks
		for( std::list<uint32>::iterator iterBlockID = m_RemovedBlockIDs.begin();
				iterBlockID != m_RemovedBlockIDs.end();
				++iterBlockID )
		{
			uint32 blockID = *iterBlockID;
			serializer.AddData( blockID );
		}
	}
	// Else read the block IDs
	else
	{
		m_RemovedBlockIDs.clear();
		uint32 blockID = 0;
		for( int blockIndex = 0; blockIndex < numBlocks; ++numBlocks )
		{
			serializer.AddData( blockID );

			m_RemovedBlockIDs.push_back( blockID );
		}
	}
}

/// Serialize the instruction to a memory stream
void GFIAddSummands::SubclassSerialize( Serializer& serializer )
{
	serializer.AddListData( m_SummandValues );

	//// Write the number of blocks
	//int numBlocks = (int)m_SummandValues.size();
	//serializer.AddData( numBlocks );

	//// If this instruction is being written out
	//if( serializer.InWriteMode() )
	//{
	//	// Go through the removed blocks
	//	for( std::list<int32>::iterator iterSummandValue = m_SummandValues.begin();
	//			iterSummandValue != m_SummandValues.end();
	//			++iterSummandValue )
	//	{
	//		int32 blockID = *iterSummandValue;
	//		serializer.AddData( blockID );
	//	}
	//}
	//// Else read the summand values
	//else
	//{
	//	m_SummandValues.clear();
	//	int32 summandValue = 0;
	//	
	//	for( int32 blockIndex = 0; blockIndex < numBlocks; ++numBlocks )
	//	{
	//		serializer.AddData( summandValue );

	//		m_SummandValues.push_back( summandValue );
	//	}
	//}
}