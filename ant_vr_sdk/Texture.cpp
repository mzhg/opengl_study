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
