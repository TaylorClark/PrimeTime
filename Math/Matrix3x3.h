/*====================================================

  Taylor Clark
  Matrix Class
  Matrix3x3.h
  Created : 2/7/02
  Last Modified : 2/7/02

  This header contains the class declaration for the
  3 by 3 matrix class.

====================================================*/

#ifndef __MATRIX3X3_H
#define __MATRIX3X3_H

#include "Base/Types.h"
#include "Vector3.h"
#include "Point2.h"
#include "Vector2.h"


////////////////////////////////////////////////////
// The Matrix Class
////////////////////////////////////////////////////
class Matrix3x3
{
private:
	// The actual matrix data
	float32 m_fData[3][3];

public:
	// The constructor
	Matrix3x3();
	Matrix3x3( float32 f11, float32 f12, float32 f13,
				float32 f21, float32 f22, float32 f23,
				float32 f31, float32 f32, float32 f33 )
	{
		m_fData[0][0] = f11; m_fData[0][1] = f12; m_fData[0][2] = f13;
		m_fData[1][0] = f21; m_fData[1][1] = f22; m_fData[1][2] = f23;
		m_fData[2][0] = f31; m_fData[2][1] = f32; m_fData[2][2] = f33;
	}

	// Set all of the data
	void Set( float32 f11, float32 f12, float32 f13,
				float32 f21, float32 f22, float32 f23,
				float32 f31, float32 f32, float32 f33 )
	{
		m_fData[0][0] = f11; m_fData[0][1] = f12; m_fData[0][2] = f13;
		m_fData[1][0] = f21; m_fData[1][1] = f22; m_fData[1][2] = f23;
		m_fData[2][0] = f31; m_fData[2][1] = f32; m_fData[2][2] = f33;
	}

	// Set an entry in the matrix
	void Set(uint32 rowNumber, uint32 columnNumber, float32 entryValue);

	// Get an entry from the matrix
	float32 Get(uint32 rowNumber, uint32 columnNumber) const;

	// Matrix multiplication
	Matrix3x3 Compose(const Matrix3x3 &multiplicandMatrix) const;
	Matrix3x3 operator *(const Matrix3x3 &rhsMtrx) const { return Compose(rhsMtrx); }

	// Transpose
	Matrix3x3 Transpose(void) const;

	// Calculate the determinate
	float32 Determinant(void) const;

	// Calculate the trace of the matrix
	float32 Trace(void) const;

	// Get the inverse
	Matrix3x3 Inverse( void ) const;

	// Add matrices
	void operator += ( const Matrix3x3& addMat );

	// Matrix coeffiencient multiply
	void Scale( float32 scalar );
	void operator *= ( float32 scalar ){ Scale( scalar ); }

	// Create the matrix from a vector using the tilde operator
	void TildeVector( const Vector3& vect );

	// Matrix vector multiplication
	Vector3 Compose(const Vector3 &multiplicandVector) const;
	Vector3 operator *(const Vector3 &multiplicandVector) const 
	{
		return Compose(multiplicandVector);
	}
	void Compose(float32 *xValP, float32 *yValP, float32 *zValP)
	{
		Vector3 tempVect(*xValP, *yValP, *zValP);
		tempVect = Compose(tempVect);
		*xValP = tempVect.x;
		*yValP = tempVect.y;
		*zValP = tempVect.z;
	}

	Vector2 Compose( const Vector2 &multiplicandVector ) const;
	Vector2 operator *(const Vector2 &multiplicandVector) const
	{
		return Compose(multiplicandVector);
	}

	Point2 Compose( const Point2 &multiplicandVector ) const;
	Point2 operator *(const Point2 &multiplicandVector) const
	{
		return Compose( multiplicandVector );
	}

	// Matrix construction from 3 vectors
	void ConstructFromRowVectors(const Vector3 vectors[3]);

	// Construct the rotation matrix for the coordinate axes
	void ConstructRotationAboutXAxis(float32 fDegrees);
	void ConstructRotationAboutYAxis(float32 fDegrees);
	void ConstructRotationAboutZAxis(float32 fDegrees);

	// Construction of a rotation matrix for an arbitrary axis
	void ConstructRotationFromVector(const Vector3 &axisVector, float32 fDegrees);
};

#endif

