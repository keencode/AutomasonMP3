#ifndef E_MATH_FUNC_H
#define E_MATH_FUNC_H
	//======================================================================
	#pragma warning (disable : 4305)
	#pragma warning (disable : 4244)

	//======================================================================
	#include <float.h>
	#include <math.h>
	#include <stdlib.h>

	//======================================================================
	#include "j_MathTypes.h"

	//======================================================================
	// math routines: integer																							//
	//======================================================================
	inline int IAbs		(int iValue);
	inline int ICeil	(float fValue);
	inline int IFloor (float fValue);
	inline int ISign	(int iValue);

	//======================================================================
	// math routines: real (float/double)																	//
	//======================================================================
	inline float Abs	(float fValue);
	inline float ACos (float fValue);
	inline float ASin (float fValue);
	inline float ATan (float fValue);
	inline float ATan2(float fY, float fX);
	inline float Ceil (float fValue);
	inline float Cos	(float fValue);
	inline float Exp	(float fValue);
	inline float Floor(float fValue);
	inline float Log	(float fValue);
	inline float Pow	(float kBase, float kExponent);
	inline float Sign (float fValue);
	inline float Sin	(float fValue);
	inline float Sqr	(float fValue);
	inline float Sqrt (float fValue);
	inline float Min	(float a,float b);
	inline float Max	(float a,float b);
	inline float UnitRandom ();			 // in [0,1]
	inline float SymmetricRandom (); // in [-1,1]

	//======================================================================
	//  Integer Absolute Value																						//
	//======================================================================
	inline int IAbs(int iValue)
	{ return(iValue >= 0 ? iValue : -iValue); }

	//======================================================================
	//  Integer Ceiling																										//
	//======================================================================
	inline int ICeil(float fValue)
	{ return int(ceil(fValue)); }

	//======================================================================
	//  Integer Floor																											//
	//======================================================================
	inline int IFloor(float fValue)
	{ return int(floor(fValue));}

	//======================================================================
	//  Integer Sign Change																								//
	//======================================================================
	inline int ISign(int iValue)
	{ return(iValue > 0 ? +1 : (iValue<0 ? -1:0)); }

	//======================================================================
	//  Float Absolute Value																							//
	//======================================================================
	inline float Abs(float fValue)
	{ return float(fabs(fValue)); }

	//======================================================================
	//  Float Arc Cosine																									//
	//======================================================================
	inline float ACos(float fValue)
	{
			if(-1.0 < fValue)
			{
				if(fValue < 1.0)
					return float(acos(fValue));
				else
					return 0.0;
			}
			else
			{ return PI; }
	}

	//======================================================================
	//  Float Arc Sine																										//
	//======================================================================
	inline float ASin(float fValue)
	{
		if(-1.0 < fValue)
		{
			if(fValue < 1.0)
				return float(asin(fValue));
			else
				return -HALFPI;
		}
		else
		{ return HALFPI; }
	}

	//======================================================================
	//  Float Arc Tangent of x																						//
	//======================================================================
	inline float ATan(float fValue)
	{ return float(atan(fValue)); }

	//======================================================================
	//  Float Arc Tangent of y/x																					//
	//======================================================================
	inline float ATan2(float fY, float fX)
	{ return float(atan2(fY,fX)); }

	//======================================================================
	//  Float Ceiling																											//
	//======================================================================
	inline float Ceil(float fValue)
	{ return float(ceil(fValue)); }

	//======================================================================
	//  Float Cosine																											//
	//======================================================================
	inline float Cos(float fValue)
	{ return float(cos(fValue)); }

	//======================================================================
	//  Float Exponential																									//
	//======================================================================
	inline float Exp(float fValue)
	{ return float(exp(fValue)); }

	//======================================================================
	//  Float Floor																												//
	//======================================================================
	inline float Floor(float fValue)
	{ return float(floor(fValue)); }

	//======================================================================
	//  Float Logarithms																									//
	//======================================================================
	inline float Log(float fValue)
	{ return float(log(fValue)); }

	//======================================================================
	//  Float Power																												//
	//======================================================================
	inline float Pow(float fBase, float fExponent)
	{ return float(pow(fBase,fExponent)); }

	//======================================================================
	//  Float Sign																												//
	//======================================================================
	inline float Sign(float fValue)
	{
		if(fValue > 0.0)	return 1.0;
		if(fValue < 0.0)	return -1.0;
		return 0.0;
	}

	//======================================================================
	//  Float Sine																												//
	//======================================================================
	inline float Sin(float fValue)
	{ return float(sin(fValue)); }

	//======================================================================
	//  Float Square																											//
	//======================================================================
	inline float Sqr(float fValue)
	{ return fValue*fValue; }

	//======================================================================
	//  Float Square Root																									//
	//======================================================================
	inline float Sqrt(float fValue)
	{ return float(sqrt(fValue)); }

	//======================================================================
	//  Min of two values																									//
	//======================================================================
	inline float Min(float a, float b)
	{	return (a<b) ? a:b; }

	//======================================================================
	//  Min of two values																									//
	//======================================================================
	inline float Max(float a, float b)
	{	return (a>b) ? a:b; }

	//======================================================================
	//  Float Random Number [0->1]																				//
	//======================================================================
	inline float UnitRandom()
	{ return float(rand())/float(RAND_MAX); }

	//======================================================================
	//  Random Number [-1->1]																							//
	//======================================================================
	inline float SymmetricRandom()
	{ return 2.0*float(rand())/float(RAND_MAX) - 1.0; }

	//======================================================================
#endif