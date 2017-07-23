//=================================================================================================
/*!
	\file FractionModeSettings.h
	Game Play Library
	Fraction Settings Header
	\author Taylor Clark
	\date January 14, 2010

	This header contains the definition for the fraction mode settings object which stores the data
	used to define a fraction difficulty mode's parameters.
*/
//=================================================================================================

#pragma once
#ifndef __FractionModeSettings_h
#define __FractionModeSettings_h

#include "Base/Types.h"
#include <vector>
#include <list>
#include <map>
#include <set>
#include "Fraction.h"

class FractionModeSettings
{
public:
	struct FractionInfo
	{
		Fraction FractionValue;

		/// The values used by this fraction and all equivalent fractions up to the difficulty's
		/// maximum value
		std::set<int32> ValuesUpToMax;

		/// The number of times each fraction value is used
		std::map<int32, uint8> ValueScalars;
	};

	/// The fractions that can be presented to the user
	std::vector<FractionInfo> AvailableFractions;

	/// The numbers actually used by the fractions in this mode. For example, 11 and 13 would not
	/// be in this array as they aren't used by any fractions (Unless, of course, the difficulty
	/// uses fractions containing an 11 or 13)
	//std::vector<int32> _fractionValues;

	int32 DecimalPlaces;

	/// The maximum value, exclusive, generated for a block value
	int32 MaxValue;

	void InitValuesFromFractions( std::list<Fraction> fractions );

public:

	FractionModeSettings() : DecimalPlaces(1), MaxValue(1)
	{}

	FractionModeSettings( int32 decimalPlaces, int32 exclusiveMaxValue, std::list<Fraction> fractions );

	Fraction GetRandomFraction( uint32 fractionIndex ) const;
};

#endif // __FractionModeSettings_h