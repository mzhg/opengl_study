#pragma once

#include "Disposeable.h"
#include "Util.h"
#include <GL\glew.h>
#include "Texture.h"

namespace jet
{
	namespace util
	{
		enum class AttachType
		{
			TEXTURE_1D,
			TEXTURE_2D,
			TEXTURE_3D,
			TEXTURE_LAYER,
			TEXTURE
		};
#if 0
		enum class AttachLocation
		{
			DEPTH,
			DEPTH_STENCIL,
			STENCIL,
			COLOR0,
			COLOR1,
			COLOR2,
			COLOR3,
			COLOR4,
			COLOR5,
			COLOR6,
			COLOR7,
			COLOR8,
		};
#endif
		typedef struct TextureAttachDesc
		{
			GLuint Index;
			AttachType Type;
			GLuint Layer;
			GLuint Level;

			TextureAttachDesc() :
				Index(0),
				Type(AttachType::TEXTURE_2D),
				Layer(0),
				Level(0){}

			TextureAttachDesc(GLuint index, AttachType type, GLuint layer, GLuint level) :
				Index(index),
				Type(type),
				Layer(layer),
				Level(level){}
		}TextureAttachDesc;

		class FramebufferGL :public Disposeable
		{
		public:
			FramebufferGL(/*GLuint width, GLuint height*/):
				m_Framebuffer(0), m_Width(0), m_Height(0), m_AttachCount(0)
			{
				for (int i = 0; i < _countof(m_AttachedTextures); i++)
				{
					m_AttachedTextures[i] = nullptr;
					m_Owed[i] = false;
				}
			}
			virtual ~FramebufferGL()
			{
				dispose();
			}

			void addTextures(unsigned int count, const TextureGL** textures, const TextureAttachDesc* descs);
			const Texture2D* addTexture2D(const Texture2DDesc& texDesc, const TextureAttachDesc& attachDesc);
			void dispose() override;

			void enable();
			void disable();
			static FramebufferGL* getFramebufferObject(GLuint name);

		private:
			GLuint m_Framebuffer;
			GLuint m_Width, m_Height;
			GLuint m_AttachCount;
			TextureGL* m_AttachedTextures[8];
			bool m_Owed[8];
		};

		class RenderTargets : public Disposeable
		{
		public:
			RenderTargets():
				m_DepthAttach(),
				m_StencilAttach(),
				m_DepthStencilAttach(),
				m_ColorAttachCount(0)
			{
				for (int i = 0; i < _countof(m_ColorAttaches); i++)
				{
					m_ColorAttaches[i] = AttachInfo();
				}
			}

			void dispose() override;

			void enable();
			void disable();
			void setRenderTextures(unsigned int count, const TextureGL** textures, const TextureAttachDesc* descs);

		private:
			struct AttachInfo
			{
				GLuint textureId;
				GLenum textureTarget;
				bool attached;
				GLuint level;
				GLuint layer;
				AttachType type;

				AttachInfo() :
					textureTarget(0), attached(false),
					textureId(0), level(0), layer(0)
				{}
			};

			void handleTextureAttachment(const TextureGL* pTex, GLenum attachment, const TextureAttachDesc& desc, AttachInfo& info);

			AttachInfo m_DepthAttach;
			AttachInfo m_StencilAttach;
			AttachInfo m_DepthStencilAttach;
			AttachInfo m_ColorAttaches[8];
			GLuint     m_ColorAttachCount;

			GLuint m_Framebuffer;
		};
	}
}

