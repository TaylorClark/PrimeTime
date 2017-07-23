/*=================================================================================================

	\file Main.cpp
	Prime Number Generator
	Main Source
	\author Taylor Clark
	\Date June 27, 2006

	This source file contains the code to generate a list of prime numbers.

=================================================================================================*/

#include <iostream>
#include <fstream>
#include <list>
#include "Base/Types.h"
#include "Base/NetSafeSerializer.h"

typedef uint32 NumType;
typedef std::list<NumType> NumList;

typedef uint8 PrimeType;
typedef std::list<PrimeType> PrimeList;

typedef std::pair<NumType,PrimeList> ProductPair;
typedef std::list< ProductPair > ProductList;


/// The array or available primes
const NumType PRIMES[] = {2,3,5,7,11,13,17,19};
const uint32 NUM_PRIMES = sizeof(PRIMES) / sizeof(PRIMES[0]);

/// Get the list of prime factors for a number
PrimeList GetPrimeFactors( NumType product );


int PrimeToIndex( int primeValue )
{
	for( int curPrimeIndex = 0; curPrimeIndex < NUM_PRIMES; ++curPrimeIndex )
	{
		if( PRIMES[curPrimeIndex] == primeValue )
			return curPrimeIndex;
	}

	return -1;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	Main  Global
///
///	The entry point for the application.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
int main( int argc, char* argv[] )
{	
	/*PrimeList factorList = GetPrimeFactors( 104 );
	
	std::cout << "The prime factors of 104 are: ";
	for( PrimeList::iterator iterFactor = factorList.begin(); iterFactor != factorList.end(); ++iterFactor )
	{
		if( iterFactor != factorList.begin() )
			std::cout << ",";
		std::cout << (int)*iterFactor;
	}
	std::cout << std::endl;
	return 0;*/

	const NumType MAX_PRODUCT = 2000;

	std::ofstream outFile( "output.txt" );
	std::ostream& outStream = outFile;

	std::ofstream jsFile( "Products.js" );
	jsFile << "var Products=new Array(";// << std::endl;

	// Go through the desired products and find the factors
	ProductList productsList;
	for( NumType curProduct = 1; curProduct < MAX_PRODUCT; ++curProduct )
	{
		PrimeList factorList = GetPrimeFactors( curProduct );
		if( factorList.size() == 0 )
			continue;

		if( curProduct > 4 )
			jsFile << ",";// << std::endl;

		jsFile << "{V:" << curProduct << ",F:[";

		outStream << "The prime factors of " << curProduct << " are: ";
		int usedPrimeFlags = 0;
		for( PrimeList::iterator iterFactor = factorList.begin(); iterFactor != factorList.end(); ++iterFactor )
		{
			if( iterFactor != factorList.begin() )
			{
				outStream << ",";
				jsFile << ",";
			}

			outStream << (int)*iterFactor;
			jsFile << (int)*iterFactor;
			
			// Keep track of which primes are used by this product
			usedPrimeFlags |= (1 << PrimeToIndex( (int)*iterFactor ));
		}

		outStream << std::endl;
		jsFile << "],UPF:" << usedPrimeFlags << "}";

		productsList.push_back( std::make_pair( curProduct, factorList ) );
	}

	outStream << "There were " << productsList.size() << " products generated." << std::endl;
	outFile.close();

	jsFile << ");" << std::endl;
	jsFile.close();

	// Open the binary file
	outFile.open( "numbers.rdb", std::ios_base::out | std::ios_base::binary );

	NetSafeSerializer serializer( &outFile );

	// Write out the number of products
	uint32 numProducts = (unsigned int)productsList.size();
	serializer.AddData( numProducts );

	// Go through all of the products and write them out
	for( ProductList::iterator iterProd = productsList.begin(); iterProd != productsList.end(); ++iterProd )
	{
		// Write out the product
		NumType productVal = iterProd->first;
		serializer.AddData( productVal );

		// Write out the number of factors
		uint8 numFactors = (unsigned char)iterProd->second.size();
		serializer.AddData( numFactors );

		// Write out the prime factors
		for( PrimeList::const_iterator iterFactor = iterProd->second.begin(); iterFactor != iterProd->second.end(); ++iterFactor )
		{
			PrimeType curPrimeFactor = *iterFactor;
			serializer.AddData( curPrimeFactor );
		}
	}

	// Close the number file
	outFile.close();

	return 0;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
//	GetPrimeFactors  Global
///
///	\param product The product for which we are finding the prime factors
///	\returns A list of factors or an empty list if the product is not valid
///
///	Calculate the prime factors for a number.  This function uses the global array of primes
///	stored in the PRIMES variable. The return value is an empty list if it the product is prime or
///	it contains a prime factor beyond the last prime factor in PRIMES.
///
///////////////////////////////////////////////////////////////////////////////////////////////////
PrimeList GetPrimeFactors( NumType product )
{
	PrimeList retFactors;

	// Go through the primes
	NumType curVal = product;
	const NumType LAST_PRIME = PRIMES[ NUM_PRIMES - 1 ];
	uint32 curPrimeIndex = 0;
	while( curVal != 1 && curPrimeIndex < NUM_PRIMES )
	{
		// If this prime is not a factor of the current value
		if( (curVal % PRIMES[curPrimeIndex]) != 0 )
		{
			++curPrimeIndex;
			continue;
		}

		// Divide the value by the factor
		curVal /= PRIMES[curPrimeIndex];

		// Store the factor
		retFactors.push_back( (PrimeType)PRIMES[curPrimeIndex] );

		// Start back at the first prime
		curPrimeIndex = 0;
	}

	// If the test value is not one then this number can not be used either becuase it is prime
	// or it contains a prime factor beyond our last prime factor
	if( curVal != 1 || retFactors.size() == 1 || retFactors.size() > 60 )
		return PrimeList();

	// Otherwise return our list of prime factors
	return retFactors;
}