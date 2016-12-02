#pragma once

#include <string>

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

			std::string toString();
			int toInt();
			int toGLSLInt();
			std::string toGLSLString();
		}GLVersion;

		/// Get the current version of the opengl. 
		const GLVersion GetOpenGLVersion();

		bool IsExternsionAvaiable(const char* extName);

		const char* GetOpenGLVersionString();

		void checkGLError();

		class GLUtil
		{
		public:
			GLUtil();
			~GLUtil();


		};
	}
}


