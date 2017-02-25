#include "Texture.h"
#include "Numeric.h"
#include <GLCapabilities.h>
#include "GLStates.h"

namespace jet
{
	namespace util
	{
		void TextureGL::dispose()
		{
			SAFE_RELEASE_TEXTURE(m_Texture)
		}

		TextureGL::~TextureGL()
		{
			dispose();
		}

		void TextureGL::applySamplerDesc(const SamplerDesc& desc, bool force)
		{
			static const GLCapabilities* pCap = GLCapabilities::get();

			if (m_Target == GL_TEXTURE_2D_MULTISAMPLE || m_Target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
			{
				return;
			}

			if (pCap->ARB_direct_state_access || pCap->EXT_direct_state_access)
			{
				// Min Filter
				if (force || desc.MinFilter != m_Samplers.MinFilter)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_MIN_FILTER, ConvertTextureFilterToGLenum(desc.MinFilter)));
					m_Samplers.MinFilter = desc.MinFilter;
				}

				// Mag Filter
				if (force || desc.MagFilter != m_Samplers.MagFilter)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_MAG_FILTER, ConvertTextureFilterToGLenum(desc.MagFilter)));
					m_Samplers.MagFilter = desc.MagFilter;
				}

				// WrapS
				if (force || desc.WrapS != m_Samplers.WrapS)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_S, ConvertTextureWrapToGLenum(desc.WrapS)));
					m_Samplers.WrapS = desc.WrapS;
				}

				// WrapT
				if (force || desc.WrapT != m_Samplers.WrapT)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_T, ConvertTextureWrapToGLenum(desc.WrapT)));
					m_Samplers.WrapT = desc.WrapT;
				}

				// WrapR
				if (force || desc.WrapR != m_Samplers.WrapR)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_R, ConvertTextureWrapToGLenum(desc.WrapR)));
					m_Samplers.WrapR = desc.WrapR;
				}

				// Border Color
				if (force || !Numeric::isEqual<GLfloat, 4>(desc.BorderColor, m_Samplers.BorderColor))
				{
					CHECK_GL(glTextureParameterfv(m_Texture, GL_TEXTURE_BORDER_COLOR, desc.BorderColor));
					for (int i = 0; i < 4; i++)
					{
						m_Samplers.BorderColor[i] = desc.BorderColor[i];
					}
				}

				// Anisotropic
				if (pCap->EXT_texture_filter_anisotropic && (force || desc.Anisotropic != m_Samplers.Anisotropic))
				{
					static GLint largest = -1;
					if (largest < 0)
					{
						glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest);
					}

					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_MAX_ANISOTROPY_EXT, Numeric::max(largest, desc.Anisotropic)));
					m_Samplers.Anisotropic = desc.Anisotropic;
				}

				if (force || desc.CompareToRef != m_Samplers.CompareToRef)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_COMPARE_MODE, desc.CompareToRef ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE));
					m_Samplers.CompareToRef = desc.CompareToRef;
				}

				if (force || desc.CompareMode != m_Samplers.CompareMode)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_COMPARE_FUNC, ConvertCompareFuncToGLenum(desc.CompareMode)));
					m_Samplers.CompareMode = desc.CompareMode;
				}
			}
			else
			{
				static GLStates& states = GLStates::get();
				const TextureGL* texture[] = { this };
				states.bindTextures(1, &texture[0], nullptr);

				// Min Filter
				if (force || desc.MinFilter != m_Samplers.MinFilter)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ConvertTextureFilterToGLenum(desc.MinFilter)));
					m_Samplers.MinFilter = desc.MinFilter;
				}

				// Mag Filter
				if (force || desc.MagFilter != m_Samplers.MagFilter)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ConvertTextureFilterToGLenum(desc.MagFilter)));
					m_Samplers.MagFilter = desc.MagFilter;
				}

				// WrapS
				if (force || desc.WrapS != m_Samplers.WrapS)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ConvertTextureWrapToGLenum(desc.WrapS)));
					m_Samplers.WrapS = desc.WrapS;
				}

				// WrapT
				if (force || desc.WrapT != m_Samplers.WrapT)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ConvertTextureWrapToGLenum(desc.WrapT)));
					m_Samplers.WrapT = desc.WrapT;
				}

				// WrapR
				if (force || desc.WrapR != m_Samplers.WrapR)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, ConvertTextureWrapToGLenum(desc.WrapR)));
					m_Samplers.WrapR = desc.WrapR;
				}

				// Border Color
				if (force || !Numeric::isEqual<GLfloat, 4>(desc.BorderColor, m_Samplers.BorderColor))
				{
					CHECK_GL(glTexParameterfv(m_Target, GL_TEXTURE_BORDER_COLOR, desc.BorderColor));
					for (int i = 0; i < 4; i++)
					{
						m_Samplers.BorderColor[i] = desc.BorderColor[i];
					}
				}

				// Anisotropic
				if (pCap->EXT_texture_filter_anisotropic && (force || desc.Anisotropic != m_Samplers.Anisotropic))
				{
					static GLint largest = -1;
					if (largest < 0)
					{
						CHECK_GL(glGetIntegerv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &largest));
					}

					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_MAX_ANISOTROPY_EXT, Numeric::max(largest, desc.Anisotropic)));
					m_Samplers.Anisotropic = desc.Anisotropic;
				}

				if (force || desc.CompareToRef != m_Samplers.CompareToRef)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_COMPARE_MODE, desc.CompareToRef ? GL_COMPARE_REF_TO_TEXTURE : GL_NONE));
					m_Samplers.CompareToRef = desc.CompareToRef;
				}

				if (force || desc.CompareMode != m_Samplers.CompareMode)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_COMPARE_FUNC, ConvertCompareFuncToGLenum(desc.CompareMode)));
					m_Samplers.CompareMode = desc.CompareMode;
				}
			}
		}

		void TextureGL::setFilter(TextureFilter minFilter, TextureFilter magFilter)
		{
			const bool force = false;
			static const GLCapabilities* pCap = GLCapabilities::get();

			if (m_Target == GL_TEXTURE_2D_MULTISAMPLE || m_Target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
			{
				return;
			}

			if (pCap->ARB_direct_state_access || pCap->EXT_direct_state_access)
			{
				// Min Filter
				if (force || minFilter != m_Samplers.MinFilter)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_MIN_FILTER, ConvertTextureFilterToGLenum(minFilter)));
					m_Samplers.MinFilter = minFilter;
				}

				// Mag Filter
				if (force || magFilter != m_Samplers.MagFilter)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_MAG_FILTER, ConvertTextureFilterToGLenum(magFilter)));
					m_Samplers.MagFilter = magFilter;
				}
			}
			else
			{
				static GLStates& states = GLStates::get();
				const TextureGL* texture[] = { this };
				states.bindTextures(1, texture);

				// Min Filter
				if (force || minFilter != m_Samplers.MinFilter)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_MIN_FILTER, ConvertTextureFilterToGLenum(minFilter)));
					m_Samplers.MinFilter = minFilter;
				}

				// Mag Filter
				if (force || magFilter != m_Samplers.MagFilter)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_MAG_FILTER, ConvertTextureFilterToGLenum(magFilter)));
					m_Samplers.MagFilter = magFilter;
				}
			}
		}
		void TextureGL::setWrap(TextureWrap wrapS, TextureWrap wrapT, TextureWrap wrapR)
		{
			const bool force = false;
			static const GLCapabilities* pCap = GLCapabilities::get();

			if (m_Target == GL_TEXTURE_2D_MULTISAMPLE || m_Target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
			{
				return;
			}

			if (pCap->ARB_direct_state_access || pCap->EXT_direct_state_access)
			{
				// WrapS
				if (force || wrapS != m_Samplers.WrapS)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_S, ConvertTextureWrapToGLenum(wrapS)));
					m_Samplers.WrapS = wrapS;
				}

				// WrapT
				if (force || wrapT != m_Samplers.WrapT)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_T, ConvertTextureWrapToGLenum(wrapT)));
					m_Samplers.WrapT = wrapT;
				}

				// WrapR
				if (force || wrapR != m_Samplers.WrapR)
				{
					CHECK_GL(glTextureParameteri(m_Texture, GL_TEXTURE_WRAP_R, ConvertTextureWrapToGLenum(wrapR)));
					m_Samplers.WrapR = wrapR;
				}
			}
			else
			{
				static GLStates& states = GLStates::get();
				const TextureGL* texture[] = { this };
				states.bindTextures(1, texture);

				// WrapS
				if (force || wrapS != m_Samplers.WrapS)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_WRAP_S, ConvertTextureWrapToGLenum(wrapS)));
					m_Samplers.WrapS = wrapS;
				}

				// WrapT
				if (force || wrapT != m_Samplers.WrapT)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_WRAP_T, ConvertTextureWrapToGLenum(wrapT)));
					m_Samplers.WrapT = wrapT;
				}

				// WrapR
				if (force || wrapR != m_Samplers.WrapR)
				{
					CHECK_GL(glTexParameteri(m_Target, GL_TEXTURE_WRAP_R, ConvertTextureWrapToGLenum(wrapR)));
					m_Samplers.WrapR = wrapR;
				}
			}
		}

		void TextureGL::setTexelSwizzle(ColorSwizzle red, ColorSwizzle green, ColorSwizzle blue, ColorSwizzle alpha)
		{
			static const GLCapabilities* pCap = GLCapabilities::get();

			if (m_Target == GL_TEXTURE_2D_MULTISAMPLE || m_Target == GL_TEXTURE_2D_MULTISAMPLE_ARRAY)
			{
				return;
			}

			bool needResetting = (red != m_SwizzleRed || green != m_SwizzleGreen || blue != m_SwizzleBlue || alpha != m_SwizzleAlpha);
			if (!needResetting)
			{
				return;
			}

			const GLint swizzleValues[4] = { ConvertColorSwizzleToGLenum(red), ConvertColorSwizzleToGLenum(green), 
									   ConvertColorSwizzleToGLenum(blue), ConvertColorSwizzleToGLenum(alpha) };
			if (pCap->ARB_direct_state_access || pCap->EXT_direct_state_access)
			{
				CHECK_GL(glTextureParameteriv(m_Texture, GL_TEXTURE_SWIZZLE_RGBA, swizzleValues));
			}
			else
			{
				static GLStates& states = GLStates::get();
				const TextureGL* texture[] = { this };
				states.bindTextures(1, texture);
				CHECK_GL(glTexParameteriv(m_Target, GL_TEXTURE_SWIZZLE_RGBA, swizzleValues));
			}

			m_SwizzleRed = red;
			m_SwizzleAlpha = alpha;
			m_SwizzleBlue = blue;
			m_SwizzleGreen = green;
		}

		void TextureGL::restore()
		{
			// TODO
		}

		bool operator < (const Texture2DDesc& a, const Texture2DDesc& b)
		{
#define COMPARE(name)  \
	if(a.name < b.name) \
		return true;    \
																																else if (a.name > b.name) \
		return false;

			COMPARE(Width);
			COMPARE(Height);
			COMPARE(MipLevels);
			COMPARE(ArraySize);
			COMPARE(Format);
			COMPARE(SampleCount);

#undef COMPARE
			return false;
		}
	}
}
