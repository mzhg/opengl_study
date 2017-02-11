#ifndef _JET_UTIL_TEXTURE_H
#define _JET_UTIL_TEXTURE_H

#include <stdint.h>
#include "Disposeable.h"
#include "GLUtil.h"
#include "gl_state_define.h"

namespace jet
{
	namespace util
	{
		static const GLenum CUBE_FACES[] =
		{
			GL_TEXTURE_CUBE_MAP_POSITIVE_X, GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Y, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
			GL_TEXTURE_CUBE_MAP_POSITIVE_Z, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
		};

		class TextureGL : public Disposeable
		{
		public:
			TextureGL() : m_Texture(0), m_Samplers(),
				m_SwizzleRed(ColorSwizzle::RED),
				m_SwizzleGreen(ColorSwizzle::GREEN),
				m_SwizzleBlue(ColorSwizzle::BLUE),
				m_SwizzleAlpha(ColorSwizzle::ALPHA)
			{}
			~TextureGL();

		public:

			inline GLuint getTexture() const { return m_Texture; }
			virtual GLint getWidth() const = 0;
			virtual GLint getHeight() const  { return 1; }
			virtual GLint getDepth() const  { return 1; }

			inline GLenum getFormat() const  { return m_Format; }
			inline GLuint getMipLevels() const  { return m_MipLevels; }
			inline GLenum getTarget() const  { return m_Target; }

			void applySamplerDesc(const SamplerDesc& desc, bool force = false);
			void setFilter(TextureFilter minFilter = TextureFilter::LINEAR, TextureFilter magFilter = TextureFilter::LINEAR);
			void setWrap(TextureWrap wrapS = TextureWrap::CLAMP, TextureWrap wrapT = TextureWrap::CLAMP, TextureWrap wrapR = TextureWrap::CLAMP);
			void setTexelSwizzle(ColorSwizzle red, ColorSwizzle green, ColorSwizzle blue, ColorSwizzle alpha);
			void restore();

			SamplerDesc getSamplerDesc() const { return m_Samplers; }
			void getSampleDesc(SamplerDesc& out) const{ out = m_Samplers; }

			void dispose();

			friend class TextureUtil;
		private:

			// The OpenGL texture object
			GLuint m_Texture;
			// The texture target. GL_Texture2D
			GLenum m_Target;

			// The texture internal format. e.g: GL_RGBA16F
			GLenum m_Format;

			// The count of the mipmaps. At least is 1.
			GLuint m_MipLevels;

			SamplerDesc m_Samplers;

			ColorSwizzle m_SwizzleRed;
			ColorSwizzle m_SwizzleGreen;
			ColorSwizzle m_SwizzleBlue;
			ColorSwizzle m_SwizzleAlpha;
		};

		typedef struct Texture1DDesc
		{
			GLint Width;
			GLuint MipLevels;
			GLuint ArraySize;
			GLuint Format;

			Texture1DDesc() : Width(0), MipLevels(1), ArraySize(1), Format(0){}
			Texture1DDesc(GLint width, GLuint format) :
				Width(width), Format(format), MipLevels(1), ArraySize(1){}
			Texture1DDesc(GLint width, GLuint format, GLuint mipLevels, GLuint arraySize) :
				Width(width), Format(format), MipLevels(mipLevels), ArraySize(arraySize){}
		}Texture1DDesc;

		typedef struct Texture2DDesc
		{
			GLint Width;
			GLint Height;
			GLuint MipLevels;
			GLuint ArraySize;
			GLuint Format;
			GLuint SampleCount;

			Texture2DDesc() : Width(0), Height(0), MipLevels(1), ArraySize(1), Format(0), SampleCount(1){}
			Texture2DDesc(GLint width, GLint height, GLuint format) :
				Width(width), Height(height), Format(format), MipLevels(1), ArraySize(1), SampleCount(1){}
			Texture2DDesc(GLint width, GLint height, GLuint format, GLuint mipLevels, GLuint arraySize, GLuint sampleCount):
				Width(width), Height(height), Format(format), MipLevels(mipLevels), ArraySize(arraySize), SampleCount(sampleCount){}
		}Texture2DDesc;

		typedef struct Texture3DDesc
		{
			GLint Width;
			GLint Height;
			GLint Depth;
			GLuint MipLevels;
			GLuint Format;

			Texture3DDesc() : Width(0), Height(0), Depth(0), MipLevels(1), Format(0){}
			Texture3DDesc(GLint width, GLint height, GLint depth, GLuint format) :
				Width(width), Height(height), Depth(depth), MipLevels(1), Format(format){}
			Texture3DDesc(GLint width, GLint height, GLint depth, GLuint format, GLuint mipLevels) :
				Width(width), Height(height), Depth(depth), MipLevels(1), Format(format){}
		}Texture3DDesc;

