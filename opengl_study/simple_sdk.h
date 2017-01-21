
#ifndef _SIMPLE_SDK_
#define _SIMPLE_SDK_
/*
typedef	enum TexelFormat
{
		RGB,
		RGBA,
		BGR,
		BGRA
}TexelFormat;
*/

extern "C" void ogl_init();
extern "C" void ogl_resize(int width, int height);
extern "C" void ogl_set_projection(float fov, float near, float far);
extern "C" void ogl_set_background_texture(int width, int height, int format, const char* pData);
extern "C" void ogl_set_rect_texture(int width, int height, int format, const char* pData);
extern "C" void ogl_read_texels_from_renderbuffer();

extern "C" void ogl_set_background_texture_id(unsigned int id);
extern "C" void ogl_set_rect_texture_id(unsigned int id);

extern "C" void ogl_set_rect_size(int x, int y, int width, int height);
extern "C" void ogl_set_background_rotation(float yaw, float pitch, float roll);
extern "C" void ogl_set_background_rotation_mat(float mat[16]);
extern "C" void ogl_render(float elpsedTime);
extern "C" void ogl_destroy();

#endif
