using System;
using System.IO;

namespace TCMath
{
	//-------------------------------------------------------------------------
	/// <summary>
	/// A 2-dimensional vector.
	/// </summary>
	//-------------------------------------------------------------------------
	public struct Vector2
	{
		/// <summary> The x and y components of the vector </summary>
		public float x, y;


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector with an x and y component.
		/// </summary>
		/// <param name="inX"> The x component </param>
		/// <param name="inY"> The y compenent </param>
		///////////////////////////////////////////////////////////////////////
		public Vector2( float inX, float inY ) 
		{
			x = inX; y = inY;
		}
		

		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector from another vector.
		/// </summary>
		/// <param name="rhs"> The vector to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Vector2( Vector2 rhs )
		{
			x = rhs.x; y = rhs.y;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector from a 3D vector ignoring the Z component.
		/// </summary>
		/// <param name="rhs"> The vector to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Vector2( Vector3 rhs )
		{
			x = rhs.x; y = rhs.y;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Initialize the vector from an integer vector.
		/// </summary>
		/// <param name="rhs"> The vector to copy </param>
		///////////////////////////////////////////////////////////////////////
		public Vector2( Vector2i rhs )
		{
			x = (float)rhs.x; y = (float)rhs.y;
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Set the x and y components of the vector.
		/// </summary>
		/// <param name="nX"> The x component </param>
		/// <param name="nY"> The y component </param>
		///////////////////////////////////////////////////////////////////////
		public void Set( float nX, float nY )
		{
			x = nX; y = nY;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Negate the vector
		/// </summary>
		/// <param name="inVect"> The vector to negate </param>
		/// <returns> The negated vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector2 operator -( Vector2 inVect )
		{
			return new Vector2( -inVect.x, -inVect.y );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if another vector "nearly equals" vector.  This function tries
		/// to compensate for floating point precision by testing if the components
		/// are "close enough" to each other.
		/// </summary>
		/// <param name="rhs"> The vector to test against</param>
		/// <returns> Whether or not the vectors are equal to each other, within a treshold </returns>
		///////////////////////////////////////////////////////////////////////
		public bool NearlyEquals( Vector2 rhs )
		{
			const float V2_Epsilon = 0.00048828125f;
			return NearlyEquals( rhs, V2_Epsilon );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if another vector "nearly equals" vector.  This function tries
		/// to compensate for floating point precision by testing if the components
		/// are "close enough" to each other.
		/// </summary>
		/// <param name="rhs"> The vector to test against</param>
		/// <param name="threshold"> The threshold to test against </param>
		/// <returns> Whether or not the vectors are equal to each other, within a treshold </returns>
		///////////////////////////////////////////////////////////////////////
		public bool NearlyEquals( Vector2 rhs, float threshold )
		{
			// Get the differences
			float dX = x - rhs.x;
			float dY = y - rhs.y;

			// See if the vectors are close enough
			if( dX < threshold && dX > -threshold
				&& dY < threshold && dY > -threshold )
				return true;

			return false;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the dot product of two vectors.
		/// </summary>
		/// <param name="rhs"> The vector to dot with </param>
		/// <returns> The dot product </returns>
		///////////////////////////////////////////////////////////////////////
		public float Dot( Vector2 rhs )
		{
			return ((x * rhs.x) + (y * rhs.y));
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// The cross product of 2 vectors.  Since we know this vector lies in 2-space, we know the
		/// cross product will give us a normal to that plane.  The return value is
		/// is the magnitude in that direction
		/// </summary>
		/// <param name="rhs"> The vector to cross with </param>
		/// <returns> The scalar of the z component of the (0,0,1) 3D vector </returns>
		///////////////////////////////////////////////////////////////////////
		public float Cross( Vector2 rhs )
		{
			return ((x * rhs.y) - (rhs.x * y));
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Access the components via an index
		/// </summary>
		/// <param name="index"> The index to access the components, 0 for x and any other index for y </param>
		/// <returns> The value of the component accessed </returns>
		///////////////////////////////////////////////////////////////////////
		public float this[ int index ]
		{
			get
			{
				// On the 0 index return the x component
				if( index == 0 )
					return x;

				// On other indices return the y component
				return y;
			}

			set
			{
				// On the 0 index set the x component
				if( index == 0 )
					x = value;
					// On other indices set the y component
				else
					y = value;
			}
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Test if this is the zero vector
		/// </summary>
		/// <returns> Whether or not this is the zero vector </returns>
		///////////////////////////////////////////////////////////////////////
		public bool IsZeroVector()
		{
			return (x == 0.0f) && (y == 0.0f);
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Rotate the vector -90 degress based on a cartesian coordinate system with +x to the "right" and +y "up"
		/// </summary>
		/// <returns> The rotated vector </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector2 GetOrthoN90()
		{
			return new Vector2( y, -x );
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Rotate the vector 90 degress based on a cartesian coordinate system with +x to the "right" and +y "up"
		/// </summary>
		/// <returns> The rotated vector </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector2 GetOrtho90()
		{
			return new Vector2( -y, x );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the magnitude of the vector
		/// </summary>
		/// <returns> The magnitude of the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public float GetMag()
		{
			return (float)Math.Sqrt( (double)((x * x) + (y * y)) );
		}
		
		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the squared magnitude of the vector.  This function does not
		/// contain a call to the square root function and can be used for
		/// optimizations.
		/// </summary>
		/// <returns> The squared magnitude of the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public float GetMagSq()
		{
			return ((x * x) + (y * y));
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a normalized copy of the vector while leaving this vector
		/// unchanged.
		/// </summary>
		/// <returns> This vector normalized </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector2 Normalized()
		{
			// Get the magnitude and ensure it is not 0
			Vector2 retV = new Vector2( 0, 0 );
			float mag = GetMag();
			if( mag == 0.0f )
				return retV;

			// Get the reciprocal magnitude
			mag = 1.0f / mag;

			// Normalize the vector
			retV.x = x * mag;
			retV.y = y * mag;
			return retV;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Normalize this vector
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		void Normalize()
		{
			// Get the magnitude and ensure it is not 0
			float mag = GetMag();
			if( mag == 0.0f )
				return;

			// Get the reciprocal magnitude
			mag = 1.0f / mag;

			// Normalize the vector
			x *= mag;
			y *= mag;
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Project this vector onto another vector.
		/// </summary>
		/// <param name="proj"> The vector to project onto </param>
		/// <returns> The projected vector </returns>
		///////////////////////////////////////////////////////////////////////
		Vector2 ProjectOnto( Vector2 proj )
		{
			// If the magnitude is 0, bail
			if( proj.GetMagSq() == 0.0f )
				return new Vector2( 0,0 );

			float scalar = Dot( proj ) / proj.GetMagSq();
			return (proj * scalar);
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary> Rotate the vector </summary>
		/// <param name="degrees"> The amount to rotate in degrees </param>
		////////////////////////////////////////////////////////////////////////
		void RotateDeg( float degrees )
		{
			const float PI_OVER_180 = 0.01745329252f;
			RotateRad( degrees * PI_OVER_180 );
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary> Rotate the vector </summary>
		/// <param name="radians"> The amount to rotate in radians </param>
		////////////////////////////////////////////////////////////////////////
		void RotateRad( float radians )
		{
			// Calculate the sine and cosine
			float cosVal = (float)Math.Cos( (double)radians );
			float sinVal = (float)Math.Sin( (double)radians );

			// Rotate the point
			float tX = (cosVal * x) - (sinVal * y);
			y = (cosVal * y) + (sinVal * x);
			x = tX;
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the vector that represents this vector reflected about the
		/// passed in vector
		/// </summary>
		/// <param name="reflectVect"> The vector to reflect about </param>
		/// <returns> The reflected vector </returns>
		////////////////////////////////////////////////////////////////////////
		Vector2 ReflectAcrossVector( Vector2 reflectVect )
		{
			return ( (reflectVect * 2 * Dot(reflectVect)) - this );
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Add two vectors
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="rhs"> The second vector </param>
		/// <returns> The vector representing the sum of the two input vectors </returns>
		////////////////////////////////////////////////////////////////////////
		public static Vector2 operator + ( Vector2 lhs, Vector2 rhs )
		{
			return new Vector2( lhs.x + rhs.x, lhs.y + rhs.y );
		}

		
		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Subtract two vectors
		/// </summary>
		/// <param name="lhs"> The first vector </param>
		/// <param name="rhs"> The second vector </param>
		/// <returns> The vector representing the subtraction of the vectors</returns>
		////////////////////////////////////////////////////////////////////////
		public static Vector2 operator - ( Vector2 lhs, Vector2 rhs )
		{
			return new Vector2( lhs.x - rhs.x, lhs.y - rhs.y );
		}


		////////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Divide the components of a vector by a scalar
		/// </summary>
		/// <param name="lhs"> The vector to divide </param>
		/// <param name="den"> The scalar </param>
		/// <returns> The vector with scaled components </returns>
		////////////////////////////////////////////////////////////////////////
		public static Vector2 operator / ( Vector2 lhs, float den )
		{
			return new Vector2( lhs.x / den, lhs.y / den );
		}

		
		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Scale a vector by a scalar.
		/// </summary>
		/// <param name="lhs"> The vector to scale </param>
		/// <param name="scalar"> The amount to scale </param>
		/// <returns> The scaled vector </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector2 operator * ( Vector2 lhs, float scalar )
		{
			return new Vector2( lhs.x * scalar, lhs.y * scalar );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get a string representing the vector in the format (x,y)
		/// </summary>
		/// <returns> A string representing the vector </returns>
		///////////////////////////////////////////////////////////////////////
		public override string ToString()
		{
			return String.Format( "({0:G},{1:G})", x, y );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary> Parse a vector from a string </summary>
		/// <param name="str"> The string to parse </param>
		/// <returns> The vector stored in the string </returns>
		///////////////////////////////////////////////////////////////////////
		public static Vector2 Parse( string str )
		{
			// Ensure a long enough string starting with an open parenthesis
			// (0,0) 5 char min 
			if( str.Length < 5 && str[0] == '(' )
				throw new FormatException( "Vector2 string not formatted correctly" );

			Vector2 retVect = new Vector2();

			// Copy the string to work with
			string subStr = str.Substring( 1 );

			// Read in the parameters
			char[] delims = { ',', ')' };
			for( int paramIndex = 0; paramIndex < delims.Length; ++paramIndex )
			{
				// Get the value string
				int delimitPos = subStr.IndexOf( delims[paramIndex] );
				if( delimitPos < 0 )
					throw new FormatException( "Vector2 string not formatted correctly" );
				string valStr = subStr.Substring( 0, delimitPos );

				// Get the value from the string
				float paramValue = float.Parse( valStr );

				// Store the value
				if( paramIndex == 0 )
					retVect.x = paramValue;
				else if( paramIndex == 1 )
					retVect.y = paramValue;

				// Remove the parameter we just read in from the string and continue
				subStr = subStr.Substring( delimitPos + 1 );
			}

			// Return the vector
			return retVect;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Write the vector out to a binary stream
		/// </summary>
		/// <param name="binWriter"> The binary writer to use for outputting </param>
		///////////////////////////////////////////////////////////////////////
		public void ToBinary( BinaryWriter binWriter )
		{
			binWriter.Write( x );
			binWriter.Write( y );
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Read the vector in from a binary stream
		/// </summary>
		/// <param name="binReader"> The binary reader to retrieve the data
		/// from </param>
		///////////////////////////////////////////////////////////////////////
		public void FromBinary( BinaryReader binReader )
		{
			x = binReader.ReadSingle();
			y = binReader.ReadSingle();
		}
	}
}
