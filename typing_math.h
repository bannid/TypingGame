/* date = May 26th 2023 8:24 pm */

#ifndef TYPING_MATH_H
#define TYPING_MATH_H
#include "typedefines.h"
struct v2f
{
	union
	{
		f32 v[2];
		struct
		{
			f32 x;
			f32 y;
		};
	};
};

struct v3f
{
	union
	{
		f32 v[3];
		struct 
		{
			f32 x;
			f32 y;
			f32 z;
		};
	};
};

struct v4f
{
	union
	{
		f32 v[4];
		struct
		{
			f32 x;
			f32 y;
			f32 z;
			f32 w;
		};
	};
};

struct m4f
{
	union
	{
		f32 m[16];
		struct
		{
			f32 _00, _10, _20, _30;
			f32 _01, _11, _21, _31;
			f32 _02, _12, _22, _32;
			f32 _03, _13, _23, _33;
		};
		struct
		{
			v4f Row1;
			v4f Row2;
			v4f Row3;
			v4f Row4;
		};
	};
};

#endif //TYPING_MATH_H
