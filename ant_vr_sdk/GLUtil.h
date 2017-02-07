#pragma once

#include <string>

#if defined(_DEBUG) || defined(DEBUG)
#include <assert.h>
#define _s_l_(x) #x
#define _str_line_(x) _s_l_(x)
#define __STR_LINE__ _str_line_(__LINE__)
#define __STR_FILE__ _str_line_(__FILE__)
#define CHECK_GL_ERROR {GLenum error = glGetError(); if(error != 0) printf("Error Ocurred£ºFilename = %s, Line =  %s, Error_Str = %s\n", __STR_FILE__, __STR_LINE__, GetGLErrorString(error));}
#define CHECK_GL(x) x; CHECK_GL_ERROR
#else
#define assert(x) x
#define CHECK_GL_ERROR
#define CHECK_GL(x) x;
#endif

namespace jet
{
	namespace util
	{
		typedef struct GLVersion 
		{
			int mojar;
			int minor;
			// Indicate the Open ES platform If true.
			bool es;

			GLVersion() :mojar(0), minor(0), es(false){}

			std::string toString() const ;
			int toInt() const;
			/*
			int toGLSLInt();
			std::string toGLSLString();
			*/
		}GLVersion;

		/// Get the current version of the opengl. 
		const GLVersion GetOpenGLVersion();

		bool IsExternsionAvaiable(const char* extName);

		const char* GetOpenGLVersionString();

		const char* GetGLErrorString(int error_code);
		void checkGLError();

		class GLUtil
		{
		public:
			GLUtil();
			~GLUtil();


		};
	}
}


