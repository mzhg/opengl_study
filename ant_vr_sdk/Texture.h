#ifndef _JET_UTIL_TEXTURE_H
#define _JET_UTIL_TEXTURE_H

#include <stdint.h>
#include <string>
#include <GL\glew.h>
#include "Disposeable.h"

namespace jet
{
	namespace util
	{
		class TextureGL : public Disposeable
		{
		public:
			TextureGL() : m_Texture(0){}
			~TextureGL();

		public:

			inline GLuint getTexture() const { return m_Texture; }
			virtual GLint getWidth() const = 0;
			virtual GLint getHeight() const  { return 1; }
			virtual GLint getDepth() const  { return 1; }

			inline GLenum getFormat() const  { return m_Format; }
			inline GLuint getMipLevels() const  { return m_MipLevels; }
			inline GLenum getTarget() const  { return m_Target; }
			virtual GLuint getSampleCount() const { return 1; }

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

		};

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

		enum class MemoryType
		{
			NONE,
			NEW,
			MALLOC,
		};

		typedef struct TextureData
		{
			const GLubyte* pData;
			int Format;
			int Type;
			int UnpackAlignment;
			MemoryType Owned;

			TextureData() : pData(NULL), UnpackAlignment(4), Owned(MemoryType::NONE){}
			TextureData(GLubyte* _pData, int _format, int _type, int unpackAlignment = 4, MemoryType owned = MemoryType::NONE) :pData(_pData), Format(_format), Type(_type), UnpackAlignment(unpackAlignment), Owned(owned) {}
			
			virtual ~TextureData()
			{
				if (Owned == MemoryType::NEW)
				{
					delete[] pData;
				}
				else if (Owned == MemoryType::MALLOC)
				{
					free((void*)pData);
				}
			}
		
		}TextureData;

		extern bool operator < (const Texture2DDesc& a, const Texture2DDesc& b);

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

	}
}
#endif