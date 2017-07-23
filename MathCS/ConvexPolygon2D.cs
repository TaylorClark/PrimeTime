using System;

namespace TCMath
{
	/// <summary>
	/// A description for a closed convex 2D polygon.  The vertices should be
	/// passed in in clockwise order
	/// </summary>
	public class ConvexPolygon2D
	{
		private Point2[] m_Vertices = new Point2[ 0 ];

		private readonly Point2 m_Barycenter = new Point2();

		public ConvexPolygon2D( Point2[] verts )
		{
			// Ensure valid data
			if( verts.Length < 3 )
				throw new Exception( "Number of vertices too small." );

			// Store the vertices
			m_Vertices = verts;

			// Calculate the barycenter
			float baryX = 0, baryY = 0;
			float scalar = 1.0f / (float)NumVertices;
			foreach( Point2 vert in m_Vertices )
			{
				baryX += vert.x * scalar;
				baryY += vert.y * scalar;
			}

			// Store the barycenter
			m_Barycenter.Set( baryX, baryY );
		}


		public bool ContainsPoint( Point2 testPt )
		{
			// Go around each edge
			for( int vertIndex = 0; vertIndex < NumVertices; ++vertIndex )
			{
				// Get a vector from the center to the test
				Vector2 toTestPt = testPt - m_Vertices[ vertIndex ];

				// Get the index of the next point on this edge
				int nextVertIndex = vertIndex + 1;
				if( nextVertIndex == NumVertices )
					nextVertIndex = 0;

				// Create this edge
				Vector2 edge = m_Vertices[ nextVertIndex ] - m_Vertices[ vertIndex ];

				// Make it normal
				edge = edge.GetOrtho90();

				// Test if we are outside
				if( toTestPt.Dot( edge ) > 0.0f )
					return false;
			}

			// If we got here then we are okay
			return true;
		}


		///////////////////////////////////////////////////////////////////////
		/// <summary>
		/// Get the number of vertices used by the polygon
		/// </summary>
		///////////////////////////////////////////////////////////////////////
		public int NumVertices
		{
			get{ return m_Vertices.Length; }
		}
	}
}
