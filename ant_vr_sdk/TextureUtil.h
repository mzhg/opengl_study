#ifndef _JET_UTIL_TEXTURE_UTIL_H
#define _JET_UTIL_TEXTURE_UTIL_H

#include "Texture.h"

namespace jet
{
	namespace util
	{

		class TextureUtil
		{
		public:
			TextureUtil();
			~TextureUtil();

			static void createTexture2D(const Texture2DDesc* pDesc, const TextureData* pInitData, Texture2D* pOut);
			static bool createTexture2DFromFile(const char* filename, Texture2D* pOut, bool flip = true);
			static bool createTexture2D(GLint target, GLuint textureID, Texture2D* pOut);

			static void createTexture1D(const Texture1DDesc* pDesc, const TextureData* pInitData, Texture1D* pOut);
			static void createTexture3D(const Texture3DDesc* pDesc, const TextureData* pInitData, Texture3D* pOut);
			static void createTextureCube(const TextureCubeDesc* pDesc, const TextureData* pInitData[6], TextureCube* pOut);

			static bool loadTextureDataFromFile(const char* filename, TextureData* pOut,int* width, int* height);
			static GLenum measureComponent(GLenum internalFormat);
			static int measureSizePerPixel(GLenum internalFormat);
			static int measureCompCountPerPixel(GLenum format);
			static const char* getFormatName(GLenum internalFormat);
		};
	}
}

#endif
