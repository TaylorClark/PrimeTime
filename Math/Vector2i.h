//=============================================================================
/*!
	\file Vector2i.h
	Taylor Clark Math Library
	2-Dimensional Integer Vector
	By Taylor Clark

  This header contains the declaration for the 2D integer vector class.
*/
//=============================================================================

#pragma once

#ifndef __Vector2i_h
#define __Vector2i_h

#include "Base/Types.h"
#include <math.h>


/*=============================================================================

  Class Definitions

=============================================================================*/

//-----------------------------------------------------------------------------
/*! 
	\class Vector2ii
	\brief This is the 2-Dimensional vector class with integer components.

	The 2-Dimensional vector class.
*/
//-----------------------------------------------------------------------------
class Vector2i
{
public:
	int32 x, y;

public:

	//! Constructors
	Vector2i() : x(0), y(0) { }
	Vector2i( int32 nX, int32 nY ) : x(nX), y(nY) { }
	Vector2i( const Vector2i& rhs ) : x(rhs.x), y(rhs.y) { }
	const Vector2i& operator =( const Vector2i& rhs ){ x = rhs.x; y = rhs.y; return *this; }
	void Set( int32 nX, int32 nY ){ x = nX; y = nY; }

	//! Negatize the vector
	Vector2i operator -( void ) const { return Vector2i( -x, -y ); }

	//! Equality Operators
	bool operator ==( const Vector2i& rhs ) const
	{
		// See if the components are equal
		if( x == rhs.x && y == rhs.y )
			return true;

		return false;
	}
	bool operator !=( const Vector2i& rhs ) const
	{
		// See if the components are equal
		if( x != rhs.x || y != rhs.y )
			return true;

		return false;
	}

	//! Dot Product
	int32 Dot( const Vector2i& rhs ) const { return ((x * rhs.x) + (y * rhs.y)); }

	//! Cross product.  Since we know this vector lies in 2-space, we know the
	//! cross product will give us a normal to that plane.  The return value is
	//! is the magnitude in that direction
	int32 Cross( const Vector2i& rhs ) const { return ((x * rhs.y) - (rhs.x * y)); }

	//! Get a orthogonal vector in the clockwise and counter clockwise direction
	Vector2i GetOrthoN90( void ) const { return Vector2i( y, -x ); }
	Vector2i GetOrtho90( void ) const { return Vector2i( -y, x ); }

	//! Magnitude Functions
	int32 GetMag( void ) const { return (int32)sqrtf( (float32)((x * x) + (y * y)) ); }
	int32 GetMagSq( void ) const { return ((x * x) + (y * y)); }

	//! Normal Functions
	Vector2i Normalized( void ) const
	{
		Vector2i retV;
		
		// Get the magnitude and ensure it is not 0
		float32 mag = (float32)GetMag();
		if( mag == 0.0f )
			return retV;

		// Get the reciprocal magnitude
		mag = 1.0f / mag;

		// Normalize the vector
		retV.x = (int32)( (float32)x * mag );
		retV.y = (int32)( (float32)y * mag );
		return retV;
	}
	void Normalize( void )
	{
		// Get the magnitude and ensure it is not 0
		float32 mag = (float32)GetMag();
		if( mag == 0.0f )
			return;

		// Get the reciprocal magnitude
		mag = 1.0f / mag;

		// Normalize the vector
		x = (int32)((float32)x * mag);
		y = (int32)((float32)y * mag);
	}

	// Project the vector
	Vector2i ProjectOnto( const Vector2i& proj ) const
	{
		// If the magnitude is 0, bail
		if( proj.GetMagSq() == 0.0f )
			return Vector2i();

		int32 scalar = Dot( proj ) / proj.GetMagSq();
		return (proj * scalar);
	}

	//! Rotate the vector about its origin
	void RotateDeg( float32 degrees ){ RotateRad( degrees * 0.01745329252f ); }

	// Convert the degrees to radians by multiplying PI/180
	void RotateRad( float32 radians )
	{
		// Calculate the sine and cosine
		float32 cosVal = cosf( radians );
		float32 sinVal = sinf( radians );

		// Rotate the point
		float32 deltaX = (cosVal * x) - (sinVal * y);
		y = (int32)( (cosVal * y) + (sinVal * x) );
		x = (int32)deltaX;
	}

	// Reflect a vector
	Vector2i ReflectAcrossVector( const Vector2i& reflectVect ) const
	{
        return ( (reflectVect * 2 * this->Dot(reflectVect)) - *this );
	}

	// Arithmetic operators
	void operator += ( const Vector2i& rhs ){ x += rhs.x; y += rhs.y; }
	Vector2i operator + ( const Vector2i& rhs ) const { return Vector2i( x + rhs.x, y + rhs.y ); }

	void operator -= ( const Vector2i& rhs ){ x -= rhs.x; y -= rhs.y; }
	Vector2i operator - ( const Vector2i& rhs ) const { return Vector2i( x - rhs.x, y - rhs.y ); }

	void operator /= ( int32 den ){	x /= den; y /= den; }
	Vector2i operator / ( int32 den ) const { return Vector2i( x / den, y / den ); }

	void operator *= ( int32 scalar ){ x *= scalar; y *= scalar; }
	Vector2i operator * ( int32 scalar ) const { return Vector2i( x * scalar, y * scalar ); }
	
	void operator *= ( float32 scalar )
	{
		x = (int32)((float32)x * scalar);
		y = (int32)((float32)y * scalar);
	}
	Vector2i operator * ( float32 scalar ) const
	{
		return Vector2i( (int32)((float32)x * scalar), (int32)((float32)y * scalar) );
	}
};


#endif // __Vector2i_h