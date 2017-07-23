//=============================================================================
/*!
	\file CollisionCircle.h
	Die By The Cannon
	2x2 Matrix Header
	\author Taylor Clark

  This header contains the class declaration for the 2 by 2 matrix class.
*/
//=============================================================================

#pragma once

#ifndef __Matrix2x2_h
#define __Matrix2x2_h

#include "Base/Types.h"
#include "Point2.h"
#include "Vector2.h"


/*=============================================================================

  Class Definitions

=============================================================================*/

//-----------------------------------------------------------------------------
/*! 
	\class Matrix2x2
	\brief This is the 2x2 matrix class

	This class holds functionailty for a 2 column by 2 row matrix.
*/
//-----------------------------------------------------------------------------
class Matrix2x2
{
private:
	// The actual matrix data
	float32 m_fData[2][2];

public:
	// The constructor
	Matrix2x2();
	Matrix2x2( float32 f11, float32 f12,
				float32 f21, float32 f22 )
	{
		m_fData[0][0] = f11; m_fData[0][1] = f12;
		m_fData[1][0] = f21; m_fData[1][1] = f22;
	}
	const Matrix2x2& operator = ( const Matrix2x2& rhs )
	{
		m_fData[0][0] = rhs.m_fData[0][0];
		m_fData[0][1] = rhs.m_fData[0][1];
		m_fData[1][0] = rhs.m_fData[1][0];
		m_fData[1][1] = rhs.m_fData[1][1];
		return *this;
	}

	// Set all of the data
	void Set( float32 f11, float32 f12,
				float32 f21, float32 f22 )
	{
		m_fData[0][0] = f11; m_fData[0][1] = f12;
		m_fData[1][0] = f21; m_fData[1][1] = f22;
	}

	// Set an entry in the matrix
	void Set(uint32 row, uint32 col, float32 entryValue);

	// Get an entry from the matrix
	float32 Get(uint32 row, uint32 col) const;

	// Matrix multiplication
	Matrix2x2 Compose(const Matrix2x2 &multiplicandMatrix) const;
	Matrix2x2 operator *(const Matrix2x2 &rhsMtrx) const { return Compose(rhsMtrx); }

	// Transpose
	Matrix2x2 Transpose( void ) const;

	// Inverse
	Matrix2x2 Inverse( void ) const;

	// Calculate the determinate
	float32 Determinant( void ) const;

	// Calculate the trace of the matrix
	float32 Trace( void ) const;

	// Matrix vector multiplication
	Vector2 Compose( const Vector2 &multiplicandVector ) const;
	Vector2 operator *( const Vector2 &multiplicandVector ) const 
	{
		return Compose(multiplicandVector);
	}
	void Compose( float32 *xValP, float32 *yValP )
	{
		Vector2 tempVect(*xValP, *yValP);
		tempVect = Compose(tempVect);
		*xValP = tempVect.x;
		*yValP = tempVect.y;
	}

	Point2 Compose( const Point2 &multiplicandVector ) const;
	Point2 operator *(const Point2 &multiplicandVector) const
	{
		return Compose( multiplicandVector );
	}

	// Matrix construction from 3 vectors
	void ConstructFromRowVectors( const Vector2 vectors[2] );

	// Construct the rotation matrix for the coordinate axes
	void ConstructRotation( float32 fDegrees );
};

#endif // __Matrix2x2_h