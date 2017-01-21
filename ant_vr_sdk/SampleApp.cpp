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

			SAFE_DELETE(m_Timer);
			SAFE_DELETE(m_Transformer);
		}

		void SampleApp::Create() 
		{
			InitConfig(mOutputDesc);
			int cmp = TextureUtil::measureComponent(mOutputDesc.DepthStencilFormat);
			assert(mOutputDesc.NoFBO || cmp == GL_DEPTH_COMPONENT || cmp == GL_STENCIL || cmp == GL_DEPTH_STENCIL);

			m_Timer = NvStopWatch::create();
			m_Timer->start();
			m_Transformer = new NvInputTransformer();
			OnCreate();
		}

		void SampleApp::InitConfig(SampleOutputDesc& desc)
		{
			desc.ColorCount = 1;
			desc.ColorFormats[0] = GL_RGBA8;
			desc.DepthStencilFormat = GL_DEPTH_COMPONENT16_ARB;
			desc.StencilOnly = false;
			desc.NoFBO = false;
//			desc.EnableSRGB = true;
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
					int internalFormat = mOutputDesc.ColorFormats[i];
#if 0
					if (false && mOutputDesc.EnableSRGB)
					{
						if (mOutputDesc.ColorFormats[i] == GL_RGB8)
						{
							internalFormat = GL_SRGB8_EXT;
						}
						else if (mOutputDesc.ColorFormats[i] == GL_RGBA8)
						{
							internalFormat = GL_RGBA8_SNORM;
						}
						else if (mOutputDesc.ColorFormats[i] = GL_RGB16)
						{
							internalFormat = GL_RGB16_SNORM;
						}
						else if (mOutputDesc.ColorFormats[i] = GL_RGBA16)
						{
							internalFormat = GL_RGBA16_SNORM;
						}
					}
#endif
					Texture2DDesc colorDesc = Texture2DDesc(width, height, static_cast<GLuint>(internalFormat));
					SAFE_DISPOSE(m_ColorTextures[i]);
					m_ColorTextures[i] = new Texture2D();
					TextureUtil::createTexture2D(&colorDesc, nullptr, m_ColorTextures[i]);
					needReAttachTextures = true;
				}
			}
#if 0
			if (mOutputDesc.EnableSRGB && mOutputDesc.ColorCount)
			{
				glEnable(GL_FRAMEBUFFER_SRGB);
			}
#endif
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

		void SampleApp::Render(bool renderToFBO, float timeScale)
		{
			if (renderToFBO)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
			}

			m_Timer->stop();
			float elpsedTime = m_Timer->getTime() * timeScale;
			m_Transformer->update(elpsedTime);
			OnRender(elpsedTime);

			m_Timer->reset();
			m_Timer->start();
			
			if (renderToFBO)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
		}

		bool SampleApp::OnkeyPressed(int keycode, char keychar)
		{
			return m_Transformer->processKey(keycode, NvKeyActionType::DOWN);
		}
		bool SampleApp::OnkeyReleased(int keycode, char keychar)
		{
			return m_Transformer->processKey(keycode, NvKeyActionType::UP);
		}
		bool SampleApp::OnkeyTyped(int keycode, char keychar)
		{
			return m_Transformer->processKey(keycode, NvKeyActionType::REPEAT);
		}

		bool SampleApp::OnMousePressed(int x, int y, Button button)
		{
			NvPointerEvent event;
			event.m_device = NvInputDeviceType::MOUSE;
			event.m_id = static_cast<uint32_t>(button);
			event.m_x = x;
			event.m_y = y;

			return m_Transformer->processPointer(NvInputDeviceType::MOUSE, NvPointerActionType::DOWN, 0, 1, &event);
		}

		bool SampleApp::OnMouseReleased(int x, int y, Button button)
		{
			NvPointerEvent event;
			event.m_device = NvInputDeviceType::MOUSE;
			event.m_id = static_cast<uint32_t>(button);
			event.m_x = x;
			event.m_y = y;

			return m_Transformer->processPointer(NvInputDeviceType::MOUSE, NvPointerActionType::UP, 0, 1, &event);
		}

		bool SampleApp::OnMouseMoved(int x, int y, int dx, int dy)
		{
			NvPointerEvent event;
			event.m_device = NvInputDeviceType::MOUSE;
			event.m_id = 0;
			event.m_x = x;
			event.m_y = y;

			return m_Transformer->processPointer(NvInputDeviceType::MOUSE, NvPointerActionType::MOTION, 0, 1, &event);
		}

		bool SampleApp::OnMouseDraged(int x, int y, int dx, int dy, Button button)
		{
			NvPointerEvent event;
			event.m_device = NvInputDeviceType::MOUSE;
			event.m_id = static_cast<uint32_t>(button);
			event.m_x = x;
			event.m_y = y;

			return m_Transformer->processPointer(NvInputDeviceType::MOUSE, NvPointerActionType::MOTION, 0, 1, &event);
		}
	}
}
