
#ifndef _JET_UTIL_UTIL_H
#define _JET_UTIL_UTIL_H

#include <string.h>
#if defined(_DEBUG) || defined(DEBUG)
#include <assert.h>
#define _s_l_(x) #x
#define _str_line_(x) _s_l_(x)
#define __STR_LINE__ _str_line_(__LINE__)
#define __STR_FILE__ _str_line_(__FILE__)
#define CHECK_GL_ERROR {GLenum error = glGetError(); if(error != 0) printf("Error Ocurred£ºFilename = %s, Line =  %s\n", __STR_FILE__, __STR_LINE__);}
#else
#define assert(x) x
#define CHECK_GL_ERROR
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