		typedef struct TextureCubeDesc
		{
			GLint Width;
			GLuint Format;
			GLuint MipLevels;
			GLuint ArraySize;

			TextureCubeDesc() : Width(0), MipLevels(1), ArraySize(1), Format(0){}
			TextureCubeDesc(GLint width, GLuint format) :
				Width(width), Format(format), MipLevels(1), ArraySize(1){}
			TextureCubeDesc(GLint width, GLuint format, GLuint mipLevels, GLuint arraySize) :
				Width(width), Format(format), MipLevels(mipLevels), ArraySize(arraySize){}
		}TextureCubeDesc;

		enum class MemoryType
		{
			NONE,
			NEW,
			MALLOC,
		};

		typedef struct TextureData
		{
			GLuint Mipmaps;
			GLubyte** pData;
			GLuint* pImageSize;
			int Format;
			int Type;
			int UnpackAlignment;
			MemoryType Owned;

			TextureData() : Mipmaps(0), pData(nullptr), UnpackAlignment(4), Owned(MemoryType::NONE), pImageSize(nullptr){}
			TextureData(GLuint mipmaps, const GLubyte** _pData, int _format, int _type, int unpackAlignment = 4, MemoryType owned = MemoryType::NONE) :
				Mipmaps(mipmaps), Format(_format), Type(_type), 
				UnpackAlignment(unpackAlignment), Owned(owned),
				pImageSize(nullptr)
			{
				pData = new GLubyte*[mipmaps];
				for (GLuint i = 0; i < mipmaps; i++)
				{
					pData[i] = (GLubyte*)_pData[i];
				}
			}
			
			virtual ~TextureData()
			{
				if (Owned == MemoryType::NEW)
				{
					for (GLuint i = 0; i < Mipmaps; i++)
						delete[] pData[i];
				}
				else if (Owned == MemoryType::MALLOC)
				{
					for (GLuint i = 0; i < Mipmaps; i++)
						free((void*)pData[i]);
				}

				if (pData)
				{
					delete[] pData;
				}

				if (pImageSize)
				{
					delete[] pImageSize;
				}
			}

		private:
			TextureData(TextureData&) = delete;
			TextureData operator=(TextureData&) = delete;
		}TextureData;

		extern bool operator < (const Texture2DDesc& a, const Texture2DDesc& b);

		// TEXTURE_1D, TEXTURE_1D_ARRAY
		class Texture1D : public TextureGL
		{
			
		public:
			Texture1D() :TextureGL() {}

			GLuint getArraySize() const  { return m_ArraySize; }
			GLint  getWidth() const { return m_Width; }

			Texture1DDesc getDesc() const
			{
				Texture1DDesc desc;
				desc.ArraySize = getArraySize();
				desc.Format = getFormat();
				desc.MipLevels = getMipLevels();
				desc.Width = getWidth();

				return desc;
			}

			friend class TextureUtil;
		private:
			GLint m_Width;
			GLuint m_ArraySize;
		};

		// GL_TEXTURE_CUBE_MAP, GL_TEXTURE_CUBE_ARRAY_MAP
		class TextureCube : public TextureGL
		{

		public:
			TextureCube() :TextureGL() {}

			GLuint getArraySize() const  { return m_ArraySize; }
			GLint  getWidth() const { return m_Width; }

			TextureCubeDesc getDesc() const
			{
				TextureCubeDesc desc;
				desc.ArraySize = getArraySize();
				desc.Format = getFormat();
				desc.MipLevels = getMipLevels();
				desc.Width = getWidth();

				return desc;
			}

			friend class TextureUtil;
		private:
			GLint m_Width;
			GLuint m_ArraySize;
		};

		// GL_TEXTURE_2D, GL_TEXTURE_2D_ARRAY, GL_TEXTURE_2D_MULTISAMPLE and GL_TEXTURE_2D_MULTISAMPLE_ARRAY.
		class Texture2D : public TextureGL
		{

		public:
			Texture2D() :TextureGL() {}

			GLuint getArraySize() const  { return m_ArraySize; }
			GLint  getWidth() const { return m_Width; }
			GLint  getHeight() const  { return m_Height; }
			GLuint getSampleCount() const { return m_Samples; }

			Texture2DDesc getDesc() const
			{
				Texture2DDesc desc;
				desc.ArraySize = getArraySize();
				desc.Format = getFormat();
				desc.Height = getHeight();
				desc.MipLevels = getMipLevels();
				desc.SampleCount = getSampleCount();
				desc.Width = getWidth();

				return desc;
			}

			friend class TextureUtil;
		private:
			GLint m_Width;
			GLint m_Height;
			GLuint m_ArraySize;
			GLuint m_Samples;
		};

		// GL_TEXTURE_3D.
		class Texture3D : public TextureGL
		{

		public:
			Texture3D() :TextureGL() {}

			GLint  getWidth() const { return m_Width; }
			GLint  getHeight() const  { return m_Height; }
			GLint getDepth() const  { return m_Depth; }

			Texture3DDesc getDesc() const
			{
				Texture3DDesc desc;
				desc.Format = getFormat();
				desc.Height = getHeight();
				desc.MipLevels = getMipLevels();
				desc.Width = getWidth();
				desc.Depth = getDepth();

				return desc;
			}

			friend class TextureUtil;
		private:
			GLint m_Width;
			GLint m_Height;
			GLint m_Depth;
		};

	}
}
#endif