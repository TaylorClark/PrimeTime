//=============================================================================
/*!
	\file Matrix3x3.cpp
	Taylor Clark Math Library
	3x3 Matrix Class
	\author Taylor Clark

  This source file contains the implementation for the 3x3 matrix class.
*/
//=============================================================================

#include "../Matrix3x3.h"
#include "../Vector3.h"
#include <math.h>


const float32 PI_OVER_180 = 3.14159265f / 180.0f;


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix3x3::Matrix3x3( ) Public
/*!
//	This constructor for the matrix class clears the matrix with 0's.
*/
///////////////////////////////////////////////////////////////////////////////
Matrix3x3::Matrix3x3()
{
	// Set the data to 0
	for(uint32 index = 0; index < 3; ++index)
	{
		// Set this entry to 0
		m_fData[index][0] = 0;
		m_fData[index][1] = 0;
		m_fData[index][2] = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix3x3::SetEntry( ) Public
/*!
//	\param rowNumber The row index to set
//	\param columnNumber The column index to set
//	\param entryValue The value to set
//
//	This function sets one of the entries in the matrix to a passed in value.
*/
///////////////////////////////////////////////////////////////////////////////
void Matrix3x3::Set( uint32 rowNumber,
							uint32 columnNumber,
							float32 entryValue )
{
	// Ensure the row and column are valid
	if((rowNumber >= 3) || (columnNumber >= 3))
	{
		// The row number is too great so exit this function
		return;
	}

	// Set the value in the matrix
	m_fData[rowNumber][columnNumber] = entryValue;
}


/***********************************************

  Matrix3x3::GetEntry()

  This function returns an entry from the matrix.

  Returns the value at the indices passed in.

***********************************************/
float32 Matrix3x3::Get
(
	uint32 rowNumber,						// In : The row index to set
	uint32 columnNumber						// In : The column index to set
) const
{
	// Ensure the row and column are valid
	if((rowNumber >= 3) || (columnNumber >= 3))
	{
		// The row number is too great so exit this function
		return 0;
	}

	// Return the value in the matrix
	return m_fData[rowNumber][columnNumber];
}


/***********************************************

  Matrix3x3::Compose()

  This function multiplies this matrix by the passed
  in matrix and returns the result.

  Returns the resultant matrix.

***********************************************/
Matrix3x3 Matrix3x3::Compose
(
	const Matrix3x3 &multiplicandMatrix			// In : The matrix to multiply by
) const
{
	Matrix3x3 returnMatrix;
/*
	// Multiply the matrices together
	// Loop through all of the rows
	for(uint32 currentRow = 0; currentRow < 3; ++currentRow)
	{
		// Loop through all of the columns
		for(uint32 currentColumn = 0; currentColumn < 3; ++currentColumn)
		{
			// Multiply the row and column
			uint32 curMultRow = 0, curMultColumn = 0;
			float32 currentEntrySum = 0;
			while(curMultColumn < 3)
			{
				// Get the product and add it to the sum
				currentEntrySum += m_fData[currentRow][curMultColumn]
						* multiplicandMatrix.m_fData[curMultRow][currentColumn];

				// Increment the current row and column
				++curMultRow;
				++curMultColumn;

			} // End of row and column multiplication

			// Set this entry with the calculated value
			returnMatrix.m_fData[currentRow][currentColumn] = currentEntrySum;

		} // End of column loop

	} // End of row loop
*/
	// Multiply the rows and columns
	// Row 1
	returnMatrix.m_fData[0][0] = (m_fData[0][0] * multiplicandMatrix.m_fData[0][0])
								+ (m_fData[0][1] * multiplicandMatrix.m_fData[1][0])
								+ (m_fData[0][2] * multiplicandMatrix.m_fData[2][0]);

	returnMatrix.m_fData[0][1] = (m_fData[0][0] * multiplicandMatrix.m_fData[0][1])
								+ (m_fData[0][1] * multiplicandMatrix.m_fData[1][1])
								+ (m_fData[0][2] * multiplicandMatrix.m_fData[2][1]);

	returnMatrix.m_fData[0][2] = (m_fData[0][0] * multiplicandMatrix.m_fData[0][2])
								+ (m_fData[0][1] * multiplicandMatrix.m_fData[1][2])
								+ (m_fData[0][2] * multiplicandMatrix.m_fData[2][2]);

	// Row 2
	returnMatrix.m_fData[1][0] = (m_fData[1][0] * multiplicandMatrix.m_fData[0][0])
								+ (m_fData[1][1] * multiplicandMatrix.m_fData[1][0])
								+ (m_fData[1][2] * multiplicandMatrix.m_fData[2][0]);

	returnMatrix.m_fData[1][1] = (m_fData[1][0] * multiplicandMatrix.m_fData[0][1])
								+ (m_fData[1][1] * multiplicandMatrix.m_fData[1][1])
								+ (m_fData[1][2] * multiplicandMatrix.m_fData[2][1]);

	returnMatrix.m_fData[1][2] = (m_fData[1][0] * multiplicandMatrix.m_fData[0][2])
								+ (m_fData[1][1] * multiplicandMatrix.m_fData[1][2])
								+ (m_fData[1][2] * multiplicandMatrix.m_fData[2][2]);

	// Row 3
	returnMatrix.m_fData[2][0] = (m_fData[2][0] * multiplicandMatrix.m_fData[0][0])
								+ (m_fData[2][1] * multiplicandMatrix.m_fData[1][0])
								+ (m_fData[2][2] * multiplicandMatrix.m_fData[2][0]);

	returnMatrix.m_fData[2][1] = (m_fData[2][0] * multiplicandMatrix.m_fData[0][1])
								+ (m_fData[2][1] * multiplicandMatrix.m_fData[1][1])
								+ (m_fData[2][2] * multiplicandMatrix.m_fData[2][1]);

	returnMatrix.m_fData[2][2] = (m_fData[2][0] * multiplicandMatrix.m_fData[0][2])
								+ (m_fData[2][1] * multiplicandMatrix.m_fData[1][2])
								+ (m_fData[2][2] * multiplicandMatrix.m_fData[2][2]);

	// Return the matrix
	return returnMatrix;
}


/***********************************************

  Matrix3x3::Transpose()

  This function calculates this matrix's transpose.

  Returns the resultant matrix.

***********************************************/
Matrix3x3 Matrix3x3::Transpose( void ) const
{
	// Create the matrix with the rows and columns reversed from this class
	Matrix3x3 retTransposeMatrix;

	// Fill in the matrix entries
	for(uint32 currentRow = 0; currentRow < 3; ++currentRow)
	{
		// Loop through the columns
		for(uint32 currentColumn = 0; currentColumn < 3; ++currentColumn)
		{
			// Set the entries in the new matrix
			retTransposeMatrix.Set(currentColumn,
										currentRow,
										m_fData[currentRow][currentColumn]);
		}
	}

	// Return the transposed matrix
	return retTransposeMatrix;
}


/***********************************************

  Matrix3x3::Determinant()

  This function calculates the determinate and
  returns it.

  Returns the determinant, 0 if there was any
  problem.

***********************************************/
float32 Matrix3x3::Determinant() const
{
	float32 determinant;

	// Get the first part
	determinant = m_fData[0][0] * ((m_fData[1][1] * m_fData[2][2])
														- (m_fData[2][1]*m_fData[1][2]));

	// Subtract the second part
	determinant -= m_fData[0][1] * ((m_fData[1][0] * m_fData[2][2])
														- (m_fData[2][0]*m_fData[1][2]));

	// Add the third part
	determinant += m_fData[0][2] * ((m_fData[1][0] * m_fData[2][1])
														- (m_fData[2][0]*m_fData[1][1]));

	// Return the determinant
	return determinant;
}


/***********************************************

  Matrix3x3::Trace()

  This function calculates the of the matrix.

  Returns the trace of the matrix which is the
  sum of the diagonals.

***********************************************/
float32 Matrix3x3::Trace(void) const
{
	return m_fData[0][0] + m_fData[1][1] + m_fData[2][2];
}


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix3x3::Inverse( ) Public
/*!
//	Get the inverse of this matrix.
*/
///////////////////////////////////////////////////////////////////////////////
Matrix3x3 Matrix3x3::Inverse( void ) const
{
	// Invert a 3x3 using cofactors.  This is faster than using a generic
    // Gaussian elimination because of the loop overhead of such a method.
    Matrix3x3 retMat;

    retMat.m_fData[0][0] = m_fData[1][1] * m_fData[2][2] - m_fData[1][2] * m_fData[2][1];
    retMat.m_fData[0][1] = m_fData[0][2] * m_fData[2][1] - m_fData[0][1] * m_fData[2][2];
    retMat.m_fData[0][2] = m_fData[0][1] * m_fData[1][2] - m_fData[0][2] * m_fData[1][1];
    
	retMat.m_fData[1][0] = m_fData[1][2] * m_fData[2][0] - m_fData[1][0] * m_fData[2][2];
    retMat.m_fData[1][1] = m_fData[0][0] * m_fData[2][2] - m_fData[0][2] * m_fData[2][0];
    retMat.m_fData[1][2] = m_fData[0][2] * m_fData[1][0] - m_fData[0][0] * m_fData[1][2];
    
	retMat.m_fData[2][0] = m_fData[1][0] * m_fData[2][1] - m_fData[1][1] * m_fData[2][0];
    retMat.m_fData[2][1] = m_fData[0][1] * m_fData[2][0] - m_fData[0][0] * m_fData[2][1];
    retMat.m_fData[2][2] = m_fData[0][0] * m_fData[1][1] - m_fData[0][1] * m_fData[1][0];

	// Get the determinant
    float32 det = (m_fData[0][0] * retMat.m_fData[0][0])
				+ (m_fData[0][1] * retMat.m_fData[1][0])
				+ (m_fData[0][2] * retMat.m_fData[2][0]);

	// Is this okay to invert?
    if( det == 0.0f )
        return Matrix3x3();

	// Scale the return matrix by 1 over the determinant
    retMat *= 1.0f / det;
    return retMat;
}

	
///////////////////////////////////////////////////////////////////////////////
//
//	Matrix3x3::operator +=( ) Public
/*!
//	\param addMat The matrix to add to this one
//
//	Entry-wise add.
*/
///////////////////////////////////////////////////////////////////////////////
void Matrix3x3::operator += ( const Matrix3x3& addMat )
{
	for( int32 row = 0; row < 3; ++row )
	{
		m_fData[row][0] += addMat.m_fData[row][0];
		m_fData[row][1] += addMat.m_fData[row][1];
		m_fData[row][2] += addMat.m_fData[row][2];
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix3x3::Scale( ) Public
/*!
//	\param scalar The coefficient to multiply each entry by
//
//	Multiply every entry by a number.
*/
///////////////////////////////////////////////////////////////////////////////
void Matrix3x3::Scale( float32 scalar )
{
	for( int32 row = 0; row < 3; ++row )
	{
		m_fData[row][0] *= scalar;
		m_fData[row][1] *= scalar;
		m_fData[row][2] *= scalar;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix3x3::TildeVector( ) Public
/*!
//	\param vect The vector to use the tilde operator on
//
//	Create the matrix from a vector using the tilde operator.  This is the
//	same as turning the cross product into a matrix.
*/
///////////////////////////////////////////////////////////////////////////////
void Matrix3x3::TildeVector( const Vector3& vect )
{
	m_fData[0][0] = 0; m_fData[0][1] = -vect.z; m_fData[0][2] = vect.y;
	m_fData[1][0] = vect.z; m_fData[1][1] = 0; m_fData[1][2] = -vect.x;
	m_fData[2][0] = -vect.y; m_fData[2][1] = vect.x; m_fData[2][2] = 0;
}


/***********************************************

  Matrix3x3::Compose()

  This function multiplies this matrix with a 3x1 vector.

  Returns the determinant, 0 if there was any
  problem.

***********************************************/
Vector3 Matrix3x3::Compose
(
	const Vector3 &multiplicandVector			// In : The vector to mulitply
) const
{
	float32 newX, newY, newZ;


	//----------------------------------------------
	// Calculate the new X component of the return vector
	//----------------------------------------------
	newX = ( m_fData[0][0] * multiplicandVector.x )
			+ ( m_fData[0][1] * multiplicandVector.y )
			+ ( m_fData[0][2] * multiplicandVector.z );


	//----------------------------------------------
	// Calculate the new Y component of the return vector
	//----------------------------------------------
	newY = ( m_fData[1][0] * multiplicandVector.x )
			+ ( m_fData[1][1] * multiplicandVector.y )
			+ ( m_fData[1][2] * multiplicandVector.z );


	//----------------------------------------------
	// Calculate the new Z component of the return vector
	//----------------------------------------------
	newZ = ( m_fData[2][0] * multiplicandVector.x )
			+ ( m_fData[2][1] * multiplicandVector.y )
			+ ( m_fData[2][2] * multiplicandVector.z );

	// Return the vector
	return Vector3(newX, newY, newZ);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix3x3::Compose( ) Public
/*!
//	\param multiplicandVector The vector to multiply
//
//	This function multiplies a vector by the matrix.
*/
///////////////////////////////////////////////////////////////////////////////
Vector2 Matrix3x3::Compose( const Vector2 &multiplicandVector ) const
{
	float32 newX, newY;

	// Calculate the new X component of the return vector
	newX = ( m_fData[0][0] * multiplicandVector.x )
			+ ( m_fData[0][1] * multiplicandVector.y );


	// Calculate the new Y component of the return vector
	newY = ( m_fData[1][0] * multiplicandVector.x )
			+ ( m_fData[1][1] * multiplicandVector.y );

	// Return the vector
	return Vector2( newX, newY );
}


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix3x3::Compose( ) Public
/*!
//	\param multiplicandVector The point to multiply
//
//	This function transforms the point by the matrix and ensures the z
//	component stays 1.
*/
///////////////////////////////////////////////////////////////////////////////
Point2 Matrix3x3::Compose( const Point2 &multiplicandVector ) const
{
	float32 newX, newY, newZ;

	// Calculate the new X component of the return point
	newX = ( m_fData[0][0] * multiplicandVector.x )
			+ ( m_fData[0][1] * multiplicandVector.y )
			+ m_fData[0][2];

	// Calculate the new Y component of the return point
	newY = ( m_fData[1][0] * multiplicandVector.x )
			+ ( m_fData[1][1] * multiplicandVector.y )
			+ m_fData[1][2];

	// Calculate the new Z component of the return point
	newZ = ( m_fData[2][0] * multiplicandVector.x )
			+ ( m_fData[2][1] * multiplicandVector.y )
			+ m_fData[2][2];

	// Scale the point by the Z component to make it a point again
	newZ = 1.0f / newZ;
	newX *= newZ;
	newY *= newZ;

	// Return the point
	return Point2( newX, newY );
}


/***********************************************

  Matrix3x3::ConstructFromVectors()

  This function fills in a 3x3 matrix from 3 column
  vectors.

***********************************************/
void Matrix3x3::ConstructFromRowVectors
(
	const Vector3 vectors[3]					// In : The column vectors
)
{
	// Fill in the X row with the vector values
	m_fData[0][0] = vectors[0].x;
	m_fData[0][1] = vectors[1].x;
	m_fData[0][2] = vectors[2].x;

	// Fill in the Y row with the vector values
	m_fData[1][0] = vectors[0].y;
	m_fData[1][1] = vectors[1].y;
	m_fData[1][2] = vectors[2].y;

	// Fill in the Z row with the vector values
	m_fData[2][0] = vectors[0].z;
	m_fData[2][1] = vectors[1].z;
	m_fData[2][2] = vectors[2].z;
}


/***********************************************

  Matrix3x3::ConstructRotationAboutXAxis()

  This function creates a rotation matrix about the
  X axis.

***********************************************/
void Matrix3x3::ConstructRotationAboutXAxis
(
	float32 fDegrees							// In : The amount of degrees to rotate
)
{
	// Calculate the sine and cosine of the angle
	// Also, convert the degrees to radians
	float32 cosAngle = cosf(fDegrees * PI_OVER_180),
				sinAngle = sinf(fDegrees * PI_OVER_180);

	// Fill in the matrix with the X axis rotation matrix
	m_fData[0][0] = 1;			m_fData[0][1] = 0;			m_fData[0][2] = 0;
	m_fData[1][0] = 0;			m_fData[1][1] = cosAngle;	m_fData[1][2] = -sinAngle;
	m_fData[2][0] = 0;			m_fData[2][1] = sinAngle;	m_fData[2][2] = cosAngle;
}


/***********************************************

  Matrix3x3::ConstructRotationAboutYAxis()

  This function creates a rotation matrix about the
  Y axis.

***********************************************/
void Matrix3x3::ConstructRotationAboutYAxis
(
	float32 fDegrees							// In : The amount of degrees to rotate
)
{
	// Calculate the sine and cosine of the angle
	// Also, convert the degrees to radians
	float32 cosAngle = cosf(fDegrees * PI_OVER_180),
				sinAngle = sinf(fDegrees * PI_OVER_180);

	// Fill in the matrix with the Y axis rotation matrix
	m_fData[0][0] = cosAngle;	m_fData[0][1] = 0;	m_fData[0][2] = sinAngle;
	m_fData[1][0] = 0;			m_fData[1][1] = 1;	m_fData[1][2] = 0;
	m_fData[2][0] = -sinAngle;	m_fData[2][1] = 0;	m_fData[2][2] = cosAngle;
}


/***********************************************

  Matrix3x3::ConstructRotationAboutZAxis()

  This function creates a rotation matrix about the
  Z axis.

***********************************************/
void Matrix3x3::ConstructRotationAboutZAxis
(
	float32 fDegrees							// In : The amount of degrees to rotate
)
{
	// Calculate the sine and cosine of the angle
	// Also, convert the degrees to radians
	float32 cosAngle = cosf(fDegrees * PI_OVER_180),
				sinAngle = sinf(fDegrees * PI_OVER_180);

	// Fill in the matrix with the Z axis rotation matrix
	m_fData[0][0] = cosAngle;	m_fData[0][1] = -sinAngle;	m_fData[0][2] = 0;
	m_fData[1][0] = sinAngle;	m_fData[1][1] = cosAngle;	m_fData[1][2] = 0;
	m_fData[2][0] = 0;			m_fData[2][1] = 0;			m_fData[2][2] = 1;
}


/***********************************************

  Matrix3x3::ConstructRotationFromVector()

  Construction of a rotation matrix for an
  arbitrary axis.

***********************************************/
void Matrix3x3::ConstructRotationFromVector
(
	const Vector3 &axisVector,				// In : The unit vector to rotate about
	float32 fDegrees							// In : The amount to rotate
)
{
	// Calculate the sine and cosine of the angle since it is used a lot
	// while converting the degrees to radians before hand
	float32 cosAngle = cosf(fDegrees * PI_OVER_180),
				sinAngle = sinf(fDegrees * PI_OVER_180);

	// Fill in the matrix
	// Row 1
	m_fData[0][0] = (axisVector.x * axisVector.x) * (1 - cosAngle) + cosAngle;
	m_fData[0][1] = (axisVector.x * axisVector.y) * (1 - cosAngle)
															- (axisVector.z * sinAngle);
	m_fData[0][2] = (axisVector.x * axisVector.z) * (1 - cosAngle)
															+ (axisVector.y * sinAngle);

	// Row 2
	m_fData[1][0] = (axisVector.y * axisVector.x) * (1 - cosAngle)
															+ (axisVector.z * sinAngle);
	m_fData[1][1] = (axisVector.y * axisVector.y) * (1 - cosAngle) + cosAngle;
	m_fData[1][2] = (axisVector.y * axisVector.z) * (1 - cosAngle)
															- (axisVector.x * sinAngle);

	// Row 3
	m_fData[2][0] = (axisVector.z * axisVector.x) * (1 - cosAngle)
															- (axisVector.y * sinAngle);
	m_fData[2][1] = (axisVector.z * axisVector.y) * (1 - cosAngle)
															+ (axisVector.x * sinAngle);
	m_fData[2][2] = (axisVector.z * axisVector.z) * (1 - cosAngle) + cosAngle;
}


