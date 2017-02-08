#include "FramebufferGL.h"
#include <map>
#include "GLStates.h"
#include "TextureUtil.h"

namespace jet
{
	namespace util
	{
		static std::map<GLuint, FramebufferGL*> g_FBOCaches;

		static GLenum measureTextureAttachment(const TextureGL* pTex, GLuint index)
		{
			assert(pTex);
			assert(index < 8u);

			GLenum format_conponemt = TextureUtil::measureComponent(pTex->getFormat());
			switch (format_conponemt)
			{
			case GL_DEPTH_COMPONENT: return GL_DEPTH_ATTACHMENT;
			case GL_DEPTH_STENCIL:	 return GL_DEPTH_STENCIL_ATTACHMENT;
			case GL_STENCIL:		 return GL_STENCIL_ATTACHMENT;
			default:
				return GL_COLOR_ATTACHMENT0 + index;
			}
		}

		void FramebufferGL::addTextures(unsigned int count, const TextureGL** pTextures, const TextureAttachDesc* descs)
		{
			GLStates& states = GLStates::get();
			assert(m_Framebuffer);
			assert(states.getBindingFramebuffer() == m_Framebuffer);

			for (unsigned int i = m_AttachCount; i < count; i++)
			{
				const TextureGL* pTex = pTextures[i];
				const TextureAttachDesc& desc = descs[i];
				m_AttachedTextures[i] = (TextureGL*)pTex;
				switch (desc.Type)
				{
				case AttachType::TEXTURE:
					if (pTex)
					{
						glFramebufferTexture(GL_FRAMEBUFFER, measureTextureAttachment(pTex, desc.Index), pTex->getTexture(), desc.Level);
						m_AttachCount++;
					}
					else
					{
						// TODO
					}
					break;
				case AttachType::TEXTURE_1D:
					if (pTex)
					{
						assert(pTex->getTarget() == GL_TEXTURE_1D /*|| pTex->getTarget() == GL_TEXTURE_1D_ARRAY*/);
						glFramebufferTexture1D(GL_FRAMEBUFFER, measureTextureAttachment(pTex, desc.Index), GL_TEXTURE_1D, pTex->getTexture(), desc.Level);
						m_AttachCount++;
					}
					else
					{
						// TODO
					}
					break;
				case AttachType::TEXTURE_2D:
					if (pTex)
					{
						GLenum target = pTex->getTarget();
						assert(target == GL_TEXTURE_2D || target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_RECTANGLE ||
							target == GL_TEXTURE_2D_MULTISAMPLE);
						if (target == GL_TEXTURE_CUBE_MAP)
						{
							for (int i = 0; i < _countof(CUBE_FACES); i++)
							{
								glFramebufferTexture2D(GL_FRAMEBUFFER, measureTextureAttachment(pTex, desc.Index), CUBE_FACES[i], pTex->getTexture(), desc.Level);
							}
						}
						else
						{
							glFramebufferTexture2D(GL_FRAMEBUFFER, measureTextureAttachment(pTex, desc.Index), target, pTex->getTexture(), desc.Level);
						}
						m_AttachCount++;
					}
					else
					{
						// TODO
					}
					break;
				case AttachType::TEXTURE_3D:
					if (pTex)
					{
						assert(pTex->getTarget() == GL_TEXTURE_3D /*|| pTex->getTarget() == GL_TEXTURE_1D_ARRAY*/);
						glFramebufferTexture3D(GL_FRAMEBUFFER, measureTextureAttachment(pTex, desc.Index), GL_TEXTURE_3D, pTex->getTexture(), desc.Level, desc.Layer);
						m_AttachCount++;
					}
					else
					{
						// TODO
					}
					break;
				case AttachType::TEXTURE_LAYER:
					if (pTex)
					{
						GLenum target = pTex->getTarget();
						assert(target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY || target == GL_TEXTURE_1D_ARRAY
							|| target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY || target == GL_TEXTURE_CUBE_MAP_ARRAY);
						glFramebufferTextureLayer(GL_FRAMEBUFFER, measureTextureAttachment(pTex, desc.Index), pTex->getTexture(), desc.Level, desc.Layer);
						m_AttachCount++;
					}
					else
					{
						// TODO
					}
					break;
				default:
					break;
				}
			}
		}

		const Texture2D* FramebufferGL::addTexture2D(const Texture2DDesc& texDesc, const TextureAttachDesc& attachDesc)
		{
			Texture2D* tex2D = new Texture2D();
			TextureUtil::createTexture2D(&texDesc, nullptr, tex2D);
		
			m_Owed[m_AttachCount] = true;
			const TextureGL* texs[] = { tex2D };
			addTextures(1, texs, &attachDesc);
			return tex2D;
		}

