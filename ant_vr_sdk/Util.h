
#ifndef _JET_UTIL_UTIL_H
#define _JET_UTIL_UTIL_H

#include <string.h>
#include "GLUtil.h"

#define TO_STR(x)  #x

#define GEN_LESS_COMPARE(a, b, x)  \
	if(a.x < b.x)\
		return true; \
		else if(a.x > b.x)\
		return false;

#include "Numeric.h"



namespace jet
{
	namespace util
	{
		

#if defined(_DEBUG)
#define ASSERT(x, msg) Assert(x, msg)
#else
#define ASSERT(x, msg)  assert(x)
#endif

		extern "C"
		{
			void Assert(bool condiation, const char* msg);
			void ToLowercase(char* pDst, const char* pIn, int length = -1);
			void ToUppercase(char* pDst, const char* pIn, int length = -1);

			void debugString(const char* p);

		}
	}
}

#endif