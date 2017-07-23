//=============================================================================
/*!
	\file Affine.cpp
	Taylor Clark Math Library
	Affine Transformation Matrix Class
	\author Taylor Clark

  This source file contains the implementation for the affine transformation
  class.
*/
//=============================================================================

#include "../Affine.h"
#include <math.h>

const CAffine IDENT_4x4( 1, 0, 0, 0,
							0, 1, 0, 0,
							0, 0, 1, 0,
							0, 0, 0, 1 );

const float PI_OVER_180 = 3.14159265f / 180.0f;


///////////////////////////////////////////////////////////////////////////////
//
//	CAffine::ZeroMatrix( ) Public
/*!
//	Set the entire matrix to zeros.
*/
///////////////////////////////////////////////////////////////////////////////
void CAffine::ZeroMatrix(void)
{
	// Zero the matrix values
	for(long curRow = 0; curRow < 4; ++curRow)
	{
		// Zero out each column in the row
		m_fMatrix[curRow][0] = 0;
		m_fMatrix[curRow][1] = 0;
		m_fMatrix[curRow][2] = 0;
		m_fMatrix[curRow][3] = 0;
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//	CAffine::ConstructScaling( ) Public
/*!
//	\param fScalingFactor The amount to scale
//	\param scalingPoint The point to scale about
//
//	Construct a homogeneous scaling matrix for scaling about an arbitrary
//	point.
*/
///////////////////////////////////////////////////////////////////////////////
void CAffine::ConstructScaling(float fScalingFactor, Point3 scalingPoint)
{
	// Clear the matrix
	ZeroMatrix();
	
	// Create the translation matrix to the orgin
	CAffine translateToOrgin;
	translateToOrgin.ConstructTranslation(-scalingPoint.x,
											-scalingPoint.y,
											-scalingPoint.z);

	// Create the translation matrix back to the point
	CAffine translateBackToPt;
	translateBackToPt.ConstructTranslation(scalingPoint.x,
											scalingPoint.y,
											scalingPoint.z);

	// Fill in the matrix with the scaling factors
	m_fMatrix[0][0] = fScalingFactor;
	m_fMatrix[1][1] = fScalingFactor;
	m_fMatrix[2][2] = fScalingFactor;
	m_fMatrix[3][3] = 1;

	// Multiply the matrices together and store the result
	*this = (translateBackToPt * *this) * translateToOrgin;
}


//**********************************************
//
//	CAffine::ConstructScaling() Public
//
//	Construct a homogeneous scaling matrix for
//	scaling about an arbitrary point.
//
//	Input :	float xScaling : The amount to scale about the X axis
//			float yScaling : The amount to scale about the Y axis
//			float zScaling : The amount to scale about the Z axis
//			Point3 scalingPoint : The point to scale about
//
//**********************************************
void CAffine::ConstructScaling(float xScaling, float yScaling, float zScaling,
								Point3 scalingPoint)
{
	// Clear the matrix
	ZeroMatrix();
	
	// Create the translation matrix to the orgin
	CAffine translateToOrgin;
	translateToOrgin.ConstructTranslation(-scalingPoint.x,
											-scalingPoint.y,
											-scalingPoint.z);

	// Create the translation matrix back to the point
	CAffine translateBackToPt;
	translateBackToPt.ConstructTranslation(scalingPoint.x,
											scalingPoint.y,
											scalingPoint.z);

	// Fill in the matrix with the scaling factors
	m_fMatrix[0][0] = xScaling;
	m_fMatrix[1][1] = yScaling;
	m_fMatrix[2][2] = zScaling;
	m_fMatrix[3][3] = 1;

	// Multiply the matrices together and store the result
	*this = (translateBackToPt * *this) * translateToOrgin;
}


//**********************************************
//
//	CAffine::ConstructProjection001() Public
//
//	Create a projection matrix for the point (0, 0, 1).
//
//**********************************************
void CAffine::ConstructProjection001(void)
{
	// Fill in the matrix with the projection for (0,0,1) matrix
	m_fMatrix[0][0] = 1; m_fMatrix[0][1] = 0; m_fMatrix[0][2] = 0; m_fMatrix[0][3] = 0;
	m_fMatrix[1][0] = 0; m_fMatrix[1][1] = 1; m_fMatrix[1][2] = 0; m_fMatrix[1][3] = 0;
	m_fMatrix[2][0] = 0; m_fMatrix[2][1] = 0; m_fMatrix[2][2] = 0; m_fMatrix[2][3] = 0;
	m_fMatrix[3][0] = 0; m_fMatrix[3][1] = 0; m_fMatrix[3][2] = -1; m_fMatrix[3][3] = 1;
}


//**********************************************
//
//	CAffine::ConstructProjection00Z() Public
//
//	Create a projection matrix for the point (0, 0, Z).
//
//	Input :	float zPoint - The Z point offset
//
//**********************************************
void CAffine::ConstructProjection00Z(float zPoint)
{
	// Fill in the matrix with the projection for (0,0,1) matrix
	m_fMatrix[0][0] = 1; m_fMatrix[0][1] = 0; m_fMatrix[0][2] = 0; m_fMatrix[0][3] = 0;
	m_fMatrix[1][0] = 0; m_fMatrix[1][1] = 1; m_fMatrix[1][2] = 0; m_fMatrix[1][3] = 0;
	m_fMatrix[2][0] = 0; m_fMatrix[2][1] = 0; m_fMatrix[2][2] = 0; m_fMatrix[2][3] = 0;
	m_fMatrix[3][0] = 0; m_fMatrix[3][1] = 0; m_fMatrix[3][2] = -1; m_fMatrix[3][3] = zPoint;
}


//**********************************************
//
//	CAffine::ConstructSheer() Public
//
//	Construct the sheer matrix.
//
//	Input :	Vector3 rightVect - The right vector for the view plane
//			Vector3 upVect - The up vector for the view plane
//			Vector3 viewVect - The oblique projection vector for the view plane
//
//**********************************************
void CAffine::ConstructSheer(Vector3 rightVect, Vector3 upVect, Vector3 viewVect)
{
	// Get the normal, or Z-axis vector, for the view plane
	Vector3 normVect;
	normVect = rightVect.Cross(upVect);

	// Calculate the W component of the matrix
	Vector3 wVect;
	wVect.x = ( -(viewVect.Dot(rightVect) / viewVect.Dot(normVect)) );
	wVect.y = ( -(viewVect.Dot(upVect) / viewVect.Dot(normVect)) );
	wVect.z = ( 1 / viewVect.Dot(normVect) );

	// Fill in the matrix
	m_fMatrix[0][0] = 1; m_fMatrix[0][1] = 0; m_fMatrix[0][2] = wVect.x; m_fMatrix[0][3] = 0;
	m_fMatrix[1][0] = 0; m_fMatrix[1][1] = 1; m_fMatrix[1][2] = wVect.y; m_fMatrix[1][3] = 0;
	m_fMatrix[2][0] = 0; m_fMatrix[2][1] = 0; m_fMatrix[2][2] = wVect.z; m_fMatrix[2][3] = 0;
	m_fMatrix[3][0] = 0; m_fMatrix[3][1] = 0; m_fMatrix[3][2] = 0; m_fMatrix[3][3] = 1;

}


//******************************************************
//
//	CAffine::ConstructQ()   Public
//
//	Construct the Q matrix found in the CS250 notes.
//
//	Input :	float x - The X value for the matrix
//			float x - The Y value for the matrix
//			float x - The Z value for the matrix
//
//******************************************************
void CAffine::ConstructQ(float x, float y, float z)
{
	// Clear the matrix
	ZeroMatrix();

	// Fill in the X component
	m_fMatrix[0][0] = x;

	// Fill in the Y component
	m_fMatrix[1][1] = y;

	// Fill in the Z component
	m_fMatrix[2][2] = z;

	// Fill in the homogenous coordinate
	m_fMatrix[3][3] = 1;
}


//******************************************************
//
//	CAffine::ConstructQInv()   Public
//
//	Construct the Q inverse matrix found in the CS250 notes.
//
//	Input :	float x - The X value for the matrix
//			float x - The Y value for the matrix
//			float x - The Z value for the matrix
//
//******************************************************
void CAffine::ConstructQInv(float x, float y, float z)
{
	// Clear the matrix
	ZeroMatrix();

	// Fill in the X component
	if(x != 0)
		m_fMatrix[0][0] = 1 / x;
	else
		m_fMatrix[0][0] = 0;

	// Fill in the Y component
	if(y != 0)
		m_fMatrix[1][1] = 1 / y;
	else
		m_fMatrix[1][1] = 0;

	// Fill in the Z component
	if(z != 0)
		m_fMatrix[2][2] = 1 / z;
	else
		m_fMatrix[2][2] = 0;

	// Fill in the homogenous coordinate
	m_fMatrix[3][3] = 1;
}


//******************************************************
//
//	CAffine::ConstructPlaneInv() Public
//
//	Create the plane inverse matrix.
//
//	Input :	const Vector3 &rightVect - The view plane's
//				right vector.
//			const Vector3 &upVect - The view plane's up
//				vector.
//			const Vector3 &rightVect - The view plane's
//				up (and oblique for now) vector.
//
//******************************************************
void CAffine::ConstructPlaneInv(const Vector3 &rightVect,
					   const Vector3 &upVect,
					   const Vector3 &frontVect)
{
	// Clear the matrix
	ZeroMatrix();

	// Set the bottom left entry to 0
	m_fMatrix[3][3] = 1;

	// Get m dot n (front dot front normally)
	float mDotn = frontVect.Dot(frontVect);

	// Ensure this is not 0
	if(mDotn == 0)
		mDotn = 1;

	// Get the first column vector
	float scalar = (frontVect.Dot(rightVect)) / mDotn;
	Vector3 curColVect = frontVect;
	curColVect *= scalar;
	curColVect = rightVect - curColVect;

	// Set the first column in the matrix
	m_fMatrix[0][0] = curColVect.x;
	m_fMatrix[1][0] = curColVect.y;
	m_fMatrix[2][0] = curColVect.z;

	// Get the second column
	scalar = (frontVect.Dot(upVect)) / mDotn;
	curColVect = frontVect;
	curColVect *= scalar;
	curColVect = upVect - curColVect;

	// Set the second column in the matrix
	m_fMatrix[0][1] = curColVect.x;
	m_fMatrix[1][1] = curColVect.y;
	m_fMatrix[2][1] = curColVect.z;

	// Get the third column
	scalar = 1 / mDotn;
	curColVect = frontVect * scalar;

	// Set the third column in the matrix
	m_fMatrix[0][2] = curColVect.x;
	m_fMatrix[1][2] = curColVect.y;
	m_fMatrix[2][2] = curColVect.z;

	// Transpose this matrix
	*this = Transpose();
}


//**********************************************
//
//	CAffine::Transpose() Public
//
//	Create the transpose of this matrix.
//
//	Output:	CAffine - Returns the transpose of this matrix.
//
//**********************************************
CAffine CAffine::Transpose(void)
{
	CAffine transposedMatrix;

	// Fill in the new matrix
	for(int curRow = 0; curRow < 4; ++curRow)
	{
		transposedMatrix.m_fMatrix[curRow][0] = m_fMatrix[0][curRow];
		transposedMatrix.m_fMatrix[curRow][1] = m_fMatrix[1][curRow];
		transposedMatrix.m_fMatrix[curRow][2] = m_fMatrix[2][curRow];
		transposedMatrix.m_fMatrix[curRow][3] = m_fMatrix[3][curRow];
	}

	// Return the transposed matrix
	return transposedMatrix;
}


//**********************************************
//
//	CAffine::ConstructRotation() Public
//
//	Construct a rotation matrix for rotation about
//	an arbitrary point.
//
//	Input :	Point3 pointOnAxis : A point of the axis of rotation
//			Vector3 axisVector : A unit vector parallel to the axis of rotation
//			float fDegrees : The amount to rotate, in degrees about the axis
//
//**********************************************
void CAffine::ConstructRotation(Point3 pointOnAxis, Vector3 axisVector, float fDegrees)
{
	// Clear the matrix
	ZeroMatrix();
	
	// Create the translation matrix to the orgin
	CAffine translateToOrgin;
	translateToOrgin.ConstructTranslation(-pointOnAxis.x,
											-pointOnAxis.y,
											-pointOnAxis.z);

	// Create the translation matrix back to the point
	CAffine translateBackToPt;
	translateBackToPt.ConstructTranslation(pointOnAxis.x,
											pointOnAxis.y,
											pointOnAxis.z);


	//----------------------------------------------
	// Fill in the matrix with the rotation matrix
	//----------------------------------------------
	
	// Clear the matrix
	ZeroMatrix();

	// Calculate the sine and cosine of the angle since it is used a lot
	// while converting the degrees to radians before hand
	float cosAngle = cosf(fDegrees * PI_OVER_180),
				sinAngle = sinf(fDegrees * PI_OVER_180);

	// Row 1
	m_fMatrix[0][0] = (axisVector.x * axisVector.x) * (1 - cosAngle) + cosAngle;
	m_fMatrix[0][1] = (axisVector.x * axisVector.y) * (1 - cosAngle)
															- (axisVector.z * sinAngle);
	m_fMatrix[0][2] = (axisVector.x * axisVector.z) * (1 - cosAngle)
															+ (axisVector.y * sinAngle);
	
	// Row 2
	m_fMatrix[1][0] = (axisVector.y * axisVector.x) * (1 - cosAngle)
															+ (axisVector.z * sinAngle);
	m_fMatrix[1][1] = (axisVector.y * axisVector.y) * (1 - cosAngle) + cosAngle;
	m_fMatrix[1][2] = (axisVector.y * axisVector.z) * (1 - cosAngle)
															- (axisVector.x * sinAngle);
	
	// Row 3
	m_fMatrix[2][0] = (axisVector.z * axisVector.x) * (1 - cosAngle)
															- (axisVector.y * sinAngle);
	m_fMatrix[2][1] = (axisVector.z * axisVector.y) * (1 - cosAngle)
															+ (axisVector.x * sinAngle);
	m_fMatrix[2][2] = (axisVector.z * axisVector.z) * (1 - cosAngle) + cosAngle;

	// Set the bottom right entry to 1
	m_fMatrix[3][3] = 1;

	
	//----------------------------------------------
	// Multiply the matrices together and store the result
	//----------------------------------------------
	*this = (translateBackToPt * *this) * translateToOrgin;
}


//**********************************************
//
//	CAffine::ConstructReflection() Public
//
//	Construct a reflection matrix for reflection about
//	an arbitrary plane.
//
//	Input :	Vector3 normalVector : The vector normal to the plane
//			Point3 point : A point in the plane
//
//**********************************************
void CAffine::ConstructReflection(Vector3 normalVector, Point3 point)
{
	// Clear the matrix
	ZeroMatrix();
	
	// Create the translation matrix to the orgin
	CAffine translateToOrgin;
	translateToOrgin.ConstructTranslation(-point.x,
											-point.y,
											-point.z);

	// Create the translation matrix back to the point
	CAffine translateBackToPt;
	translateBackToPt.ConstructTranslation(point.x,
											point.y,
											point.z);

	// Row 1
	m_fMatrix[0][0] = 1 - (2 * (normalVector.x * normalVector.x) );
	m_fMatrix[0][1] = -2 * normalVector.x * normalVector.y;
	m_fMatrix[0][2] = -2 * normalVector.x * normalVector.z;
	
	// Row 2
	m_fMatrix[1][0] = -2 * normalVector.x * normalVector.y;
	m_fMatrix[1][1] = 1 - (2 * (normalVector.y * normalVector.y) );
	m_fMatrix[1][2] = -2 * normalVector.y * normalVector.z;
	
	// Row 3
	m_fMatrix[2][0] = -2 * normalVector.x * normalVector.z;
	m_fMatrix[2][1] = -2 * normalVector.y * normalVector.z;
	m_fMatrix[2][2] = 1 - (2 * (normalVector.z * normalVector.z) );
	
	// Set the bottom right entry to 1
	m_fMatrix[3][3] = 1;

	// The projection
	*this = (translateBackToPt * *this) * translateToOrgin;
}


//**********************************************
//
//	CAffine::ConstructFromVectors3D() Public
//
//	Initialize the first 3x3 entries from 3 column vectors
//	and place a 1 in the bottom left entry.
//
//	Input :	Vector3 col1Vect - The first column vector
//			Vector3 col2Vect - The second column vector
//			Vector3 col3Vect - The third column vector
//
//**********************************************
void CAffine::ConstructFromVectors3D(Vector3 col1Vect, Vector3 col2Vect,
																Vector3 col3Vect)
{
	// Clear the matrix
	ZeroMatrix();

	// Set the first column
	m_fMatrix[0][0] = col1Vect.x;
	m_fMatrix[1][0] = col1Vect.y;
	m_fMatrix[2][0] = col1Vect.z;

	// Set the second column
	m_fMatrix[0][1] = col2Vect.x;
	m_fMatrix[1][1] = col2Vect.y;
	m_fMatrix[2][1] = col2Vect.z;

	// Set the third column
	m_fMatrix[0][2] = col3Vect.x;
	m_fMatrix[1][2] = col3Vect.y;
	m_fMatrix[2][2] = col3Vect.z;

	// Set the last entry to 1
	m_fMatrix[3][3] = 1;
}


//**********************************************
//
//	CAffine::ConstructTranslation() Public
//
//	Construct a translation matrix.
//
//	Input :	Vector3 translationAmount : The translation amount
//
//**********************************************
void CAffine::ConstructTranslation(float x, float y, float z)
{
	// Clear the matrix
	ZeroMatrix();

	// Set the diagonals to 1
	m_fMatrix[0][0] = 1;
	m_fMatrix[1][1] = 1;
	m_fMatrix[2][2] = 1;
	m_fMatrix[3][3] = 1;

	// Fill in the last column with the translation
	m_fMatrix[0][3] = x;
	m_fMatrix[1][3] = y;
	m_fMatrix[2][3] = z;
}


//**********************************************
//
//	CAffine::Compose() Public
//
//	Compose 2 affine objects together.
//
//	Input :	const CAffine &composeAffine : The affine transformation object to
//												compose with.
//
//	Output : CAffine : The resultant affine transformation object
//
//**********************************************
CAffine CAffine::Compose(const CAffine &composeAffine) const
{
	CAffine returnAffine;

	// Multiply the matrices together
	// Loop through all of the rows
	for(int curRow = 0; curRow < 4; ++curRow)
	{
		// Loop through all of the columns
		for(int curColumn = 0; curColumn < 4; ++curColumn)
		{
			// Multiply the row and column
			int curMultRow = 0, curMultColumn = 0;
			float currentEntrySum = 0;
			while(curMultColumn < 4)
			{
				// Get the product and add it to the sum
				currentEntrySum += m_fMatrix[curRow][curMultColumn]
									* composeAffine.m_fMatrix[curMultRow][curColumn];

				// Increment the current row and column
				++curMultRow;
				++curMultColumn;

			} // End of row and column multiplication

			// Set this entry with the calculated value
			returnAffine.m_fMatrix[curRow][curColumn] = currentEntrySum;

		} // End of column loop

	} // End of row loop

	// Return the calculate affine matrix
	return returnAffine;
}


//**********************************************
//
//	CAffine::Compose() Public
//
//	Compose this affine object with points treated as a 4D vector.
//
//	Input :	float *xP, *yP, *zP are the point components
//
//**********************************************
void CAffine::ComposeWithWDiv(float *xP, float *yP, float *zP) const
{
	// Calculate the new X component
	float newX;

	// Multiply the first row by the column vector
	newX = (m_fMatrix[0][0] * *xP) + (m_fMatrix[0][1] * *yP)
				+ (m_fMatrix[0][2] * *zP) + m_fMatrix[0][3];

	// Calculate the new Y component
	float newY;

	// Multiply the second row by the column vector
	newY = (m_fMatrix[1][0] * *xP) + (m_fMatrix[1][1] * *yP)
				+ (m_fMatrix[1][2] * *zP) + m_fMatrix[1][3];

	// Calculate the new Z component
	float newZ;

	// Multiply the third row by the column vector
	newZ = (m_fMatrix[2][0] * *xP) + (m_fMatrix[2][1] * *yP)
				+ (m_fMatrix[2][2] * *zP) + m_fMatrix[2][3];

	// Calculate the new W component
	float newW;

	// Multiply the fourth row by the column vector
	newW = (m_fMatrix[3][0] * *xP) + (m_fMatrix[3][1] * *yP)
				+ (m_fMatrix[3][2] * *zP) + m_fMatrix[3][3];

	// Ensure the W component is 1 by multiplying by 1/W
	newW = 1 / newW;
	newX *= newW;
	newY *= newW;
	newZ *= newW;

	// Store the values in the pointers passed in
	*xP = newX;
	*yP = newY;
	*zP = newZ;
}


//**********************************************
//
//	CAffine::Compose() Public
//
//	Compose this affine object with points treated as a 4D vector.
//
//	Input :	float *xP, *yP, *zP are the point components
//
//**********************************************
Point3 CAffine::Compose(const Point3 &point) const
{
	Point3 retPoint;

	// Calculate the new X component
	// Multiply the first row by the column vector
	retPoint.x = (m_fMatrix[0][0] * point.x) + (m_fMatrix[0][1] * point.y)
				+ (m_fMatrix[0][2] * point.z) + m_fMatrix[0][3];

	// Calculate the new Y component
	// Multiply the second row by the column vector
	retPoint.y = (m_fMatrix[1][0] * point.x) + (m_fMatrix[1][1] * point.y)
				+ (m_fMatrix[1][2] * point.z) + m_fMatrix[1][3];

	// Calculate the new Z component
	// Multiply the third row by the column vector
	retPoint.z = (m_fMatrix[2][0] * point.x) + (m_fMatrix[2][1] * point.y)
				+ (m_fMatrix[2][2] * point.z) + m_fMatrix[2][3];

	// Calculate the new W component
	float newW;

	// Multiply the fourth row by the column vector
	newW = (m_fMatrix[3][0] * point.x) + (m_fMatrix[3][1] * point.y)
				+ (m_fMatrix[3][2] * point.z) + m_fMatrix[3][3];

	// Ensure the W component is 1 by multiplying by 1/W
	newW = 1 / newW;
	retPoint.x *= newW;
	retPoint.y *= newW;
	retPoint.z *= newW;

	// Return the point
	return retPoint;
}


//**********************************************
//
//	CAffine::Compose() Public
//
//	Compose this affine object with a 4D vector.
//
//	Input :	const Vector3 &compVector : The vector to compose with
//
//	Output : Vector3 : The resultant vector
//
//**********************************************
Vector3 CAffine::Compose(const Vector3 &compVector) const
{
	Vector3 returnVector;

	// Calculate the new X component
	float newX;

	// Multiply the first row by the column vector
	newX = (m_fMatrix[0][0] * compVector.x) + (m_fMatrix[0][1] * compVector.y)
					+ (m_fMatrix[0][2] * compVector.z);

	// Calculate the new Y component
	float newY;

	// Multiply the second row by the column vector
	newY = (m_fMatrix[1][0] * compVector.x) + (m_fMatrix[1][1] * compVector.y)
					+ (m_fMatrix[1][2] * compVector.z);

	// Calculate the new Z component
	float newZ;

	// Multiply the third row by the column vector
	newZ = (m_fMatrix[2][0] * compVector.x) + (m_fMatrix[2][1] * compVector.y)
					+ (m_fMatrix[2][2] * compVector.z);

	// Store the values in the pointers passed in
	returnVector.x = (newX);
	returnVector.y = (newY);
	returnVector.z = (newZ);
	
	// Return the resultant vector
	return returnVector;
}


//**********************************************
//
//	CAffine::operator =() Public
//
//	Copy one CAffine object to another.
//
//	Input :	const CAffine &copyObject : The CAffine object to copy
//
//**********************************************
void CAffine::operator =(const CAffine &copyObject)
{
	// Copy the rows and columns
	for(long curRow = 0; curRow < 4; ++curRow)
	{
		m_fMatrix[curRow][0] = copyObject.m_fMatrix[curRow][0];
		m_fMatrix[curRow][1] = copyObject.m_fMatrix[curRow][1];
		m_fMatrix[curRow][2] = copyObject.m_fMatrix[curRow][2];
		m_fMatrix[curRow][3] = copyObject.m_fMatrix[curRow][3];
	}
}


//**********************************************
//
//	CAffine::CAffine() Public
//
//	The copy constructor will copy one object to another.
//
//	Input :	const CAffine &copyObject : The CAffine object to copy
//
//**********************************************
CAffine::CAffine(const CAffine &copyObject)
{
	// Copy this object
	*this = copyObject;
}