		void FramebufferGL ::dispose()
		{
			if (m_Framebuffer)
			{
				glDeleteFramebuffers(1, &m_Framebuffer);
				g_FBOCaches.erase(m_Framebuffer);
				m_Framebuffer = 0;
			}

			for (int i = 0; i < _countof(m_Owed); i++)
			{
				if (m_Owed[i])
				{
					SAFE_DELETE(m_AttachedTextures[i]);
					m_Owed[i] = false;
				}
			}
		}

		void FramebufferGL::enable()
		{
			GLStates& states = GLStates::get();
			if (m_Framebuffer == 0)
			{
				glGenFramebuffers(1, &m_Framebuffer);
				g_FBOCaches.insert(std::pair<GLuint, FramebufferGL*>(m_Framebuffer, this));
			}

			states.bindFramebuffer(m_Framebuffer);
		}

		void FramebufferGL::disable()
		{
			GLStates& states = GLStates::get();
			states.resetFramebuffer();
		}

		FramebufferGL* FramebufferGL::getFramebufferObject(GLuint name)
		{
			auto it = g_FBOCaches.find(name);
			return it != g_FBOCaches.end() ? it->second : nullptr;
		}

		void RenderTargets::dispose()
		{
			if (m_Framebuffer)
			{
				glDeleteFramebuffers(1, &m_Framebuffer);
				m_Framebuffer = 0;
			}
		}

		void RenderTargets::enable()
		{
			GLStates& states = GLStates::get();
			if (m_Framebuffer == 0)
			{
				glGenFramebuffers(1, &m_Framebuffer);
//				g_FBOCaches.insert(std::pair<GLuint, FramebufferGL*>(m_Framebuffer, this));
			}

			states.bindFramebuffer(m_Framebuffer);
		}

		void RenderTargets::disable()
		{
			GLStates& states = GLStates::get();
			states.resetFramebuffer();
		}

