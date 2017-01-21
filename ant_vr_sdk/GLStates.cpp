#include "GLStates.h"
#include <GLCapabilities.h>

namespace jet
{
	namespace util
	{

		static const BlendDesc g_BlendDefault = BlendDesc();
		static const DepthStencilOPDesc g_DSStateDefault = DepthStencilOPDesc();
		static const RasterizerDesc g_RSStateDefault = RasterizerDesc();

		GLStates& GLStates::getGLStates()
		{
			static GLStates instance;
			return instance;
		}

		void GLStates::setBlendState(const BlendDesc* pBlend)
		{

		}
		void GLStates::setDSState(const DepthStencilOPDesc* pDS)
		{

		}
		void GLStates::setRSState(const RasterizerDesc* pRaster)
		{

		}

		void GLStates::bindProgram(GLuint program)
		{

		}
		void GLStates::bindFramebuffer(GLuint framebuffer)
		{

		}

		void GLStates::bindTextures(unsigned int count, const TextureGL* pTextures, const int* units)
		{

		}

		GLStates::GLStates()
		{
		}


		GLStates::~GLStates()
		{
		}
	}
}
