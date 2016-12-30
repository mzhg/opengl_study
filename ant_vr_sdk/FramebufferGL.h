#pragma once

#include "Disposeable.h"
#include "Util.h"
#include <GL\glew.h>

namespace jet
{
	namespace util
	{
		class FramebufferGL :public Disposeable
		{
		public:
			FramebufferGL(/*GLuint width, GLuint height*/);
			~FramebufferGL();

		private:
			GLuint m_Framebuffer;
		};
	}
}

