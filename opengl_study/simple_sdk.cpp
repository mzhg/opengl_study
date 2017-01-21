#include "simple_sdk.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <string.h>
#if 1
#include <GL\glew.h>
#else
#include <GLES2/gl2.h>
#endif

#define OGL_DEBUG 1
#if OGL_DEBUG
#define AND_LOG(...)  printf(__VA_ARGS__)
#define INIT_ONCE 1
#define GL(x) x; check_gl_error()

#define OBJ_CHECK(name, func)  if(name == 0 || !func(name)) {AND_LOG(#name" is not valid!\n");}

#else
#define AND_LOG(...)
#define INIT_ONCE 
#define GL(x) x
#endif

#ifdef __cplusplus
extern "C"{
#endif

	static const char* g_pVertStr =
		"uniform mat4 g_Mat;\n"
		"attribute vec4  g_Position;\n"
		"attribute vec2  g_TextureCoord;\n"
		"varying   vec2  vTextureCoord;\n"

		"void main(){\n"
		"vTextureCoord = g_TextureCoord;\n"
		"gl_Position = g_Mat*g_Position;\n"
		"}\n";

	static const char* g_pFragStr =
		"#if GL_ES\n"
		"precision mediump float;\n"
		"#endif\n"

		"varying vec2  vTextureCoord;\n"
		"uniform sampler2D sparrow;\n"

		"void main(){\n"
		"	gl_FragColor = texture2D(sparrow, vTextureCoord);\n"
		"}\n";

	static void check_gl_error()
	{
		char* errorMsg;
		GLenum error = glGetError();
		switch (error)
		{
		case GL_NO_ERROR:
			return;
			break;
		case GL_INVALID_ENUM:
			errorMsg = "GL_INVALID_ENUM";
			break;
		case GL_INVALID_VALUE:
			errorMsg = "GL_INVALID_VALUE";
			break;
		case GL_INVALID_OPERATION:
			errorMsg = "GL_INVALID_OPERATION";
			break;
		default:
			errorMsg = "Other";
			break;
		}

		AND_LOG("Error msg: %s\n", errorMsg);
	}

#define POSITION 0
#define TEXCOORD 1

	typedef struct Rect
	{
		int x, y;
		int width, height;
	}Rect;

	typedef struct Size
	{
		int width, height;
	}Size;

	typedef struct Projection
	{
		float fov;
		float near;
		float far;
	}Projection;

	typedef struct TextureGL
	{
		int width;
		int height;
		unsigned int textureId;
		int owend;
	}TextureGL;

	static TextureGL empty_texture()
	{
		TextureGL tex;
		tex.width = 0;
		tex.height = 0;
		tex.textureId = 0;
		tex.owend = false;

		return tex;
	}

	static void release_texture(TextureGL& tex)
	{
		if (tex.owend)
		{
			GL(glDeleteTextures(1, &tex.textureId));
			tex.textureId = 0;
		}
	}

	static const float PI = 3.1415926f;

	static GLuint g_program = 0;
	static TextureGL g_background_tex_id;
	static TextureGL g_rect_texture_id;

	static GLuint g_sphere_vbo = 0;  // sphere vertex buffer
	static GLuint g_sphere_ibo = 0;  // sphere indices buffer. 
	static GLuint g_sphere_indices_count = 0;
	static GLuint g_rect_vbo = 0;
	static GLuint g_rect_ibo = 0;

	static GLint g_mat_loc = -1;

	static Rect g_rect_bound;
	static Size g_window_size;
	static Projection g_projection;

	typedef struct Vertex2
	{
		float x, y;
	}Vertex2;

	typedef struct Vertex3
	{
		float x, y, z;
	}Vertex3;

	static Vertex3 g_background_rotate;

	typedef struct Vertex
	{
		Vertex3 position;
		Vertex2 texcoord;
	}Vertex;

	typedef struct Matrix4
	{
		union 
		{
			struct 
			{
				float m00, m01, m02, m03, m10, m11, m12, m13, m20, m21, m22, m23, m30, m31, m32, m33;
			};

			float m[16];
		};
	}Matrix4;

	static Matrix4 g_background_rotate2;

	static void create_sphere_vertexs(int, int);
	static void create_rect_vertices();
	static void create_program();
	static Vertex3 setVertex(float, float, float);

	static Matrix4 identity()
	{
		Matrix4 mat;
		int idx;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				idx = j * 4 + i;
				if (i == j)
				{
					mat.m[idx] = 1.0f;
				}
				else
				{
					mat.m[idx] = 0.0f;
				}
			}
		}
		return mat;
	}

	void ogl_init()
	{
		create_sphere_vertexs(50,20);
		create_rect_vertices();
		create_program();

		g_background_rotate = setVertex(0, 0, 0);
		g_projection.far = 100.0f;
		g_projection.near = 0.1f;
		g_projection.fov = 60.0f;

		g_background_tex_id = empty_texture();
		g_rect_texture_id = empty_texture();

		g_background_rotate2 = identity();
	}

	void ogl_resize(int width, int height)
	{
		g_window_size.width = width;
		g_window_size.height = height;
	}

	void ogl_set_projection(float fov, float near, float far)
	{
		g_projection.far = far;
		g_projection.near = near;
		g_projection.fov = fov;
	}

	static Matrix4 perspective(float fov, float aspect, float zNear, float zFar) {
		float r = PI / 360.0f * fov;
//		float r = (float) Math.toRadians(fov / 2);
		float deltaZ = zFar - zNear;
		float s = sinf(r);
		float cotangent = 0;

		Matrix4 out = identity();

		// cos(r) / sin(r) = cot(r)
		cotangent = cosf(r) / s;
		out.m00 = cotangent / aspect;
		out.m11 = cotangent;
		out.m22 = -(zFar + zNear) / deltaZ;
		out.m23 = -1;
		out.m32 = -2 * zNear * zFar / deltaZ;
		out.m33 = 0;

		return out;
	}

	static Matrix4 ortho(float left, float right, float bottom, float top, float near, float far) {
		float tx = -((right + left) / (right - left));
		float ty = -((top + bottom) / (top - bottom));
		float tz = -((far + near) / (far - near));

		Matrix4 out = identity();

		out.m00 = 2 / (right - left);
		out.m11 = 2 / (top - bottom);
		out.m22 = -2 / (far - near);
		out.m30 = tx;
		out.m31 = ty;
		out.m32 = tz;

		return out;
	}

	/**
	* Rotation matrix creation. From euler angles:<ol>
	* <li> Yaw around Y axis in radians
	* <li> Pitch around X axis in radians
	* <li> Roll around Z axis in radians
	* </ol>
	* return the rotation matrix [R] = [Roll].[Pitch].[Yaw]
	*/
	static Matrix4 rotationYawPitchRoll(float yaw, float pitch, float roll){
		Matrix4 result;

		float cx = cos(pitch);
		float sx = sin(pitch);
		float cy = cos(yaw);
		float sy = sin(yaw);
		float cz = cos(roll);
		float sz = sin(roll);

		float cxsy = cx * sy;
		float sxsy = sx * sy;

		result.m00 = (cy * cz);
		result.m10 = (-cy * sz);
		result.m20 = -sy;
		result.m01 = (-sxsy * cz + cx * sz);
		result.m11 = (sxsy * sz + cx * cz);
		result.m21 = (-sx * cy);
		result.m02 = (cxsy * cz + sx * sz);
		result.m12 = (-cxsy * sz + sx * cz);
		result.m22 = (cx * cy);
		result.m30 = 0.0f;
		result.m31 = 0.0f;
		result.m32 = 0.0f;
		result.m33 = 1.0f;
		result.m03 = 0.0f;
		result.m13 = 0.0f;
		result.m23 = 0.0f;

		return result;
	}

	/**
	* Multiply the right matrix by the left and place the result in a third matrix.
	* @param left The left source matrix
	* @param right The right source matrix
	* @return the destination matrix
	*/
	static Matrix4 mulMat(const Matrix4& left, const Matrix4& right) {
		Matrix4 dest;

		dest.m00 = left.m00 * right.m00 + left.m10 * right.m01 + left.m20 * right.m02 + left.m30 * right.m03;
		dest.m01 = left.m01 * right.m00 + left.m11 * right.m01 + left.m21 * right.m02 + left.m31 * right.m03;
		dest.m02 = left.m02 * right.m00 + left.m12 * right.m01 + left.m22 * right.m02 + left.m32 * right.m03;
		dest.m03 = left.m03 * right.m00 + left.m13 * right.m01 + left.m23 * right.m02 + left.m33 * right.m03;
		dest.m10 = left.m00 * right.m10 + left.m10 * right.m11 + left.m20 * right.m12 + left.m30 * right.m13;
		dest.m11 = left.m01 * right.m10 + left.m11 * right.m11 + left.m21 * right.m12 + left.m31 * right.m13;
		dest.m12 = left.m02 * right.m10 + left.m12 * right.m11 + left.m22 * right.m12 + left.m32 * right.m13;
		dest.m13 = left.m03 * right.m10 + left.m13 * right.m11 + left.m23 * right.m12 + left.m33 * right.m13;
		dest.m20 = left.m00 * right.m20 + left.m10 * right.m21 + left.m20 * right.m22 + left.m30 * right.m23;
		dest.m21 = left.m01 * right.m20 + left.m11 * right.m21 + left.m21 * right.m22 + left.m31 * right.m23;
		dest.m22 = left.m02 * right.m20 + left.m12 * right.m21 + left.m22 * right.m22 + left.m32 * right.m23;
		dest.m23 = left.m03 * right.m20 + left.m13 * right.m21 + left.m23 * right.m22 + left.m33 * right.m23;
		dest.m30 = left.m00 * right.m30 + left.m10 * right.m31 + left.m20 * right.m32 + left.m30 * right.m33;
		dest.m31 = left.m01 * right.m30 + left.m11 * right.m31 + left.m21 * right.m32 + left.m31 * right.m33;
		dest.m32 = left.m02 * right.m30 + left.m12 * right.m31 + left.m22 * right.m32 + left.m32 * right.m33;
		dest.m33 = left.m03 * right.m30 + left.m13 * right.m31 + left.m23 * right.m32 + left.m33 * right.m33;

		return dest;
	}

	static void measure_internal_format(GLenum* internalFormat, GLenum* pixelFormat, int format)
	{
		switch (format)
		{
		case GL_RGB:
			*internalFormat = GL_RGB;
			*pixelFormat = GL_RGB;
			break;
		case GL_RGBA:
			*internalFormat = GL_RGBA;
			*pixelFormat = GL_RGBA;
			break;
#if 0
		case GL_BGR:
			*internalFormat = GL_RGB;
			*pixelFormat = GL_BGR;
			break;
		case GL_BGRA:
			*internalFormat = GL_RGBA;
			*pixelFormat = GL_BGRA;
			break;
#endif
		default:
			assert(0);
			break;
		}
	}

	static void create_texture_internal(TextureGL& tex, int width, int height, int format, const char* pData)
	{
		int allocated = 0;
		int tex_width = 0;
		int tex_height = 0;

		GLenum internalFormat;
		GLenum pixelFormat;

		unsigned int& textureID = tex.textureId;
		if (textureID == 0 || !glIsTexture(textureID) || !tex.owend)
		{
			GL(glGenTextures(1, &textureID));
			GL(glBindTexture(GL_TEXTURE_2D, textureID));
			tex.owend = 1;
		}
		else
		{
			GL(glBindTexture(GL_TEXTURE_2D, textureID));
			allocated = 1;
//			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &tex_width);
//			glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &tex_height);

			tex_width = tex.width;
			tex_height = tex.height;
		}
		
		measure_internal_format(&internalFormat, &pixelFormat, format);
		if (!allocated || tex_width != width || tex_height != height)
		{
			GL(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, pixelFormat, GL_UNSIGNED_BYTE, pData));
			GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
			GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
			GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
			GL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

			tex.width = width;
			tex.height = height;
		}
		else
		{
			GL(glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, pixelFormat, GL_UNSIGNED_BYTE, pData));
		}

		GL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void ogl_read_texels_from_renderbuffer()
	{
		if (g_rect_texture_id.textureId == 0 || g_rect_texture_id.width != g_window_size.width || g_rect_texture_id.height != g_window_size.height)
		{
			release_texture(g_rect_texture_id);
			create_texture_internal(g_rect_texture_id, g_window_size.width, g_window_size.height, GL_RGBA, (const char*)0);
			AND_LOG("Create rect texture from renderbuffer.");
		}

		GL(glBindTexture(GL_TEXTURE_2D, g_rect_texture_id.textureId));
		GL(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, g_window_size.width, g_window_size.height));
		GL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void ogl_set_background_texture(int width, int height, int format, const char* pData)
	{
		create_texture_internal(g_background_tex_id, width, height, format, pData);
		AND_LOG("Create background texture from pixel data.");
	}

	void ogl_set_background_rotation(float yaw, float pitch, float roll)
	{
		g_background_rotate = setVertex(pitch, yaw, roll);
	}

	void ogl_set_background_rotation_mat(float mat[16])
	{
		memcpy(&g_background_rotate2, mat, sizeof(Matrix4));
	}

	void ogl_set_rect_texture(int width, int height, int format, const char* pData)
	{
		create_texture_internal(g_rect_texture_id, width, height, format, pData);
		AND_LOG("Create rect texture from pixel data.");
	}

	void ogl_set_background_texture_id(unsigned int id)
	{
		release_texture(g_background_tex_id);
		g_background_tex_id.textureId = id;
		g_background_tex_id.owend = 0;

		AND_LOG("Create background texture with the given texture id.");
	}

	void ogl_set_rect_texture_id(unsigned int id)
	{
		release_texture(g_rect_texture_id);
		g_rect_texture_id.textureId = id;
		g_rect_texture_id.owend = 0;

		AND_LOG("Create rect texture with the given texture id.");
	}

	void ogl_set_rect_size(int x, int y, int width, int height)
	{
		g_rect_bound.x = x;
		g_rect_bound.y = y;
		g_rect_bound.width = width;
		g_rect_bound.height = height;

		AND_LOG("set rect bound[x = %d, y = %d, width = %d, height = %d].\n", x,y,width, height);
	}

	void ogl_render(float elpsedTime)
	{
		GL(glEnable(GL_DEPTH_TEST));
		GL(glDepthFunc(GL_ALWAYS));
		GL((GL_FALSE));
		GL(glDisable(GL_CULL_FACE));
		GL((GL_BLEND));
		GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
		
#if OGL_DEBUG
		int viewports[4];
		glGetIntegerv(GL_VIEWPORT, viewports);

		GLint framebuffer;
		glGetIntegerv(0x8CA6, &framebuffer);

		AND_LOG("The current Viewport = [%d, %d, %d, %d].\n", viewports[0], viewports[1], viewports[2], viewports[3]);
		AND_LOG("The current framebuffer name = %d.\n", framebuffer);

		OBJ_CHECK(g_program, glIsProgram);
		OBJ_CHECK(g_background_tex_id.textureId, glIsTexture);
		OBJ_CHECK(g_rect_texture_id.textureId, glIsTexture);
		OBJ_CHECK(g_sphere_vbo, glIsBuffer);
		OBJ_CHECK(g_sphere_ibo, glIsBuffer);
		OBJ_CHECK(g_rect_vbo,   glIsBuffer);

#endif

		GL(glViewport(0, 0, g_window_size.width, g_window_size.height));
		GL(glUseProgram(g_program));

		{
			// render the sphere
			Matrix4 proj = perspective(g_projection.fov, (float)g_window_size.width / (float)g_window_size.height, g_projection.near, g_projection.far);
			Matrix4 rotation = rotationYawPitchRoll(g_background_rotate.y, g_background_rotate.x, g_background_rotate.z);
			rotation = mulMat(rotation, g_background_rotate2);
			Matrix4 mvp = mulMat(proj, rotation);

			GL(glUniformMatrix4fv(g_mat_loc, 1, GL_FALSE, (const GLfloat*)&mvp));

			GL(glActiveTexture(GL_TEXTURE0));
			GL(glBindTexture(GL_TEXTURE_2D, g_background_tex_id.textureId));

			GL(glBindBuffer(GL_ARRAY_BUFFER, g_sphere_vbo));
			GL(glEnableVertexAttribArray(0));
			GL(glEnableVertexAttribArray(1));

			GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
			GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)sizeof(Vertex3)));

			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_sphere_ibo));
			GL(glDrawElements(GL_TRIANGLES, g_sphere_indices_count, GL_UNSIGNED_SHORT, 0));
		}

		{
			// render the rect
//			glEnable(GL_BLEND);
//			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			Matrix4 orthMat = ortho(0, +g_window_size.width, 0, +g_window_size.height,
				-1.0f, 1.0f);
