#ifndef E_MATH_FAST_H
#define E_MATH_FAST_H
	//======================================================================
	#pragma warning (disable : 4305)
	#pragma warning (disable : 4244)

	//======================================================================
	#include <float.h>
	#include <math.h>

	//======================================================================
	#include "j_MathTypes.h"

	//======================================================================
	// Sine Method 1																											//
	//======================================================================
	inline float fSin0(float fT)
	{
			// assert:  0 <= fT <= PI/2
			// maximum absolute error = 1.6415e-04
			// speedup = 1.91

			float fTSqr = fT*fT;
			float fResult = 7.61e-03f;
			fResult *= fTSqr;
			fResult -= 1.6605e-01f;
			fResult *= fTSqr;
			fResult += 1.0f;
			fResult *= fT;
			return fResult;
	}

	//======================================================================
	// Sine Method 2																											//
	//======================================================================
	inline float fSin1(float fT)
	{
			// assert:  0 <= fT <= PI/2
			// maximum absolute error = 2.3279e-09
			// speedup = 1.40

			float fTSqr = fT*fT;
			float fResult = -2.39e-08f;
			fResult *= fTSqr;
			fResult += 2.7526e-06f;
			fResult *= fTSqr;
			fResult -= 1.98409e-04f;
			fResult *= fTSqr;
			fResult += 8.3333315e-03f;
			fResult *= fTSqr;
			fResult -= 1.666666664e-01f;
			fResult *= fTSqr;
			fResult += 1.0f;
			fResult *= fT;
			return fResult;
	}

	//======================================================================
	// Cosine Method 1																										//
	//======================================================================
	inline float fCos0(float fT)
	{
			// assert:  0 <= fT <= PI/2
			// maximum absolute error = 1.1880e-03
			// speedup = 2.14

			float fTSqr = fT*fT;
			float fResult = 3.705e-02f;
			fResult *= fTSqr;
			fResult -= 4.967e-01f;
			fResult *= fTSqr;
			fResult += 1.0f;
			return fResult;
	}

	//======================================================================
	// Cosine Method 2																										//
	//======================================================================
	inline float fCos1(float fT)
	{
			// assert:  0 <= fT <= PI/2
			// maximum absolute error = 2.3082e-09
			// speedup = 1.47

			float fTSqr = fT*fT;
			float fResult = -2.605e-07f;
			fResult *= fTSqr;
			fResult += 2.47609e-05f;
			fResult *= fTSqr;
			fResult -= 1.3888397e-03f;
			fResult *= fTSqr;
			fResult += 4.16666418e-02f;
			fResult *= fTSqr;
			fResult -= 4.999999963e-01f;
			fResult *= fTSqr;
			fResult += 1.0f;
			return fResult;
	}

	//======================================================================
	// Tangent Method 1																										//
	//======================================================================
	inline float fTan0(float fT)
	{
			// assert:  0 <= fT <= PI/4
			// maximum absolute error = 8.0613e-04
			// speedup = 2.51

			float fTSqr = fT*fT;
			float fResult = 2.033e-01f;
			fResult *= fTSqr;
			fResult += 3.1755e-01f;
			fResult *= fTSqr;
			fResult += 1.0f;
			fResult *= fT;
			return fResult;
	}

	//======================================================================
	// Tangent Method 2																										//
	//======================================================================
	inline float fTan1(float fT)
	{
			// assert:  0 <= fT <= PI/4
			// maximum absolute error = 1.8897e-08
			// speedup = 1.71

			float fTSqr = fT*fT;
			float fResult = 9.5168091e-03f;
			fResult *= fTSqr;
			fResult += 2.900525e-03f;
			fResult *= fTSqr;
			fResult += 2.45650893e-02f;
			fResult *= fTSqr;
			fResult += 5.33740603e-02f;
			fResult *= fTSqr;
			fResult += 1.333923995e-01f;
			fResult *= fTSqr;
			fResult += 3.333314036e-01f;
			fResult *= fTSqr;
			fResult += 1.0f;
			fResult *= fT;
			return fResult;
	}

	//======================================================================
	// Inverse Tangent Method 1																						//
	//======================================================================
	inline float fInvTan0(float fT)
	{
			// assert:  |fT| <= 1
			// maximum absolute error = 4.8830e-03
			// speedup = 2.14

			float fTSqr = fT*fT;
			return fT/(1.0f + 0.28f*fTSqr);
	}

	//======================================================================
	// Inverse Tangent Method 2																						//
	//======================================================================
	inline float fInvTan1(float fT)
	{
			// assert:  |fT| <= 1
			// maximum absolute error = 1.1492e-05
			// speedup = 2.16

			float fTSqr = fT*fT;
			float fResult = 0.0208351f;
			fResult *= fTSqr;
			fResult -= 0.085133f;
			fResult *= fTSqr;
			fResult += 0.180141f;
			fResult *= fTSqr;
			fResult -= 0.3302995f;
			fResult *= fTSqr;
			fResult += 0.999866f;
			fResult *= fT;
			return fResult;
	}

	//======================================================================
	// Inverse Tangent Method 3																						//
	//======================================================================
	inline float fInvTan2(float fT)
	{
			// assert:  |fT| <= 1
			// maximum absolute error = 1.3593e-08
			// speedup = 1.50

			float fTSqr = fT*fT;
			float fResult = 0.0028662257f;
			fResult *= fTSqr;
			fResult -= 0.0161657367f;
			fResult *= fTSqr;
			fResult += 0.0429096138f;
			fResult *= fTSqr;
			fResult -= 0.0752896400f;
			fResult *= fTSqr;
			fResult += 0.1065626393f;
			fResult *= fTSqr;
			fResult -= 0.1420889944f;
			fResult *= fTSqr;
			fResult += 0.1999355085f;
			fResult *= fTSqr;
			fResult -= 0.3333314528f;
			fResult *= fTSqr;
			fResult += 1.0f;
			fResult *= fT;
			return fResult;
	}

	//======================================================================
	// Inverse Sine Method 1																							//
	//======================================================================
	inline float fInvSin0(float fT)
	{
			// assert:  0 <= fT <= 1
			// maximum absolute error = 6.7626e-05
			// speedup = 2.59 (apparently asin is much more expensive than sqrt)
			float fRoot = sqrtf(1.0f-fT);

			float fResult = -0.0187293f;
			fResult *= fT;
			fResult += 0.0742610f;
			fResult *= fT;
			fResult -= 0.2121144f;
			fResult *= fT;
			fResult += 1.5707288f;
			fResult = HALFPI - fRoot*fResult;
			return fResult;
	}

	//======================================================================
	// Inverse Cosine Method 1																						//
	//======================================================================
	inline float fInvCos0(float fT)
	{
			// assert:  0 <= fT <= 1
			// maximum absolute error = 6.7626e-05
			// speedup = 2.59
			float fRoot = sqrtf(1.0f-fT);

			float fResult = -0.0187293f;
			fResult *= fT;
			fResult += 0.0742610f;
			fResult *= fT;
			fResult -= 0.2121144f;
			fResult *= fT;
			fResult += 1.5707288f;
			fResult *= fRoot;
			return fResult;
	}

	//======================================================================
	// Reciprocal Square Root																							//
	//======================================================================
	inline float fRSqrt(float number)
	{
		long i;
		float x2, y;
		const float threehalfs = 1.5f;

		x2 = number * 0.5f;
		y  = number;
		i  = * (long *) &y;											// evil floating point bit level hacking
		i  = 0x5f3759df - (i >> 1);             // what the fuck?
		y  = * (float *) &i;
		y  = y * (threehalfs - (x2 * y * y));   // 1st iteration

		return y;
	}
	//======================================================================
	// SSE Square Root																										//
	//======================================================================
	inline float fSqrt(float num)
	{
		_asm
		{
			MOVSS xmm2,num
			SQRTSS xmm1,xmm2
			MOVSS num,xmm1
		}
		return num;
	}

	//======================================================================
	// Find the Closest Power of 2																				//
	//======================================================================
	inline int fNextPowerOfTwo(int N)
	{
		if (N<=0L				) return 0;
		if (N<=1L				) return 1;
		if (N<=2L				) return 2;
		if (N<=4L				) return 4;
		if (N<=8L				) return 8;
		if (N<=16L	    ) return 16;
		if (N<=32L	    ) return 32;
		if (N<=64L 	    ) return 64;
		if (N<=128L     ) return 128;
		if (N<=256L     ) return 256;
		if (N<=512L     ) return 512;
		if (N<=1024L    ) return 1024;
		if (N<=2048L    ) return 2048;
		if (N<=4096L    ) return 4096;
		if (N<=8192L    ) return 8192;
		if (N<=16384L   ) return 16384;
		if (N<=32768L   ) return 32768;
		if (N<=65536L		) return 65536;
		else			  return 0;
	}

	//======================================================================

#endif