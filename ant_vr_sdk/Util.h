
#ifndef _JET_UTIL_UTIL_H
#define _JET_UTIL_UTIL_H

#include <string.h>
#if defined(_DEBUG) || defined(DEBUG)
#include <assert.h>
#else
#define assert(x) x
#endif

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