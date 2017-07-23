//=================================================================================================
/*!
	\file IUpdatable.h
	Base Library
	Updatable Interface Header
	\author Taylor Clark
	\date May 13, 2006

	This header contains the interface definition for the updatable base class.
*/
//=================================================================================================

#pragma once
#ifndef __IUpdatable_h
#define __IUpdatable_h

#include "Base/Types.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class IUpdatable
	\brief Represents an object that can be updated and drawn.
*/
//-------------------------------------------------------------------------------------------------
class IUpdatable
{
public:

	/// Update the object
	virtual void Update( float32 frameTime ) = 0;

	/// Draw the object
	virtual void Draw() const = 0;
};

#endif // __IUpdatable_h