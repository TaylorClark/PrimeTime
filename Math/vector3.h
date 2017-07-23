//=============================================================================
/*!
	\file Vector3.h
	Taylor Clark Math Library
	3-Dimensional Vector Header
	\author Taylor Clark
	\data Create January 18, 2002

  This header contains the class declaration for a 3-D vector.
*/
//=============================================================================

#ifndef __Vector3_h
#define __Vector3_h

#include <math.h>


const float V3_Epsilon = 0.00048828125f;

//-----------------------------------------------------------------------------
/*! 
	\class Vector3
	\brief This is the 3-Dimensional vector class.

	The 3-Dimensional vector class.
*/
//-----------------------------------------------------------------------------
class Vector3
{
public:
	float x, y, z;

public:
	// The constructor
	Vector3(float fX, float fY, float fZ)
		:	x( fX ),
			y( fY ),
			z( fZ )
	{
	}
	Vector3()
		:	x( 0.0f ),
			y( 0.0f ),
			z( 0.0f )
	{
	}
	void Set( float fX, float fY, float fZ ){ x = fX; y = fY; z = fZ; }

	// The assignment (equal) operator
	const Vector3& operator =(const Vector3 &rhsVector)
	{
		x = rhsVector.x; y = rhsVector.y; z = rhsVector.z;
		return *this;
	}

	// The copy constructor
	Vector3(const Vector3 &copyVector)
		: x( copyVector.x ),
			y( copyVector.y ),
			z( copyVector.z )
	{
	}
	
	//! Equality Operators
	bool operator ==( const Vector3& rhs ) const
	{
		// See if the components are equal
		if( x == rhs.x && y == rhs.y && z == rhs.z )
			return true;

		return false;
	}
	bool operator !=( const Vector3& rhs ) const
	{
		// See if the components are equal
		if( x != rhs.x || y != rhs.y || z != rhs.z )
			return true;

		return false;
	}

	//! Nearly equals
	bool NearlyEquals( const Vector3& rhs ) const
	{
		float dotVal = Dot( rhs );
		
		// See if the vectors are close within a radius
		if( (dotVal < (GetMagSq() + V3_Epsilon))
			&& (dotVal > (GetMagSq() - V3_Epsilon)) )
			return true;

		return false;
	}

	// Get the length
	float GetMag() const
	{
		return sqrtf((x * x) + (y * y) + (z * z));
	}
	float GetMagSq() const
	{
		return ((x * x) + (y * y) + (z * z));
	}

	// Vector addition
	Vector3 operator +(const Vector3 &rhsVector) const
	{
		return Vector3( (x + rhsVector.x),
					(y + rhsVector.y),
					(z + rhsVector.z) );
	}

	// Vector addition assignment
	const Vector3 &operator +=(const Vector3 &rhsVector)
	{
		x += rhsVector.x;
		y += rhsVector.y;
		z += rhsVector.z;
		return *this;
	}

	// Vector subtraction
	Vector3 operator -(const Vector3 &rhsVector) const
	{
		return Vector3( (x - rhsVector.x),
					(y - rhsVector.y),
					(z - rhsVector.z) );
	}
	
	// Vector subtraction assignment
	const Vector3 operator -=(const Vector3 &rhsVector)
	{
		x -= rhsVector.x;
		y -= rhsVector.y;
		z -= rhsVector.z;
		return *this;
	}

	// Project the vector
	Vector3 ProjectOnto( const Vector3& proj ) const
	{
		// If the magnitude is 0, bail
		if( proj.GetMagSq() == 0.0f )
			return Vector3();

		float scalar = Dot( proj ) / proj.GetMagSq();
		return (proj * scalar);
	}

	// Scalar multiplication
	Vector3 operator *(float fScalar) const
	{
		return Vector3(x * fScalar, y * fScalar, z * fScalar);
	}

	// Scalar multiplication assignment
	const Vector3& operator *=(float fScalar)
	{
		x *= fScalar;
		y *= fScalar;
		z *= fScalar;
		return *this;
	}

	// Scalar division
	Vector3 operator /(float fScalar) const
	{
		return Vector3(x / fScalar, y / fScalar, z / fScalar);
	}

	// Scalar division assignment
	const Vector3& operator /=(float fScalar)
	{
		x /= fScalar;
		y /= fScalar;
		z /= fScalar;
		return *this;
	}

	// The negate operator
	Vector3 operator -(void) const
	{
		return Vector3(-x, -y, -z);
	}

	// Normalization of the vector
	void Normalize()
	{
		float fMagnitude = GetMag();
		x /= fMagnitude;
		y /= fMagnitude,
		z /= fMagnitude;
	}
	Vector3 Normalized() const
	{
		float fMagnitude = GetMag();
		Vector3 normalledVect(x / fMagnitude,
							y / fMagnitude,
							z / fMagnitude);
		return normalledVect;
	}

	// Dot Product
	float Dot(const Vector3 &dotVector) const
	{
		return ((x * dotVector.x) + (y * dotVector.y) + (z * dotVector.z));
	}

	// Cross Product
	Vector3 Cross(const Vector3 &crossVector) const
	{
		Vector3 returnVector;
		returnVector.x = (y * crossVector.z) - (z * crossVector.y);
		returnVector.y = (z * crossVector.x) - (x * crossVector.z);
		returnVector.z = (x * crossVector.y) - (y * crossVector.x);
		return returnVector;
	}

	// Reflect a vector
	Vector3 ReflectAcrossVector( const Vector3& reflectVect ) const
	{
        return ( (reflectVect * 2 * this->Dot(reflectVect)) - *this );
	}

	// If this is a zero vector
	bool IsZeroVector( void ) const
	{
		return (x == 0.0f) && (y == 0.0f) && (z == 0.0f);
	}

	// Get a random orthogonal vector
	Vector3 GetRandomOrthoVector( void ) const
	{
		// If we have a 0 vector
		if( IsZeroVector() )
			return Vector3( 0, 0, 0 );

		// Get a random vector from this vector
		Vector3 randVect( z, x, y );

		// Cross the random vector with this vector
		Vector3 retVect( Cross(randVect) );

		// Ensure it is orthogonal
		float dotVal = Dot( retVect );
		if( (dotVal != 0.0f) || retVect.IsZeroVector() )
		{
			// Try crossing with another vector
			randVect.Set( -x, -y, z );
			retVect = Cross(randVect);
			dotVal = Dot( retVect );

			if( (dotVal != 0.0f) || retVect.IsZeroVector() )
			{
				// Try crossing with another vector
				randVect.Set( x, y, -z );
				retVect = Cross(randVect);
				// Just use the vector at this point
			}
		}

		// Return the vector
		return retVect;
	}
};

#endif // __Vector3_h