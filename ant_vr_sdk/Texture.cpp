#include "Texture.h"

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
	}
}
