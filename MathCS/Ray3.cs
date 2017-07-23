using System;

namespace TCMath
{
	//---------------------------------------------------------------------------------------------
	/// <summary>
	/// A class that represents the a 3 dimensional ray.  It consists of a point that stores the
	/// location of the ray and a vector storing the direction the ray is pointing.
	/// </summary>
	//---------------------------------------------------------------------------------------------
	public struct Ray3
	{
		/// <summary> The position of the ray </summary>
		public Point3 pos;

		/// <summary> The direction of the ray </summary>
		public Vector3 dir;


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> A basic constructor  </summary>
		/// <param name="inPos"> The position of the ray </param>
		/// <param name="inDir"> The direction of the ray </param>
		///////////////////////////////////////////////////////////////////////////////////////////
		public Ray3( Point3 inPos, Vector3 inDir )
		{
			// Store the data
			pos = inPos;
			dir = inDir;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> Get the time of intersection with a plane.  The equation to get the time of
		/// intersection between a ray and plane is
		/// t = -((ray.pos - planePos) * planeNorm)/(ray.dir * planeNorm) </summary>
		/// <param name="planeData"> The plane position and normal </param>
		/// <param name="intTime"> The time scalar of when the ray intersects the plane defined by
		/// the data passed in </param>
		/// <returns> Whether or not the the ray actually intersects the plane.  It would not
		/// intersect the plane if it is perpendicular to the plane. </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public bool GetPlaneIntersect( Ray3 planeData, ref float intTime )
		{
			return GetPlaneIntersect( planeData.pos, planeData.dir, ref intTime );
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> Get the time of intersection with a plane.  The equation to get the time of
		/// intersection between a ray and plane is
		/// t = -((ray.pos - planePos) * planeNorm)/(ray.dir * planeNorm) </summary>
		/// <param name="planePos"> The plane position (||planePos - O|| = D of the plane
		/// equation) </param>
		/// <param name="planeNormal"> The plane normal (A, B, C of the plane equation) </param>
		/// <param name="intTime"> The time scalar of when the ray intersects the plane defined by
		/// the data passed in </param>
		/// <returns> Whether or not the the ray actually intersects the plane.  It would not
		/// intersect the plane if it is perpendicular to the plane. </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public bool GetPlaneIntersect( Point3 planePos, Vector3 planeNormal, ref float intTime )
		{
			// Get the dot product to test if the ray is perpendicular to the plane
			float dirDotNorm = dir.Dot( planeNormal );
			if( dirDotNorm == 0.0f )
				return false;

			// Get the intersection time
			intTime = -(pos - planePos).Dot( planeNormal ) / dirDotNorm;

			// Since we aren't perpendicular we intersected
			return true;
		}


		///////////////////////////////////////////////////////////////////////////////////////////
		/// <summary> Get a position from a ray scaled </summary>
		/// <param name="scalar"> The value to use as a scalar for the direction vector </param>
		/// <returns> The position represented by the position of the ray added to the direction
		/// scaled by the passed in value </returns>
		///////////////////////////////////////////////////////////////////////////////////////////
		public Point3 GetScalarPos( float scalar )
		{
			// Return the origin plus the direction scaled by the scalar
			return pos + (dir * scalar);
		}
	}
}