#if 0
			Matrix4 viewMat0 = identity();
			viewMat0.m00 = 0.5f;
			viewMat0.m11 = 0.5f;
			viewMat0.m30 = 0.5f;
			viewMat0.m31 = 0.5f;

			Matrix4 viewMat1 = identity();
			viewMat1.m00 = g_rect_bound.width;
			viewMat1.m11 = g_rect_bound.height;
			viewMat1.m30 = g_rect_bound.x;
			viewMat1.m31 = g_rect_bound.y;

			Matrix4 viewMat = mulMat(viewMat1, viewMat0);
#else
			Matrix4 viewMat = identity();
			viewMat.m00 = g_rect_bound.width * 0.5f;
			viewMat.m11 = g_rect_bound.height * 0.5f;
			viewMat.m30 = g_rect_bound.x + viewMat.m00;
			viewMat.m31 = g_rect_bound.y + viewMat.m11;
#endif

			Matrix4 mvp = mulMat(orthMat, viewMat);
			GL(glUniformMatrix4fv(g_mat_loc, 1, GL_FALSE, (const GLfloat*)&mvp));

			GL(glActiveTexture(GL_TEXTURE0));
			GL(glBindTexture(GL_TEXTURE_2D, g_rect_texture_id.textureId));
			GL(glBindBuffer(GL_ARRAY_BUFFER, g_rect_vbo));
			GL(glEnableVertexAttribArray(0));
			GL(glEnableVertexAttribArray(1));

			GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
			GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)sizeof(Vertex3)));

			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
			GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

			GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
			GL(glDisableVertexAttribArray(0));
			GL(glDisableVertexAttribArray(1));
			GL(glDisable(GL_BLEND));
		}

		GL(glDisable(GL_DEPTH_TEST));
		GL(glDepthFunc(GL_LESS));
		GL(glDepthMask(GL_TRUE));
	}

	void ogl_destroy()
	{
		if (g_program)
		{
			glDeleteProgram(g_program);
			g_program = 0;
		}

		release_texture(g_background_tex_id);
		release_texture(g_rect_texture_id);

#define DELETE_BUF(x) if(x) {GL(glDeleteBuffers(1, &x)); x = 0;}
		DELETE_BUF(g_sphere_ibo);
		DELETE_BUF(g_sphere_vbo);
		DELETE_BUF(g_rect_vbo);
		DELETE_BUF(g_rect_ibo)
#undef DELETE_BUF
	}

	static void create_sphere_vertexs(int mSegmentsW, int mSegmentsH)
	{
#if INIT_ONCE
		if (g_rect_vbo != 0 && glIsBuffer(g_sphere_vbo) && glIsBuffer(g_sphere_ibo))
			return;
#endif
		const int numVertices = (mSegmentsW+1)*(mSegmentsH+1);
        const int numIndices = 2*mSegmentsW*(mSegmentsH-1)*3;
        int i, j;
		float mRadius = 1.0f;
		Vertex* pVertices = (Vertex*)malloc(numVertices * sizeof(Vertex));
		unsigned short* pIndices = (unsigned short*)malloc(numIndices * sizeof(unsigned short));
		unsigned int vtx_idx = 0;
		unsigned int idi_idx = 0;

		for (j = 0; j <= mSegmentsH; ++j)
		{
			float phi = (float)(PI*j / mSegmentsH);
			float y = mRadius*cosf(phi);

			float ringRadius = mRadius*sinf(phi);

			for (i = 0; i <= mSegmentsW; ++i)
			{
				float theta = (float)(2.0f*PI*i / mSegmentsW);
				float x = ringRadius * cosf(theta);
				float z = ringRadius * sinf(theta);

				pVertices[vtx_idx].position.x = x;
				pVertices[vtx_idx].position.y = y;
				pVertices[vtx_idx].position.z = z;

				//TextureCoords
				float u = (float)i / mSegmentsW;
				float v = (float)j / mSegmentsH;

				pVertices[vtx_idx].texcoord.x = u;
				pVertices[vtx_idx].texcoord.y = v;

				//Indexes
				if (i > 0 && j > 0)
				{
					int a = (mSegmentsW + 1)*j + i; // right-down
					int b = (mSegmentsW + 1)*j + i - 1; // left-down
					int c = (mSegmentsW + 1)*(j - 1) + i - 1; // left-upper
					int d = (mSegmentsW + 1)*(j - 1) + i; // right-upper


					if (j == mSegmentsH)
					{
						pIndices[idi_idx++] = a;
						pIndices[idi_idx++] = c;
						pIndices[idi_idx++] = d;
					}
					else if (j == 1)
					{
						pIndices[idi_idx++] = a;
						pIndices[idi_idx++] = b;
						pIndices[idi_idx++] = c;
					}
					else
					{
						pIndices[idi_idx++] = a;
						pIndices[idi_idx++] = b;
						pIndices[idi_idx++] = c;
						pIndices[idi_idx++] = a;
						pIndices[idi_idx++] = c;
						pIndices[idi_idx++] = d;
					}
				}

				vtx_idx++;
			}
		}

		assert(vtx_idx == numVertices);
		assert(idi_idx == numIndices);

		g_sphere_indices_count = numIndices;

		// create ogl buffers
		{
			GLuint bufs[2];
			glGenBuffers(2, bufs);
			glBindBuffer(GL_ARRAY_BUFFER, bufs[0]);
			glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), pVertices, GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);

			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[1]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned short), pIndices, GL_STATIC_DRAW);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			g_sphere_vbo = bufs[0];
			g_sphere_ibo = bufs[1];
		}

		GLenum error = glGetError();
		assert(error == GL_NONE);

		free(pVertices);
		free(pIndices);
	}

	static Vertex3 setVertex(float x, float y, float z)
	{
		Vertex3 v;
		v.x = x;
		v.y = y;
		v.z = z;

		return v;
	}

	static void create_rect_vertices()
	{
#if INIT_ONCE
		if (g_rect_vbo != 0 && glIsBuffer(g_rect_vbo))
			return;
#endif
		Vertex pVertices[4];
		pVertices[0].position = setVertex(-1, -1, 0);
		pVertices[1].position = setVertex(+1, -1, 0);
		pVertices[2].position = setVertex(-1, +1, 0);
		pVertices[3].position = setVertex(+1, +1, 0);

		for (int i = 0; i < 4; i++)
		{
			pVertices[i].texcoord.x = 0.5f * pVertices[i].position.x + 0.5f;
			pVertices[i].texcoord.y = 0.5f * pVertices[i].position.y + 0.5f;
		}

		glGenBuffers(1, &g_rect_vbo);
		glBindBuffer(GL_ARRAY_BUFFER, g_rect_vbo);
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), pVertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		GLenum error = glGetError();
		assert(error == GL_NONE);
	}

	static void create_program()
	{
		GLuint vertexShader;
		GLuint fragShader;
#if INIT_ONCE
		if (g_program != 0 && glIsProgram(g_program))
			return;
#endif
		g_program = GL(glCreateProgram());

		{
			vertexShader = GL(glCreateShader(GL_VERTEX_SHADER));
			GL(glShaderSource(vertexShader, 1, &g_pVertStr, 0));
			GL(glCompileShader(vertexShader));
//			if (!checkCompileError(shader, src[i].type))
//				return 0;

			GL(glAttachShader(g_program, vertexShader));

			// can be deleted since the program will keep a reference
			GL(glDeleteShader(vertexShader));
		}

		{
			fragShader = GL(glCreateShader(GL_FRAGMENT_SHADER));
			GL(glShaderSource(fragShader, 1, &g_pFragStr, 0));
			GL(glCompileShader(fragShader));
			//			if (!checkCompileError(shader, src[i].type))
			//				return 0;

			GL(glAttachShader(g_program, fragShader));

			// can be deleted since the program will keep a reference
			GL(glDeleteShader(fragShader));
		}

		GL(glBindAttribLocation(g_program, POSITION, "g_Position"));
		GL(glBindAttribLocation(g_program, TEXCOORD, "g_TextureCoord"));
		GL(glLinkProgram(g_program));

		GLint texLoc = GL(glGetUniformLocation(g_program, "sparrow"));
		g_mat_loc = GL(glGetUniformLocation(g_program, "g_Mat"));
		assert(texLoc >= 0);
		assert(g_mat_loc >= 0);

		AND_LOG("texLoc: %d\n", texLoc);
		AND_LOG("g_mat_loc: %d\n", g_mat_loc);

		GL(glUseProgram(g_program));
		GL(glUniform1i(texLoc, 0));
		GL(glUseProgram(0));
	}

#ifdef __cplusplus
}
#endif