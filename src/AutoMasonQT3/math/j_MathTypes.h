#ifndef E_MATH_TYPES_H
#define E_MATH_TYPES_H
	//======================================================================
	// frequently used numerical constants																//
	//======================================================================
	static const float	PI				=	3.14159265358979323846f;	// PI
	static const float  HALFPI		=	1.57079632679489661923f;	// PI / 2
	static const float  TWOPI			=	6.28318530717958647692f;	// PI * 2
	static const float  INVPI			=	0.31830988618379067154f;	// 1 / PI
	static const float	EPSILON		= 0.00000000000000000001f;

	static const float  RADTODEG	=	57.2957795130823208768f;	// convert radians to degrees
	static const float  DEGTORAD	=	0.01745329251994329577f;	// convert degrees to radians

	static const float  EXP				=	2.71828182845904523536f;	// e
	static const float  ILOG2			=	3.32192809488736234787f;	// 1.0 / log10(2)
	static const float  INV255		=	0.00392156862745098039f;	// 1/255

	//========================================================================
	struct UByte4 
	{ 
		//------------------------------------------------------
		unsigned char rgba[4]; 
		UByte4() { rgba[0]=rgba[1]=rgba[2]=rgba[3]=255; }

		//------------------------------------------------------
		UByte4 operator = (const UByte4 &v) 
		{ 
			rgba[0]=v.rgba[0];	rgba[1]=v.rgba[1];
			rgba[2]=v.rgba[2];	rgba[3]=v.rgba[3];
			return *this;
		}
			
		//------------------------------------------------------
		bool operator == (const UByte4 &v) 
		{ 
			if((rgba[0]==v.rgba[0])&&(rgba[1]==v.rgba[1])&&
				(rgba[2]==v.rgba[2])&&(rgba[3]==v.rgba[3]))
			{ return true; }
			else 
			{ return false; }
		}
	};

	//======================================================================
#endif