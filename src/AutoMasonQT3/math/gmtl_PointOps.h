#ifndef _GMTL_POINT_OPS_H_
#define _GMTL_POINT_OPS_H_
//======================================================================
#include "j_MathFunc.h"
#include "j_MathFast.h"
#include "gmtl_Defines.h"
#include "gmtl_Math.h"
#include "gmtl_Point.h"
#include <math.h>
//========================================================================
// routines weren't provided by GMTL so they were added.								 
//========================================================================
namespace gmtl
{
	//======================================================================
	template<class DATA_TYPE, unsigned SIZE>
	Point<DATA_TYPE, SIZE>& operator +=(Point<DATA_TYPE, SIZE>& p1,
																				const Point<DATA_TYPE, SIZE>& p2)
	{
		for(unsigned i=0;i<SIZE;++i) {	p1[i] += p2[i];	}
		return p1;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE>
	Point<DATA_TYPE, SIZE> operator +(const Point<DATA_TYPE, SIZE>& p1,
																			const Point<DATA_TYPE, SIZE>& p2)
	{
		VecBase<DATA_TYPE, SIZE> ret_val(p1);
		ret_val += p2; return ret_val;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE>
	Point<DATA_TYPE, SIZE>& operator -=(Point<DATA_TYPE, SIZE>& p1,
																				const Point<DATA_TYPE, SIZE>& p2)
	{
		for(unsigned i=0;i<SIZE;++i) {	p1[i] -= p2[i]; }
		return p1;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE>
	inline bool operator==(const Point<DATA_TYPE, SIZE>& p1,
												 const Point<DATA_TYPE, SIZE>& p2)
	{
		for(unsigned i=0;i<SIZE;++i)
		{	if(p1[i] != p2[i]){	return false;	}	}
		return true;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE>
	inline bool operator!=(const Point<DATA_TYPE, SIZE>& p1,
												 const Point<DATA_TYPE, SIZE>& p2)
	{	return(! (p1 == p2));	}

	//======================================================================
	template < class DATA_TYPE, unsigned SIZE>
	Point<DATA_TYPE, SIZE> operator -(const Point<DATA_TYPE, SIZE>& p1,
																		const Point<DATA_TYPE, SIZE>& p2)
	{
		Point<DATA_TYPE, SIZE> ret_val(p1);
		ret_val -= p2; return ret_val;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE, class SCALAR_TYPE>
	Point<DATA_TYPE, SIZE>& operator *=(Point<DATA_TYPE, SIZE>& p1,
																			const SCALAR_TYPE& scalar)
	{
		for(unsigned i=0;i<SIZE;++i) {	p1[i] *= (DATA_TYPE)scalar;	}
		return p1;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE, class SCALAR_TYPE>
	Point<DATA_TYPE, SIZE> operator *(const Point<DATA_TYPE, SIZE>& p1,
																		const SCALAR_TYPE& scalar)
	{
		Point<DATA_TYPE, SIZE> ret_val(p1);
		ret_val *= scalar;	return ret_val;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE, class SCALAR_TYPE>
	Point<DATA_TYPE, SIZE> operator *(const SCALAR_TYPE& scalar,
																		const Point<DATA_TYPE, SIZE>& p1)
	{
		Point<DATA_TYPE, SIZE> ret_val(p1);
		ret_val *= scalar;	return ret_val;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE, class SCALAR_TYPE>
	Point<DATA_TYPE, SIZE>& operator /=(Point<DATA_TYPE, SIZE>& p1,
																			const SCALAR_TYPE& scalar)
	{
		for(unsigned i=0;i<SIZE;++i) {	p1[i] /= scalar; }
		return p1;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE, class SCALAR_TYPE>
	Point<DATA_TYPE, SIZE> operator /(const Point<DATA_TYPE, SIZE>& p1,
																		const SCALAR_TYPE& scalar)
	{
		Point<DATA_TYPE, SIZE> ret_val(p1);
		ret_val /= scalar;	return ret_val;
	}

	//======================================================================
	template<class DATA_TYPE, unsigned SIZE>
	DATA_TYPE dot(const Point<DATA_TYPE, SIZE>& p1, const Point<DATA_TYPE, SIZE>& p2)
	{
		DATA_TYPE ret_val(0);
		for(unsigned i=0;i<SIZE;++i) {	ret_val += (p1[i] * p2[i]);	}
		return ret_val;
	}

	//======================================================================
	// unit random vector	[0,1]																						 
	//======================================================================
	inline Point3f UnitRandomPoint3(void)
	{ return Point3f(UnitRandom(),UnitRandom(),UnitRandom());	}

	//======================================================================
	// symmetric random vector [-1,1]																			 
	//======================================================================
	inline Point3f SymRandomPoint3(void)
	{ return Point3f(SymmetricRandom(),SymmetricRandom(),SymmetricRandom());	}

	//======================================================================
	// linear interpolation between two points. 													 
	//======================================================================
	inline Point3f Lerp(const Point3f& a,const Point3f& b,const float t)
	{	
		return Point3f(a[0] + t * (b[0] - a[0]),
									 a[1] + t * (b[1] - a[1]),
									 a[2] + t * (b[2] - a[2]));	
	}

	//======================================================================
	// Hermite interpolation between p1 & p2. p0 & p3 are									 
	// used to find the gradient for p1 & p2.															 
	//======================================================================
	inline Point3f Herp(const Point3f& p0,const Point3f& p1,
										  const Point3f& p2,const Point3f& p3,const float t)
	{
		Point3f ret;
		const float t2 = t * t;
		const float t3 = t2 * t;
		const float kp0 = (2.0f * t2 - t3 - t) * 0.5f;
		const float kp1 = (3.0f * t3 - 5.0f * t2 + 2.0f) * 0.5f;
		const float kp2 = (4.0f * t2 - 3.0f * t3 + t) * 0.5f;
		const float kp3 = (t3 - t2) * 0.5f;
		ret[0] = p0[0] * kp0 + p1[0] * kp1 + p2[0] * kp2 + p3[0] * kp3;
		ret[1] = p0[1] * kp0 + p1[1] * kp1 + p2[1] * kp2 + p3[1] * kp3;
		ret[2] = p0[2] * kp0 + p1[2] * kp1 + p2[2] * kp2 + p3[2] * kp3;
		return ret;
	}

	//======================================================================
	// swap two point 3's																									 
	//======================================================================
	inline void Swap(Point3f &a, Point3f &b) 
	{ Point3f tmp(a); a = b; b = tmp; }

	//======================================================================
	// distance between two points																				 
	//======================================================================
	inline float Distance(Point3f &a, Point3f &b)
	{ 
		return fSqrt(((b[0] - a[0])*(b[0] - a[0]))+ 
								 ((b[1] - a[1])*(b[1] - a[1]))+
								 ((b[2] - a[2])*(b[2] - a[2])));
	}

	//======================================================================
	// returns the minimum/maximum component
	//======================================================================
	inline Point3f PMin(Point3f& a,Point3f& b) 
	{ return Point3f(Min(a[0],b[0]),
									 Min(a[1],b[1]),
									 Min(a[2],b[2])); }

	//======================================================================
	inline Point3f PMax(Point3f& a,Point3f& b) 
	{ 
		return Point3f(Max(a[0],b[0]),
									 Max(a[1],b[1]),
									 Max(a[2],b[2])); }

	//======================================================================

} // end namespace gtml

#endif