		void RenderTargets::handleTextureAttachment(const TextureGL* pTex, GLenum attachment, const TextureAttachDesc& desc, AttachInfo& info)
		{
			info.type = desc.Type;
			switch (desc.Type)
			{
			case AttachType::TEXTURE_1D:
				if (pTex)
				{
					assert(pTex->getTarget() == GL_TEXTURE_1D);
					if (!info.attached || info.textureTarget != pTex->getTarget() || info.textureId != pTex->getTexture() || info.level != desc.Level)
					{
						glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, pTex->getTarget(), pTex->getTexture(), desc.Level);
						info.attached = true;
						info.textureTarget = pTex->getTarget();
						info.textureId = pTex->getTexture();
						info.level = desc.Level;
					}
					
				}
				else
				{
					if (info.attached)
					{
						glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_1D, 0, 0);
						info.attached = false;
						info.textureTarget = GL_TEXTURE_1D;
						info.textureId = 0;
					}
					
				}
				break;
			case AttachType::TEXTURE_3D:
				if (pTex)
				{
					assert(pTex->getTarget() == GL_TEXTURE_3D);
					if (!info.attached || info.textureTarget != pTex->getTarget() || info.textureId != pTex->getTexture() || info.level != desc.Level || info.layer != desc.Layer)
					{
						glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, pTex->getTarget(), pTex->getTexture(), desc.Level, desc.Layer);
						info.attached = true;
						info.textureTarget = pTex->getTarget();
						info.textureId = pTex->getTexture();
						info.level = desc.Level;
						info.layer = desc.Layer;
					}
				}
				else
				{
					if (info.attached)
					{
						glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, 0, 0, 0);
						info.attached = false;
						info.textureTarget = GL_TEXTURE_3D;
						info.textureId = 0;
						info.level = 0;
						info.layer = 0;
					}
					
				}
				break;
			case AttachType::TEXTURE_2D:
				if (pTex)
				{
					GLenum target = pTex->getTarget();
					assert(target == GL_TEXTURE_2D || target == GL_TEXTURE_CUBE_MAP || target == GL_TEXTURE_RECTANGLE || 
						   target == GL_TEXTURE_2D_MULTISAMPLE);
					if (!info.attached || info.textureTarget != pTex->getTarget() || info.textureId != pTex->getTexture() || info.level != desc.Level)
					{
						if (target == GL_TEXTURE_CUBE_MAP)
						{
							for (int i = 0; i < _countof(CUBE_FACES); i++)
							{
								glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, CUBE_FACES[i], pTex->getTexture(), desc.Level);
							}
						}
						else
						{
							glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, pTex->getTexture(), desc.Level);
						}

						info.attached = true;
						info.textureTarget = target;
						info.textureId = pTex->getTexture();
						info.level = desc.Level;
					}
				}
				else
				{
					if (info.attached)
					{
						glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, 0, 0);
						info.attached = false;
						info.textureTarget = GL_TEXTURE_2D;
						info.textureId = 0;
						info.level = 0;
					}
					
				}
				break;
			case AttachType::TEXTURE:
				if (pTex)
				{
					if (!info.attached || info.textureId != pTex->getTexture() || info.level != desc.Level)
					{
						glFramebufferTexture(GL_FRAMEBUFFER, attachment, pTex->getTexture(), desc.Level);
						info.attached = true;
						info.textureId = pTex->getTexture();
						info.level = desc.Level;
					}
				}
				else
				{
					if (info.attached)
					{
						glFramebufferTexture(GL_FRAMEBUFFER, attachment, 0, 0);
						info.attached = false;
						info.textureId = 0;
						info.level = 0;
					}
				}
				break;
			case AttachType::TEXTURE_LAYER:
				if (pTex)
				{
					GLenum target = pTex->getTarget();
					assert(target == GL_TEXTURE_3D || target == GL_TEXTURE_2D_ARRAY || target == GL_TEXTURE_1D_ARRAY
						|| target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY || target == GL_TEXTURE_CUBE_MAP_ARRAY);
					if (!info.attached || info.textureId != pTex->getTexture() || info.level != desc.Level || info.layer != desc.Layer)
					{
						glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, pTex->getTexture(), desc.Level, desc.Layer);
						info.attached = true;
						info.textureId = pTex->getTexture();
						info.level = desc.Level;
						info.layer = desc.Layer;
					}
				}
				else
				{
					if (info.attached)
					{
						glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, 0, 0, 0);
						info.attached = false;
						info.textureId = 0;
						info.level = 0;
						info.layer = 0;
					}
					
				}
				break;
			default:
				assert(false);
				break;
			}
		}

		static void deAttachTexture(GLenum attachment, AttachType type)
		{
			switch (type)
			{
			case jet::util::AttachType::TEXTURE_1D:
				glFramebufferTexture1D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_1D, 0, 0);
				break;
			case jet::util::AttachType::TEXTURE_2D:
				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, 0, 0);
				break;
			case jet::util::AttachType::TEXTURE_3D:
				glFramebufferTexture3D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, 0, 0, 0);
				break;
			case jet::util::AttachType::TEXTURE_LAYER:
				glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, 0, 0);
				break;
			case jet::util::AttachType::TEXTURE:
				glFramebufferTexture(GL_FRAMEBUFFER, attachment, 0, 0);
				break;
			default:
				break;
			}
		}

		void RenderTargets::setRenderTextures(unsigned int count, const TextureGL** pTextures, const TextureAttachDesc* descs)
		{
			bool depthHandled = false;
			bool depthStencilHandled = false;
			bool stencilHandled = false;
			bool colorHandled[8] = { false };

			enable();
			for (unsigned int i = 0; i < count; i++)
			{
				const TextureGL* pTex = pTextures[i];
				const TextureAttachDesc& desc = descs[i];
				GLuint index = desc.Index;

				assert(index < 8u);

				GLenum format_conponemt = TextureUtil::measureComponent(pTex->getFormat());
				switch (format_conponemt)
				{
				case GL_DEPTH_COMPONENT:
					assert(!depthHandled);
					handleTextureAttachment(pTex, GL_DEPTH_ATTACHMENT, desc, m_DepthAttach);
					depthHandled = true;
					break;
				case GL_DEPTH_STENCIL:
					assert(!depthStencilHandled);
					handleTextureAttachment(pTex, GL_DEPTH_STENCIL_ATTACHMENT, desc, m_DepthStencilAttach);
					depthStencilHandled = true;
					break;
				case GL_STENCIL:
					assert(!stencilHandled);
					handleTextureAttachment(pTex, GL_STENCIL_ATTACHMENT, desc, m_StencilAttach);
					stencilHandled = true;
					break;
				default:
					assert(!colorHandled[index]);
					handleTextureAttachment(pTex, GL_COLOR_ATTACHMENT0 + index, desc, m_ColorAttaches[index]);
					colorHandled[index] = true;
					return ;
				}
			}
			
			// unbind the previouse textures attchment.
			if (!depthHandled && m_DepthAttach.attached)
			{
				deAttachTexture(GL_DEPTH_ATTACHMENT, m_DepthAttach.type);
				m_DepthAttach.attached = false;
			}

			if (!depthStencilHandled && m_DepthStencilAttach.attached)
			{
				deAttachTexture(GL_DEPTH_STENCIL_ATTACHMENT, m_DepthStencilAttach.type);
				m_DepthStencilAttach.attached = false;
			}

			if (!stencilHandled && m_StencilAttach.attached)
			{
				deAttachTexture(GL_STENCIL_ATTACHMENT, m_StencilAttach.type);
				m_StencilAttach.attached = false;
			}

			for (int i = 0; i < 8; i++)
			{
				if (!colorHandled[i] && m_ColorAttaches[i].attached)
				{
					deAttachTexture(GL_COLOR_ATTACHMENT0 + i, m_ColorAttaches[i].type);
					m_ColorAttaches[i].attached = false;
				}
			}
		}
	}
}
