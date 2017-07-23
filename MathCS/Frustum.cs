using System;

namespace TCMath
{
	
	/// <summary>
	/// Summary description for Frustum.
	/// </summary>
	public class Frustum
	{
		// The four rays making up the frustum, indexed by cartesian quadrant
		// order (0 being upper left (+x,+y), 1 being upper right, etc.)
		public Ray3[] m_Rays = new Ray3[ 4 ];

		public Frustum( Ray3[] rays )
		{
			m_Rays = rays;
		}

		public bool ContainsPoint( TCMath.Point3 pt )
		{
			return false;
		}

		public TCMath.Box3 BoundBox
		{
			get
			{
				// Go through all of the points and store min and max values
				TCMath.Vector3 minVals = m_Rays[0].dir;
				TCMath.Vector3 maxVals = m_Rays[0].dir;
				foreach( Ray3 edgeRay in m_Rays )
				{
					for( int i = 0; i < 3; ++i )
					{
						if( edgeRay.pos[i] < minVals[i] )
							minVals[i] = edgeRay.pos[i];
						if( edgeRay.pos[i] > maxVals[i] )
							maxVals[i] = edgeRay.pos[i];

						TCMath.Point3 endPt = edgeRay.pos + edgeRay.dir;
						if( endPt[i] < minVals[i] )
							minVals[i] = endPt[i];
						if( endPt[i] > maxVals[i] )
							maxVals[i] = endPt[i];
					}
				}

				TCMath.Point3 boxPos = new Point3( minVals.x, minVals.y, minVals.z );
				TCMath.Vector3 boxDims = maxVals - minVals;
				return new TCMath.Box3( boxPos, boxDims );
			}
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the dimensions of the view frustum at a distance from the near
		/// plane
		/// </summary>
		/// <param name="distFromNear"> The distance from the near plane </param>
		/// <returns> The dimensions of the 2D plane </returns>
		///////////////////////////////////////////////////////////////////////
		public Vector2 GetDimsAtDist( float distFromNear )
		{
			// Get the near plane data
			Ray3 intPlane = NearPlane;

			float w = (m_Rays[0].pos - m_Rays[1].pos).GetMag();
			float h = (m_Rays[1].pos - m_Rays[2].pos).GetMag();
			float ar = w/h;

			// Get the plane at the distance
			intPlane.pos += intPlane.dir * distFromNear;

			// Get the intersection of the rays
			float intTime = 0.0f;
			m_Rays[1].GetPlaneIntersect( intPlane, ref intTime );
			Point3 upperLeftPt = m_Rays[1].GetScalarPos( intTime );
			Point3 upperRightPt = m_Rays[0].GetScalarPos( intTime );
			Point3 lowerLeftPt = m_Rays[2].GetScalarPos( intTime );

			// Get the dims
			Vector2 retDims = new Vector2();
			retDims.x = (upperLeftPt - upperRightPt).GetMag();
			retDims.y = (upperLeftPt - lowerLeftPt).GetMag();

			return retDims;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the near plane of the frustom in point-normal form
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public Ray3 NearPlane
		{
			get
			{
				Ray3 retPlane = new Ray3();

				// Get the data for the plane
				TCMath.Vector3 baryCenter = new Vector3();
				TCMath.Vector3 calcPlaneNorm = new Vector3();
				foreach( Ray3 ray in m_Rays )
				{
					// Add the position
					baryCenter.x += ray.pos.x;
					baryCenter.y += ray.pos.y;
					baryCenter.z += ray.pos.z;

					// Add the direction
					calcPlaneNorm += ray.dir;
				}
				baryCenter /= 4.0f;
				retPlane.pos = new Point3( baryCenter.x, baryCenter.y, baryCenter.z );

				// Get the plane normal
				retPlane.dir = calcPlaneNorm.Normalized();

				// Return the plane data
				return retPlane;
			}
		}
	}
}
