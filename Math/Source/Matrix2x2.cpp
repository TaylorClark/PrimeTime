//=============================================================================
/*!
	\file Matrix2x2.cpp
	Taylor Clark Math Library
	3x3 Matrix Class
	\author Taylor Clark

  This source file contains the implementation of the 2x2 matrix class.
*/
//=============================================================================

#include "../Matrix2x2.h"
#include <math.h>


const float32 PI_OVER_180 = 3.14159265f / 180.0f;


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix2x2::Matrix2x2  Public
/*!
//	This constructor for the matrix class clears the matrix with 0's.
*/
///////////////////////////////////////////////////////////////////////////////
Matrix2x2::Matrix2x2()
{
	// Set the data to 0
	for(uint32 index = 0; index < 2; ++index)
	{
		// Set this entry to 0
		m_fData[index][0] = 0;
		m_fData[index][1] = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix2x2::SetEntry  Public
/*!
//	\param row The row index to set
//	\param col The column index to set
//	\param entryValue The value to set
//
//	This function sets one of the entries in the matrix to a passed in value.
*/
///////////////////////////////////////////////////////////////////////////////
void Matrix2x2::Set( uint32 row, uint32 col, float32 entryValue )
{
	// Ensure the row and column are valid
	if((row >= 2) || (col >= 2))
	{
		// The row number is too great so exit this function
		return;
	}

	// Set the value in the matrix
	m_fData[row][col] = entryValue;
}


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix2x2::Get  Public
/*!
//	\param row The row of the entry to get
//	\param col The column of the entry to get
//	\retval The value at the matrix entry, 0 if an invalid index
//
//	Get a value in the matrix.
*/
///////////////////////////////////////////////////////////////////////////////
float32 Matrix2x2::Get( uint32 row, uint32 col ) const
{
	// Ensure the row and column are valid
	if((row >= 2) || (col >= 2))
	{
		// The row number is too great so exit this function
		return 0;
	}

	// Return the value in the matrix
	return m_fData[row][col];
}


/***********************************************

  Matrix2x2::Compose()

  This function multiplies this matrix by the passed
  in matrix and returns the result.

  Returns the resultant matrix.

***********************************************/
Matrix2x2 Matrix2x2::Compose
(
	const Matrix2x2 &multiplicandMatrix			// In : The matrix to multiply by
) const
{
	Matrix2x2 returnMatrix;
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
								+ (m_fData[0][1] * multiplicandMatrix.m_fData[1][0]);

	returnMatrix.m_fData[0][1] = (m_fData[0][0] * multiplicandMatrix.m_fData[0][1])
								+ (m_fData[0][1] * multiplicandMatrix.m_fData[1][1]);

	// Row 2
	returnMatrix.m_fData[1][0] = (m_fData[1][0] * multiplicandMatrix.m_fData[0][0])
								+ (m_fData[1][1] * multiplicandMatrix.m_fData[1][0]);

	returnMatrix.m_fData[1][1] = (m_fData[1][0] * multiplicandMatrix.m_fData[0][1])
								+ (m_fData[1][1] * multiplicandMatrix.m_fData[1][1]);

	// Return the matrix
	return returnMatrix;
}


/***********************************************

  Matrix2x2::Transpose()

  This function calculates this matrix's transpose.

  Returns the resultant matrix.

***********************************************/
Matrix2x2 Matrix2x2::Transpose() const
{
	// Create the matrix with the rows and columns reversed from this class
	Matrix2x2 retTransposeMatrix;

	// Fill in the matrix entries
	for(uint32 currentRow = 0; currentRow < 2; ++currentRow)
	{
		// Loop through the columns
		for(uint32 currentColumn = 0; currentColumn < 2; ++currentColumn)
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


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix2x2::Inverse( ) Public
/*!
//	Get the inverse of a 2 by 2 matrix.
*/
///////////////////////////////////////////////////////////////////////////////
Matrix2x2 Matrix2x2::Inverse( void ) const
{
	Matrix2x2 invMatrix;

	// If the determinant is 0, we can't invert it
	float32 invDet = Determinant();
    if( invDet == 0 )
		return invMatrix;
	invDet = 1.0f / invDet;

	// Get the inverse
	// Row 0
	invMatrix.m_fData[0][0] = invDet * m_fData[1][1];
	invMatrix.m_fData[0][1] = invDet * -m_fData[0][1];

	// Row 1
	invMatrix.m_fData[1][0] = invDet * -m_fData[1][0];
	invMatrix.m_fData[1][1] = invDet * m_fData[0][0];

	return invMatrix;
}


/***********************************************

  Matrix2x2::Determinant()

  This function calculates the determinate and
  returns it.

  Returns the determinant, 0 if there was any
  problem.

***********************************************/
float32 Matrix2x2::Determinant() const
{
	// Return the determinant (ad - bc)
	return (m_fData[0][0] * m_fData[1][1]) - (m_fData[1][0]*m_fData[0][1]);
}


/***********************************************

  Matrix2x2::Trace()

  This function calculates the of the matrix.

  Returns the trace of the matrix which is the
  sum of the diagonals.

***********************************************/
float32 Matrix2x2::Trace(void) const
{
	return m_fData[0][0] + m_fData[1][1];
}


/***********************************************

  Matrix2x2::Compose()

  This function multiplies this matrix with a 3x1 vector.

  Returns the determinant, 0 if there was any
  problem.

***********************************************/
Vector2 Matrix2x2::Compose
(
	const Vector2 &multiplicandVector			// In : The vector to mulitply
) const
{
	float32 newX, newY;

	// Calculate the new X component of the return vector
	newX = ( m_fData[0][0] * multiplicandVector.x )
			+ ( m_fData[0][1] * multiplicandVector.y );


	// Calculate the new Y component of the return vector
	newY = ( m_fData[1][0] * multiplicandVector.x )
			+ ( m_fData[1][1] * multiplicandVector.y );

	// Return the vector
	return Vector2(newX, newY);
}


///////////////////////////////////////////////////////////////////////////////
//
//	Matrix2x2::Compose( ) Public
/*!
//	\param multiplicandVector The point to multiply
//
//	This function transforms the point by the matrix and ensures the z
//	component stays 1.
*/
///////////////////////////////////////////////////////////////////////////////
Point2 Matrix2x2::Compose( const Point2 &multiplicandVector ) const
{
	float32 newX, newY;

	// Calculate the new X component of the return point
	newX = ( m_fData[0][0] * multiplicandVector.x )
			+ ( m_fData[0][1] * multiplicandVector.y );

	// Calculate the new Y component of the return point
	newY = ( m_fData[1][0] * multiplicandVector.x )
			+ ( m_fData[1][1] * multiplicandVector.y );

	// Return the point
	return Point2( newX, newY );
}


/***********************************************

  Matrix2x2::ConstructFromVectors()

  This function fills in a 2x2 matrix from 2 row vectors.

***********************************************/
void Matrix2x2::ConstructFromRowVectors
(
	const Vector2 vectors[2]					// In : The column vectors
)
{
	// Fill in the X row with the vector values
	m_fData[0][0] = vectors[0].x;
	m_fData[0][1] = vectors[1].x;

	// Fill in the Y row with the vector values
	m_fData[1][0] = vectors[0].y;
	m_fData[1][1] = vectors[1].y;
}


/***********************************************

  Matrix2x2::ConstructRotationFromXAxis()

  This function creates a rotation matrix about the
  X axis.

***********************************************/
void Matrix2x2::ConstructRotation
(
	float32 fDegrees							// In : The amount of degrees to rotate
)
{
	// Calculate the sine and cosine of the angle
	// Also, convert the degrees to radians
	float32 cosAngle = cosf(fDegrees * PI_OVER_180),
				sinAngle = sinf(fDegrees * PI_OVER_180);

	// Fill in the matrix with the X axis rotation matrix
	m_fData[0][0] = cosAngle;	m_fData[0][1] = -sinAngle;
	m_fData[1][0] = sinAngle;	m_fData[1][1] = cosAngle;
}