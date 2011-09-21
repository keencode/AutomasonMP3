#ifndef E_FRUSTUM_H
#define E_FRUSTUM_H
	//======================================================================	
	#include <windows.h>
	#include <Gl/gl.h>

	//======================================================================
	#include "j_Math.h"
	
	//======================================================================
	enum CullResult { outside=0, inside=1, intersects=2 };

	//======================================================================
	class JFrustum
	{
	public:
		//--------------------------------------------------------
		JFrustum(void) 
		{
			view_dir.set(0.f,0.f,1.f);
			view_pos.set(0.f,0.f,0.f);
		}

		//--------------------------------------------------------
		// test whether or not the frustum planes contain or 
		// intersects the given sphere.
		//--------------------------------------------------------
		CullResult Test_Point(Point3f pos)
		{
			float distance = 0.f;
			for(unsigned int i=0; i<6; ++i)
			{
				distance = (data[i][0] * pos[0]) + 
									 (data[i][1] * pos[1]) +
									 (data[i][2] * pos[2]) + data[i][3];
				if( distance < 0.f) return outside;
			}
			return inside;
		}

		//--------------------------------------------------------
		// test whether or not the frustum planes contains or 
		// intersects the given sphere
		//--------------------------------------------------------
		CullResult Test_Sphere(Point3f center,float radius) 
		{
			float distance = 0.f;
			for(unsigned int i=0; i<6; ++i)
			{
				distance = (data[i][0] * center[0]) + 
									 (data[i][1] * center[1]) +
									 (data[i][2] * center[2]) + data[i][3];
				//distance = dot(Vec3f(data[i][0],data[i][1],data[i][2]),Vec3f(center)) + data[i][3];

				if(distance < -radius) return outside;
				if(fabs(distance) < radius) return intersects;
			}
			return inside;
		}
		
		//--------------------------------------------------------
		// test whether or not the frustum planes contains or 
		// intersects the given box. (todo)
		//--------------------------------------------------------
		CullResult Test_Box(Point3f min, Point3f max)
		{ return inside; }

		//--------------------------------------------------------
		// tests whether the frustum's bounding sphere and cone
		// enclose/intersect the point
		//--------------------------------------------------------
		CullResult QuickTest_Point(Point3f pos) 
		{ 

			const float distance = ((view_pos[0] - pos[0])*(view_pos[0] - pos[0]))+ 
														 ((view_pos[1] - pos[1])*(view_pos[1] - pos[1]))+
														 ((view_pos[2] - pos[2])*(view_pos[2] - pos[2]));

			// first test to see if point is in sphere
			if(distance <= (sphere.mRadius*sphere.mRadius))
			{
				// test against cone
				// ...
			}				
			return outside; 
		}

		//--------------------------------------------------------
		// tests whether the frustum's bounding sphere and cone
		// enclose/intersect the box
		//--------------------------------------------------------
		CullResult QuickTest_Box(Point3f min, Point3f max) 
		{ return inside; }

		//--------------------------------------------------------
		// tests whether the frustum's bounding sphere and cone
		// enclose/intersect the sphere
		//--------------------------------------------------------
		CullResult QuickTest_Sphere(Point3f center,float radius) 
		{ 
			// get the sum of radii
			const float rads = sphere.mRadius+radius;

			// find distance between centers
			const float distance = fSqrt(((sphere.mCenter[0] - center[0])*(sphere.mCenter[0] - center[0]))+ 
														 ((sphere.mCenter[1] - center[1])*(sphere.mCenter[1] - center[1]))+
														 ((sphere.mCenter[2] - center[2])*(sphere.mCenter[2] - center[2])));

			// first test to see if point is in sphere
			if(distance <= (sphere.mRadius + radius))
			{
				//-----------------------------------------------------------
				// Cone Intersection v1
				//-----------------------------------------------------------
				// test our sphere against a cone
				Vec3f U = view_pos - (sphere.mRadius/sinf(view_fov)) * view_dir;
				Vec3f D = center - U;

				if(dot(view_dir,D) >= (length(D)*cosf(view_fov)))
				{
					D = center - view_pos;
					if(dot(view_dir,D) >= (length(D)*sinf(view_fov)))
					{
						if(length(D) <= radius)
						{	return inside; }
					}
					return intersects;
				}


				//-----------------------------------------------------------
				// Cone Intersection v2
				//---------------------------------------------------------
				// hack - it's currently not taking the bounding radius
				// into account and seems hacky since I can't specify a
				// natural cutoff angle.
				//---------------------------------------------------------
				/*Vec3f obj_vec(center-view_pos);

				// find distance between center * view pos
				const float dist = fSqrt(((view_pos[0] - center[0])*(view_pos[0] - center[0]))+ 
																((view_pos[1]  - center[1])*(view_pos[1] - center[1]))+
																((view_pos[2]  - center[2])*(view_pos[2] - center[2])));

				// dot product to find opposite side
				float dp = dot(view_dir,obj_vec);

				// find minimum angle between vectors
				float pheta = asin((dp/dist)) * RADTODEG;

				printf("pheta: %f\n",(90.f-fabs(pheta)));
				if((90.f-fabs(pheta)) <= view_fov)
				{ printf("cone clipped\n"); return inside; }
				else { printf("sphere clipped\n"); }*/
				return inside;
			}
			return outside; 
		}
		
		//--------------------------------------------------------
		// builds the frustum from the OpenGL model and projection
		// matrices. only used when using the frustum for viewport
		// culling.
		//--------------------------------------------------------
		void BuildPlanesFromGL(void)
		{
			float p[16];   // projection matrix
			float mv[16];  // model-view matrix
			float mvp[16]; // model-view-projection matrix
			float t;

			glGetFloatv( GL_PROJECTION_MATRIX, p );
			glGetFloatv( GL_MODELVIEW_MATRIX, mv );

			// Concatenate the projection matrix and the model-view matrix to produce 
			// a combined model-view-projection matrix.
			mvp[ 0] = mv[ 0] * p[ 0] + mv[ 1] * p[ 4] + mv[ 2] * p[ 8] + mv[ 3] * p[12];
			mvp[ 1] = mv[ 0] * p[ 1] + mv[ 1] * p[ 5] + mv[ 2] * p[ 9] + mv[ 3] * p[13];
			mvp[ 2] = mv[ 0] * p[ 2] + mv[ 1] * p[ 6] + mv[ 2] * p[10] + mv[ 3] * p[14];
			mvp[ 3] = mv[ 0] * p[ 3] + mv[ 1] * p[ 7] + mv[ 2] * p[11] + mv[ 3] * p[15];

			mvp[ 4] = mv[ 4] * p[ 0] + mv[ 5] * p[ 4] + mv[ 6] * p[ 8] + mv[ 7] * p[12];
			mvp[ 5] = mv[ 4] * p[ 1] + mv[ 5] * p[ 5] + mv[ 6] * p[ 9] + mv[ 7] * p[13];
			mvp[ 6] = mv[ 4] * p[ 2] + mv[ 5] * p[ 6] + mv[ 6] * p[10] + mv[ 7] * p[14];
			mvp[ 7] = mv[ 4] * p[ 3] + mv[ 5] * p[ 7] + mv[ 6] * p[11] + mv[ 7] * p[15];

			mvp[ 8] = mv[ 8] * p[ 0] + mv[ 9] * p[ 4] + mv[10] * p[ 8] + mv[11] * p[12];
			mvp[ 9] = mv[ 8] * p[ 1] + mv[ 9] * p[ 5] + mv[10] * p[ 9] + mv[11] * p[13];
			mvp[10] = mv[ 8] * p[ 2] + mv[ 9] * p[ 6] + mv[10] * p[10] + mv[11] * p[14];
			mvp[11] = mv[ 8] * p[ 3] + mv[ 9] * p[ 7] + mv[10] * p[11] + mv[11] * p[15];

			mvp[12] = mv[12] * p[ 0] + mv[13] * p[ 4] + mv[14] * p[ 8] + mv[15] * p[12];
			mvp[13] = mv[12] * p[ 1] + mv[13] * p[ 5] + mv[14] * p[ 9] + mv[15] * p[13];
			mvp[14] = mv[12] * p[ 2] + mv[13] * p[ 6] + mv[14] * p[10] + mv[15] * p[14];
			mvp[15] = mv[12] * p[ 3] + mv[13] * p[ 7] + mv[14] * p[11] + mv[15] * p[15];


			// Extract the frustum's right clipping plane and normalize it.
			data[0][0] = mvp[ 3] - mvp[ 0];
			data[0][1] = mvp[ 7] - mvp[ 4];
			data[0][2] = mvp[11] - mvp[ 8];
			data[0][3] = mvp[15] - mvp[12];

			t = (float) fSqrt(data[0][0] * data[0][0] + 
												data[0][1] * data[0][1] + 
												data[0][2] * data[0][2] );

			data[0][0] /= t;
			data[0][1] /= t;
			data[0][2] /= t;
			data[0][3] /= t;

			// Extract the frustum's left clipping plane and normalize it.
			data[1][0] = mvp[ 3] + mvp[ 0];
			data[1][1] = mvp[ 7] + mvp[ 4];
			data[1][2] = mvp[11] + mvp[ 8];
			data[1][3] = mvp[15] + mvp[12];

			t = (float) fSqrt(data[1][0] * data[1][0] + 
												data[1][1] * data[1][1] + 
												data[1][2] * data[1][2] );

			data[1][0] /= t;
			data[1][1] /= t;
			data[1][2] /= t;
			data[1][3] /= t;

			// Extract the frustum's bottom clipping plane and normalize it.
			data[2][0] = mvp[ 3] + mvp[ 1];
			data[2][1] = mvp[ 7] + mvp[ 5];
			data[2][2] = mvp[11] + mvp[ 9];
			data[2][3] = mvp[15] + mvp[13];

			t = (float) fSqrt(data[2][0] * data[2][0] + 
												data[2][1] * data[2][1] + 
												data[2][2] * data[2][2] );

			data[2][0] /= t;
			data[2][1] /= t;
			data[2][2] /= t;
			data[2][3] /= t;

			// Extract the frustum's top clipping plane and normalize it.
			data[3][0] = mvp[ 3] - mvp[ 1];
			data[3][1] = mvp[ 7] - mvp[ 5];
			data[3][2] = mvp[11] - mvp[ 9];
			data[3][3] = mvp[15] - mvp[13];

			t = (float) fSqrt(data[3][0] * data[3][0] + 
												data[3][1] * data[3][1] + 
												data[3][2] * data[3][2] );

			data[3][0] /= t;
			data[3][1] /= t;
			data[3][2] /= t;
			data[3][3] /= t;

			// Extract the frustum's far clipping plane and normalize it.
			data[4][0] = mvp[ 3] - mvp[ 2];
			data[4][1] = mvp[ 7] - mvp[ 6];
			data[4][2] = mvp[11] - mvp[10];
			data[4][3] = mvp[15] - mvp[14];

			t = (float) fSqrt(data[4][0] * data[4][0] +  
												data[4][1] * data[4][1] + 
												data[4][2] * data[4][2] );

			data[4][0] /= t;
			data[4][1] /= t;
			data[4][2] /= t;
			data[4][3] /= t;

			// Extract the frustum's near clipping plane and normalize it.
			data[5][0] = mvp[ 3] + mvp[ 2];
			data[5][1] = mvp[ 7] + mvp[ 6];
			data[5][2] = mvp[11] + mvp[10];
			data[5][3] = mvp[15] + mvp[14];

			t = (float) fSqrt(data[5][0] * data[5][0] + 
												data[5][1] * data[5][1] + 
												data[5][2] * data[5][2] );

			data[5][0] /= t;
			data[5][1] /= t;
			data[5][2] /= t;
			data[5][3] /= t;
		}

		//--------------------------------------------------------
		void BuildTestVolumes(Point3f pos,Vec3f out_vec,float fov,float n_plane,float f_plane)
		{
			// store the view position & out vector
			view_dir = out_vec;
			view_pos = pos;
			view_fov = fov;

			// quick, but inaccurate
			//const float diameter = (f_plane-n_plane) / cosf(fov);
			//sphere.mRadius = diameter * 0.5f;			
			//sphere.mCenter = view_pos + (out_vec * -sphere.mRadius);

			const float len = f_plane - n_plane;
			const float height = len * tanf(fov * 0.5f);
			
			Vec3f P(0,0, n_plane + (len * 0.5f));
			Vec3f Q(height,height,len);

			sphere.mRadius = length(P-Q);
			sphere.mCenter = view_pos + (view_dir * (len * 0.5f));
		}

	private:
		//--------------------------------------------------------
		float			data[6][4];
		Spheref		sphere;
		Vec3f			view_dir;
		Point3f		view_pos;
		float     view_fov;
	};
	//======================================================================

#endif