#ifndef E_MATH_H
#define E_MATH_H
	//======================================================================
	// TIP (Quick View Volume Query)
	//======================================================================
	// 1) take camera matrix "out" vector
	// 2) convert point into camera space by subtracting camera position
	// 3) do a dot product of the point to be tested and the out vector
	// 4) if the result is < FOV then the point is in cone/frustum
	//======================================================================
	// ignore some warnings
	#pragma warning( disable : 4003 ) // not enough params for GMTL_STATIC_ASSERT

	//======================================================================
	#include "gmtl_AABox.h"
	#include "gmtl_AABoxOps.h"
	#include "gmtl_AxisAngle.h"
	#include "gmtl_AxisAngleOps.h"
	#include "gmtl_Containment.h"
	#include "gmtl_Coord.h"
	#include "gmtl_CoordOps.h"
	#include "gmtl_Defines.h"
	#include "gmtl_EulerAngle.h"
	#include "gmtl_EulerAngleOps.h"
	#include "gmtl_Generate.h"
	#include "gmtl_Intersection.h"
	#include "gmtl_LineSeg.h"
	#include "gmtl_LineSegOps.h"
	#include "gmtl_Math.h"
	#include "gmtl_Matrix.h"
	#include "gmtl_MatrixOps.h"
	#include "gmtl_Output.h"
	#include "gmtl_Plane.h"
	#include "gmtl_PlaneOps.h"
	#include "gmtl_Point.h"
	#include "gmtl_PointOps.h"
	#include "gmtl_Quat.h"
	#include "gmtl_QuatOps.h"
	#include "gmtl_Ray.h"
	#include "gmtl_Sphere.h"
	#include "gmtl_SphereOps.h"
	#include "gmtl_Tri.h"
	#include "gmtl_TriOps.h"
	#include "gmtl_VecBase.h"
	#include "gmtl_Vec.h"
	#include "gmtl_VecOps.h"
	#include "gmtl_Version.h"
	#include "gmtl_Xforms.h"

	//======================================================================
	using namespace gmtl;

	//======================================================================
	#include "j_MathTypes.h"
	#include "j_MathFunc.h"
	#include "j_MathFast.h"

	//======================================================================

#endif