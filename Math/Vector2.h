//=============================================================================
/*!
	\file Vector2.h
	Taylor Clark Math Library
	2-Dimensional Vector
	By Taylor Clark

  This header contains the declaration for the 2D vector class.
*/
//=============================================================================

#pragma once

#ifndef __Vector2_h
#define __Vector2_h

#include "Base/Types.h"
#include <math.h>


/*=============================================================================

  Class Definitions

=============================================================================*/

const float32 V2_Epsilon = 0.00048828125f;


//-----------------------------------------------------------------------------
/*! 
	\class Vector2
	\brief This is the 2-Dimensional vector class.

	The 2-Dimensional vector class.
*/
//-----------------------------------------------------------------------------
class Vector2
{
public:
	float32 x, y;

public:

	//! Constructors
	Vector2() : x(0), y(0) { }
	Vector2( float32 nX, float32 nY ) : x(nX), y(nY) { }
	Vector2( const Vector2& rhs ) : x(rhs.x), y(rhs.y) { }
	const Vector2& operator =( const Vector2& rhs ){ x = rhs.x; y = rhs.y; return *this; }
	void Set( float32 nX, float32 nY ){ x = nX; y = nY; }

	//! Negatize the vector
	Vector2 operator -( void ) const { return Vector2( -x, -y ); }

	//! Equality Operators
	bool operator ==( const Vector2& rhs ) const
	{
		// See if the components are equal
		if( x == rhs.x && y == rhs.y )
			return true;

		return false;
	}
	bool operator !=( const Vector2& rhs ) const
	{
		// See if the components are equal
		if( x != rhs.x || y != rhs.y )
			return true;

		return false;
	}

	//! Nearly equals
	bool NearlyEquals( const Vector2& rhs ) const
	{
		float32 dotVal = Dot( rhs );
		
		// See if the vectors are close within a radius
		if( (dotVal < (GetMagSq() + V2_Epsilon))
			&& (dotVal > (GetMagSq() - V2_Epsilon)) )
			return true;

		return false;
	}

	//! Dot Product
	float32 Dot( const Vector2& rhs ) const { return ((x * rhs.x) + (y * rhs.y)); }

	//! Cross product.  Since we know this vector lies in 2-space, we know the
	//! cross product will give us a normal to that plane.  The return value is
	//! is the magnitude in that direction
	float32 Cross( const Vector2& rhs ) const { return ((x * rhs.y) - (rhs.x * y)); }

	//! Get a orthogonal vector in the clockwise and counter clockwise direction
	Vector2 GetOrthoN90( void ) const { return Vector2( y, -x ); }
	Vector2 GetOrtho90( void ) const { return Vector2( -y, x ); }

	//! Magnitude Functions
	float32 GetMag( void ) const { return sqrtf( (x * x) + (y * y) ); }
	float32 GetMagSq( void ) const { return ((x * x) + (y * y)); }

	//! Normal Functions
	Vector2 Normalized( void ) const
	{
		// Get the magnitude and ensure it is not 0
		Vector2 retV;
		float32 mag = GetMag();
		if( mag == 0.0f )
			return retV;

		// Get the reciprocal magnitude
		mag = 1.0f / mag;

		// Normalize the vector
		retV.x = x * mag;
		retV.y = y * mag;
		return retV;
	}
	void Normalize( void )
	{
		// Get the magnitude and ensure it is not 0
		float32 mag = GetMag();
		if( mag == 0.0f )
			return;

		// Get the reciprocal magnitude
		mag = 1.0f / mag;

		// Normalize the vector
		x *= mag;
		y *= mag;
	}

	// Project the vector
	Vector2 ProjectOnto( const Vector2& proj ) const
	{
		// If the magnitude is 0, bail
		if( proj.GetMagSq() == 0.0f )
			return Vector2();

		float32 scalar = Dot( proj ) / proj.GetMagSq();
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
		float32 tX = (cosVal * x) - (sinVal * y);
		y = (cosVal * y) + (sinVal * x);
		x = tX;
	}

	// Reflect a vector
	Vector2 ReflectAcrossVector( const Vector2& reflectVect ) const
	{
        return ( (reflectVect * 2 * this->Dot(reflectVect)) - *this );
	}

	// Arithmetic operators
	void operator += ( const Vector2& rhs ){ x += rhs.x; y += rhs.y; }
	Vector2 operator + ( const Vector2& rhs ) const { return Vector2( x + rhs.x, y + rhs.y ); }

	void operator -= ( const Vector2& rhs ){ x -= rhs.x; y -= rhs.y; }
	Vector2 operator - ( const Vector2& rhs ) const { return Vector2( x - rhs.x, y - rhs.y ); }

	void operator /= ( float32 den ){	x /= den; y /= den; }
	Vector2 operator / ( float32 den ) const { return Vector2( x / den, y / den ); }

	void operator *= ( float32 scalar ){ x *= scalar; y *= scalar; }
	Vector2 operator * ( float32 scalar ) const { return Vector2( x * scalar, y * scalar ); }
};


#endif //__Vector2_h