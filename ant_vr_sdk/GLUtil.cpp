#include "GLUtil.h"
#include <GL\glew.h>

#if defined(WIN32) || defined(WINDOWS)
#include <GLFW\glfw3.h>
#endif
#include "Numeric.h"
#include "Util.h"

namespace jet
{
	namespace util
	{

		static const int GL_VERSIONS[][6] = 
		{
//			110, 120, 130, 140, 150, 300, 330, 400, 410, 420, 430, 440, 450
			{100, 110, 120, 130, 140, 150},
			{200, 210},
			{300, 310, 320, 330},
			{400, 410, 420, 430, 440, 450}
		};

		static const int GLES_VERSIONS[][3] = 
		{
			{100},
			{200},
			{300, 310,320}
		};

		static const int GLSL_VERSIONS[][6] =
		{
			{ 0 },   // 1.x
			{ 110, 120 },  // 2.x
			{ 130, 140, 150, 330},  // 3.x
			{400, 410, 420, 430, 440, 450} // 4.x
		};

		static const int GLSLES_VERSIONS[][3] =
		{
			{ 100 },   // 2.0
			{ 300, 310, 320 },  // 3.x
		};

		int GLVersion::toGLSLInt()
		{
			return es ? GLSLES_VERSIONS[mojar - 2][minor]: GLSL_VERSIONS[mojar - 1][minor];
		}

		std::string GLVersion::toGLSLString()
		{
			return Numeric::toString(toGLSLInt());
		}

		std::string GLVersion::toString()
		{
			return Numeric::toString(toInt());
		}

		int GLVersion::toInt()
		{
			return es ? GLES_VERSIONS[mojar - 1][minor] : GL_VERSIONS[mojar - 1][minor];
		}

		const GLVersion GetOpenGLVersion()
		{
			static GLVersion g_GLVersion;

			if (g_GLVersion.mojar == 0)
			{
				const GLubyte* version = glGetString(GL_VERSION);
				std::string strings = (const char*)version;
				auto end = strings.find_first_of(' ');
				std::string _version = strings.substr(0, end);
				auto dot = _version.find_first_of('.');
				auto next = _version.find_first_of('.', dot + 1);

				g_GLVersion.mojar = Numeric::parseInt(_version.substr(0, dot).c_str());
				g_GLVersion.minor = Numeric::parseInt(_version.substr(dot + 1, next).c_str());
			}

			return g_GLVersion;
		}

		static GLboolean CheckExtension(char *extName, const char *extString)
		{
			/*
			** Search for extName in the extensions string.  Use of strstr()
			** is not sufficient because extension names can be prefixes of
			** other extension names.  Could use strtok() but the constant
			** string returned by glGetString can be in read-only memory.
			*/
			char *p = (char *)extString;
			char *end;
			int extNameLen;

			extNameLen = strlen(extName);
			end = p + strlen(p);

			while (p < end) {
				int n = strcspn(p, " ");
				if ((extNameLen == n) && (strncmp(extName, p, n) == 0)) {
					return GL_TRUE;
				}
				p += (n + 1);
			}
			return GL_FALSE;
		}

		bool IsExternsionAvaiable(const char* extName)
		{
#if defined(WIN32) || defined(WINDOWS)
			return glfwExtensionSupported(extName)!=0;
#else
			return CheckExtension(extName, glGetString(GL_EXTENSIONS));
#endif
		}

		void checkGLError()
		{
			GLenum errorCode = glGetError();
			assert(errorCode == 0);
		}

		GLUtil::GLUtil()
		{
		}


		GLUtil::~GLUtil()
		{
		}

	}
}
