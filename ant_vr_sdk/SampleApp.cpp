#include "SampleApp.h"
#include <GL\glew.h>
#include "TextureUtil.h"
#include "Util.h"

namespace jet
{
	namespace util
	{
		SampleApp::SampleApp() : m_Framebuffer(0), m_DepthTexture(0), m_StencilTexture(0), X(0), Y(0), Width(0), Height(0)
		{
			for (int i = 0; i < _countof(m_ColorTextures); i++)
			{
				m_ColorTextures[i] = 0;
			}
		}

		SampleApp::~SampleApp()
		{
			SAFE_RELEASE_FRAMEBUFFER(m_Framebuffer);
			SAFE_DISPOSE(m_DepthTexture);
			SAFE_DISPOSE(m_StencilTexture);

			for (int i = 0; i < _countof(m_ColorTextures); i++)
			{
				SAFE_DISPOSE(m_ColorTextures[i]);
			}
		}

		void SampleApp::Create() 
		{
			InitConfig(mOutputDesc);
			int cmp = TextureUtil::measureComponent(mOutputDesc.DepthStencilFormat);
			assert(cmp == GL_DEPTH_COMPONENT || cmp == GL_STENCIL || cmp == GL_DEPTH_STENCIL);
			OnCreate();
		}

		void SampleApp::InitConfig(SampleOutputDesc& desc)
		{
			desc.ColorCount = 1;
			desc.ColorFormats[0] = GL_RGBA8;
			desc.DepthStencilFormat = GL_DEPTH_COMPONENT16_ARB;
			desc.StencilOnly = false;
		}

		void SampleApp::Resize(int x, int y, int width, int height) {
			X = x;
			Y = y;
			Width = width;
			Height = height;

			if (width == 0 || height == 0)
				return;

			bool needReAttachTextures = false;
			if (m_Framebuffer == 0)
			{
				glGenFramebuffers(1, &m_Framebuffer);
				needReAttachTextures = true;
			}
			// Resize the Color Textures.
			for (int i = 0; i < mOutputDesc.ColorCount; i++)
			{
				if (needReAttachTextures || !m_ColorTextures[i] || (m_ColorTextures[i]->getWidth() != width || m_ColorTextures[i]->getHeight() != height))
				{
					Texture2DDesc colorDesc = Texture2DDesc(width, height, static_cast<GLuint>(mOutputDesc.ColorFormats[i]));
					SAFE_DISPOSE(m_ColorTextures[i]);
					m_ColorTextures[i] = new Texture2D();
					TextureUtil::createTexture2D(&colorDesc, nullptr, m_ColorTextures[i]);
					needReAttachTextures = true;
				}
			}

			if (needReAttachTextures)
			{
				if (mOutputDesc.DepthStencilFormat != GL_NONE)
				{
					Texture2DDesc depthDesc = Texture2DDesc(width, height, static_cast<GLuint>(mOutputDesc.DepthStencilFormat));
					SAFE_DISPOSE(m_DepthTexture);
					m_DepthTexture = new Texture2D();

					TextureUtil::createTexture2D(&depthDesc, nullptr, m_DepthTexture);
				}
				
				if (mOutputDesc.StencilOnly)
				{
					int component = TextureUtil::measureComponent(mOutputDesc.DepthStencilFormat);
					bool containStencil = ((component == GL_STENCIL) || (component == GL_DEPTH_STENCIL));
					if (!containStencil)
					{
						Texture2DDesc stencilDesc = Texture2DDesc(width, height, GL_STENCIL_INDEX8);
						SAFE_DISPOSE(m_StencilTexture);
						m_StencilTexture = new Texture2D();

						TextureUtil::createTexture2D(&stencilDesc, nullptr, m_StencilTexture);
					}
				}

				glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
				{
					GLenum drawbuffers[8];
					for (int i = 0; i < mOutputDesc.ColorCount; i++)
					{
						GLenum drawbuffer = GL_COLOR_ATTACHMENT0 + i;
						glFramebufferTexture(GL_FRAMEBUFFER, drawbuffer, m_ColorTextures[i]->getTexture(), 0);
						drawbuffers[i] = drawbuffer;
					}

					if (m_DepthTexture != nullptr)
					{
						int component = TextureUtil::measureComponent(mOutputDesc.DepthStencilFormat);
						int attachment;
						if (component == GL_DEPTH_COMPONENT)
						{
							attachment = GL_DEPTH_ATTACHMENT;
						}
						else if (component == GL_DEPTH_STENCIL)
						{
							attachment = GL_DEPTH_STENCIL_ATTACHMENT;
						}
						else if (component == GL_STENCIL)
						{
							attachment = GL_STENCIL_ATTACHMENT;
						}
						else
						{
							assert(false);
						}
						glFramebufferTexture(GL_FRAMEBUFFER, attachment, m_DepthTexture->getTexture(), 0);
					}

					if (m_StencilTexture)
					{
						glFramebufferTexture(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, m_StencilTexture->getTexture(), 0);
					}

					if (mOutputDesc.ColorCount)
					{
						glDrawBuffers(mOutputDesc.ColorCount, &drawbuffers[0]);
					}
					else
					{
						glDrawBuffer(GL_NONE);
					}
				}
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}

			OnResize(x, y, width, height);
		}

		void SampleApp::Render(float elpsedTime)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
			OnRender(elpsedTime);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		}
	}
}
