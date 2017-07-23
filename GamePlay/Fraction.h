#pragma once

class Fraction
{
public:
	Fraction() : Numerator(1),
					Denominator(1)
	{}

	Fraction(int32 num, int32 denom) : Numerator(num),
										Denominator(denom)
	{
	}

	Fraction(const Fraction& fraction, int scalar)
	{
		Numerator = fraction.Numerator * scalar;
		Denominator = fraction.Denominator * scalar;
	}

	int32 Numerator;
	int32 Denominator;
};