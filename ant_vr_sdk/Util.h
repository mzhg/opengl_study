
#ifndef _JET_UTIL_UTIL_H
#define _JET_UTIL_UTIL_H

#include <string.h>
#include "GLUtil.h"

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
		extern "C"
		{
			void ToLowercase(char* pDst, const char* pIn, int length = -1);
			void ToUppercase(char* pDst, const char* pIn, int length = -1);

			void debugString(const char* p);

		}
	}
}

#endif