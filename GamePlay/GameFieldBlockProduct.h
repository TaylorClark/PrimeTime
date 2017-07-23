//=================================================================================================
/*!
	\file GameFieldBlockProduct.h
	Game Play Library
	Game Field Header
	\author Taylor Clark
	\date May 12, 2006

	This header contains the definition for the product game field block object.
*/
//=================================================================================================

#pragma once
#ifndef __GameFieldBlockProduct_h
#define __GameFieldBlockProduct_h

#include "GameFieldBlockMultBase.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class GameFieldBlockProduct
	\brief Represents a block in a multiplication game field.
*/
//-------------------------------------------------------------------------------------------------
class GameFieldBlockProduct : public GameFieldBlockMultBase
{
private:

	/// If this block is a product or a factor
	bool m_IsProduct;

public:

	/// The default constructor
	GameFieldBlockProduct( int32 value, bool isProduct ) : GameFieldBlockMultBase( value ),
															m_IsProduct( isProduct )
	{
		// Only products need text
		if( !m_IsProduct )
		{
			// Clear the text buffer
			for( int charIndex = 0; charIndex < TEXT_BUF_SIZE; ++charIndex )
				m_TextChars[ charIndex ] = 0;
		}
	}

	/// Get if this block is a product block
	bool IsProductBlock() const { return m_IsProduct; }
};

#endif // __GameFieldBlockProduct_h