
#ifndef _JET_UTIL_DISPOSE_H
#define _JET_UTIL_DISPOSE_H

namespace jet
{
	namespace util
	{

#define SAFE_RELEASE_TEXTURE(x) if(x != 0 && glIsTexture(x)) {glDeleteTextures(1, &x); x = 0; }
#define SAFE_RELEASE_FRAMEBUFFER(x) if(x != 0 && glIsFramebuffer(x)) {glDeleteFramebuffers(1, &x); x = 0; }
#define SAFE_RELEASE_BUFFER(x) if(x != 0 && glIsBuffer(x)) {glDeleteBuffers(1, &x); x = 0; }
#define SAFE_RELEASE_VERTEX_ARRAY(x)  if(x != 0 && glIsVertexArray(x)) {glDeleteVertexArrays(1, &x); x = 0; }

#define SAFE_DISPOSE(x)  if(x != 0) {x->dispose(); delete x; x = 0;}
#define SAFE_DELETE(x)  if(x != 0) {delete x; x = 0;}

		class Disposeable
		{
		public:

			virtual void dispose() = 0;
		};
	}
}

#endif