#include "../FractionModeSettings.h"
#include <stdarg.h>
#include <set>
#include <stdlib.h>

Fraction FractionModeSettings::GetRandomFraction( uint32 fractionIndex ) const
{
	Fraction simplifiedValue = AvailableFractions[ fractionIndex ].FractionValue;

	int32 maxScalar = MaxValue / simplifiedValue.Denominator;

	int32 scalar = (rand() % maxScalar) + 1;

	return Fraction( simplifiedValue, scalar );
}

void FractionModeSettings::InitValuesFromFractions(std::list<Fraction> fractions)
{
	for( std::list<Fraction>::iterator iterFraction = fractions.begin(); iterFraction != fractions.end(); ++iterFraction )
	{
		FractionInfo newInfo;

		newInfo.FractionValue = *iterFraction;

		// Find all of the multiples for this fraction up to the max value
		int scalar = 1;
		for(;;)
		{
			Fraction fractionMultiple( newInfo.FractionValue, scalar++ );

			// If we've reached our max then stop getting values
			if( fractionMultiple.Numerator >= MaxValue || fractionMultiple.Denominator >= MaxValue )
				break;

			// If we already know we're using this value then update the scalar
			if( newInfo.ValuesUpToMax.find(fractionMultiple.Numerator) != newInfo.ValuesUpToMax.end() )
				newInfo.ValueScalars[fractionMultiple.Numerator]++;
			else
			{
				newInfo.ValuesUpToMax.insert(fractionMultiple.Numerator);
				newInfo.ValueScalars[fractionMultiple.Numerator] = 1;
			}

			// Now for the denominator
			if( newInfo.ValuesUpToMax.find(fractionMultiple.Denominator) != newInfo.ValuesUpToMax.end() )
				newInfo.ValueScalars[fractionMultiple.Denominator]++;
			else
			{
				newInfo.ValuesUpToMax.insert(fractionMultiple.Denominator);
				newInfo.ValueScalars[fractionMultiple.Denominator] = 1;
			}
		}

		AvailableFractions.push_back( newInfo );
	}
}


FractionModeSettings::FractionModeSettings( int32 decimalPlaces, int32 exclusiveMaxValue, std::list<Fraction> fractions ) :
	DecimalPlaces( decimalPlaces ),
	MaxValue( exclusiveMaxValue )
{
	InitValuesFromFractions( fractions );
}