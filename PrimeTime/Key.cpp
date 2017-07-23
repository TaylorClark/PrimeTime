#include "Base/Types.h"
#include <math.h>
#include <stdlib.h>

const uint32 NUM_INDICES = 13;

const uint32 FIXED_IDX[] = { 0, 2, 3, 7, 10, 11, 12 };

// Get on of the component values from a key
uint16 GetKeyItem( uint64 keyVal, uint32 valIndex )
{
	if( valIndex >= NUM_INDICES )
		return 0;

	const uint64 VAL_BITS = 5;
	uint16 retVal = (uint16)((keyVal >> (uint64)(VAL_BITS * valIndex)) & 31);
	if( valIndex == 12 )
		retVal = retVal & 15;

	return retVal;
}

bool Check1( uint64 keyVal )
{
	uint16 item1Val = GetKeyItem( keyVal, 1 );

	uint16 item5Val = GetKeyItem( keyVal, 11 );

	uint16 compVal = 0;
	if( item5Val > 15 )
		compVal = GetKeyItem( keyVal, FIXED_IDX[3] ) * 2;
	else
		compVal = GetKeyItem( keyVal, FIXED_IDX[6] ) * 2;
	compVal &= 31;

	if( item1Val != compVal )
		return false;

	return true;
}

bool Check4( uint64 keyVal )
{
	uint16 item4Val = GetKeyItem( keyVal, 4 );

	uint16 compVal = GetKeyItem( keyVal, FIXED_IDX[0] ) + GetKeyItem( keyVal, FIXED_IDX[3] );
	compVal &= 31;

	if( item4Val != compVal )
		return false;

	return true;
}

bool Check5( uint64 keyVal )
{
	uint16 item5Val = GetKeyItem( keyVal, 5 );

	uint16 compVal = GetKeyItem( keyVal, FIXED_IDX[6] ) % 8;
	compVal = compVal + GetKeyItem( keyVal, FIXED_IDX[1] );
	compVal &= 31;

	if( item5Val != compVal )
		return false;

	return true;
}

//bool Check6( uint64 keyVal )
//{
//	uint16 item6Val = GetKeyItem( keyVal, 6 );
//
//	uint16 compVal = GetKeyItem( keyVal, FIXED_IDX[2] );
//	compVal = ~compVal;
//	compVal &= 31;
//
//	if( item6Val != compVal )
//		return false;
//
//	return true;
//}

//const int32 FIXED_IDX[NUM_FIXED] = { 0, 2, 3, 7, 10, 11, 12 };

//bool Check8( uint64 keyVal )
//{
//	uint16 item8Val = GetKeyItem( keyVal, 8 );
//
//	/*
//	if( vals[ FIXED_IDX[5] ] % 4 == 0 )
//		vals[8] = vals[ FIXED_IDX[2] ] + vals[ FIXED_IDX[6] ];
//	else if( vals[ FIXED_IDX[5] ] % 3 == 0 )
//		vals[8] = vals[ FIXED_IDX[1] ] + vals[ FIXED_IDX[0] ];
//	else
//		vals[8] = vals[ FIXED_IDX[3] ] | 0x15;
//		*/
//	uint16 item11Val = GetKeyItem( keyVal, 11 );
//	uint16 compVal = 0;
//	if( item11Val % 4 == 0 )
//		compVal = GetKeyItem( keyVal, 3 ) + GetKeyItem( keyVal, 12 );
//	else if( item11Val % 3 == 0 )
//		compVal = GetKeyItem( keyVal, 2 ) + GetKeyItem( keyVal, 0 );
//	else
//		compVal = GetKeyItem( keyVal, 7 ) | 0x15;
//	compVal &= 31;
//
//	if( item8Val != compVal )
//		return false;
//
//	return true;
//}

//bool Check9( uint64 keyVal )
//{
//	uint16 item9Val = GetKeyItem( keyVal, 9 );
//
//	//vals[9] = (int)powf( (float)(vals[ FIXED_IDX[0] ] % 5), (float)((vals[ FIXED_IDX[1] ] + vals[ FIXED_IDX[2] ]) % 8) );
//
//	uint16 baseVal = GetKeyItem( keyVal, 0 ) % 5;
//	uint16 expVal = (GetKeyItem( keyVal, 2 ) + GetKeyItem( keyVal, 3 )) % 8;
//	uint16 compVal = (uint16)powf( (float)baseVal, (float)expVal );
//	compVal &= 31;
//
//	if( item9Val != compVal )
//		return false;
//
//	return true;
//}

/// Verify if a key value is a valid Prime Time key
uint32 AAAAAaaaaa( uint64 val )
{
	// Check values
	bool isValid = Check1( val );
	isValid = isValid || Check4( val );
	isValid = isValid || Check5( val );
	//isValid = isValid || Check6( val );
	//isValid = isValid || Check8( val );
	//isValid = isValid || Check9( val );

	// Set the 17th bit if the key is valid, the rest is merely to confuse
	uint32 retVal = 0x87329487;//(rand() << 16) | rand();
	if( isValid )
		retVal |= 0x00020000;
	else
		retVal &= ~0x00020000;

	return retVal;
}