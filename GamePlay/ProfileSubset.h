//=================================================================================================
/*!
	\file ProfileSubset.h
	Game Play Library
	Base Profile Subset Header
	\author Taylor Clark
	\date July 26, 2006

	This header contains the definition for the base player profile subset class.
*/
//=================================================================================================

#pragma once
#ifndef __ProfileSubset_h
#define __ProfileSubset_h

#include "Base/Serializer.h"
#include "Base/FourCC.h"


//-------------------------------------------------------------------------------------------------
/*!
	\class ProfileSubset
	\brief Represents the profile subset base class
*/
//-------------------------------------------------------------------------------------------------
class ProfileSubset
{
private:

public:

	/// The destructor, virtual to ensure that derived class are destructed properly
	virtual ~ProfileSubset(){}

	/// Get the FourCC for this subset
	virtual FourCC GetSubsetFourCC() const = 0;

	/// Load/save the subset data
	virtual bool Serialize( Serializer& serializer ) = 0;
};

#endif // __ProfileSubset_h