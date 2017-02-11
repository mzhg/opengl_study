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

#ifndef _SIMPLE_SDK_COMMON_H
#define _SIMPLE_SDK_COMMON_H

#define INIT_ONCE 1

#define OGL_DEBUG 0
#if OGL_DEBUG
#define AND_LOG(...)  printf(__VA_ARGS__)
#define _s_l_(x) #x
#define _str_line_(x) _s_l_(x)
#define __STR_LINE__ _str_line_(__LINE__)
#define __STR_FILE__ _str_line_(__FILE__)
#define GL(x) x; check_gl_error( __STR_FILE__, __STR_LINE__)

#define OBJ_CHECK(name, func)  if(name == 0 || !func(name)) {AND_LOG(#name" is not valid!\n");}

#else
#define AND_LOG(...)
#define GL(x) x
#define OBJ_CHECK(name, func)
#endif


static void check_gl_error(const char* file, const char* line)
{
	char errorMsg[40];
	GLenum error = glGetError();
	switch (error)
	{
	case GL_NO_ERROR:
		return;
		break;
	case GL_INVALID_ENUM:
		strcpy_s(errorMsg, "GL_INVALID_ENUM");
		break;
	case GL_INVALID_VALUE:
		strcpy_s(errorMsg, "GL_INVALID_VALUE");
		break;
	case GL_INVALID_OPERATION:
		strcpy_s(errorMsg, "GL_INVALID_OPERATION");
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		strcpy_s(errorMsg, "GL_INVALID_FRAMEBUFFER_OPERATION");
		break;
	case GL_OUT_OF_MEMORY:
		strcpy_s(errorMsg, "GL_OUT_OF_MEMORY");
		break;
	case GL_STACK_UNDERFLOW:
		strcpy_s(errorMsg, "GL_STACK_UNDERFLOW");
		break;
	case GL_STACK_OVERFLOW:
		strcpy_s(errorMsg, "GL_STACK_OVERFLOW");
		break;
	default:
		sprintf_s(errorMsg, "Other: %d", error);
		break;
	}

	AND_LOG("Error msg occured at line %s in the file %s: %s\n", line, file, errorMsg);
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

typedef struct Vertex2
{
	float x, y;
}Vertex2;

typedef struct Vertex3
{
	float x, y, z;

	void set(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	inline float getX() const{ return x; }
	inline float getY() const{ return y; }
	inline float getZ() const{ return z; }
}Vertex3;

static Vertex3 g_background_rotate;

typedef struct Vertex
{
	Vertex3 position;
	Vertex2 texcoord;
}Vertex;

Vertex3 operator- (const Vertex3& a, const Vertex3& b);
Vertex3 operator+ (const Vertex3& a, const Vertex3& b);
Vertex3 cross(const Vertex3& a, const Vertex3& b);
float   dot(const Vertex3& a, const Vertex3& b);

//--------------------------------------------------------------------------------------
// Given a ray origin (orig) and direction (dir), and three vertices of a triangle, this
// function returns TRUE and the interpolated texture coordinates if the ray intersects 
// the triangle
//--------------------------------------------------------------------------------------
extern "C" bool ogl_intersect_triangle(const Vertex3& orig, const Vertex3& dir,
	const Vertex3& v0, const Vertex3& v1, const Vertex3& v2,
	float* t, float* u, float* v);

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

extern "C" Matrix4 identity();
extern "C" Matrix4 perspective(float fov, float aspect, float zNear, float zFar);
extern "C" Matrix4 ortho(float left, float right, float bottom, float top, float near, float far);
extern "C" void decompseRigidMatrix(const Matrix4& src, Vertex3& origin, Vertex3& xAxis, Vertex3& yAxis, Vertex3& zAxis);

/**
* Rotation matrix creation. From euler angles:<ol>
* <li> Yaw around Y axis in radians
* <li> Pitch around X axis in radians
* <li> Roll around Z axis in radians
* </ol>
* return the rotation matrix [R] = [Roll].[Pitch].[Yaw]
*/
extern "C" Matrix4 rotationYawPitchRoll(float yaw, float pitch, float roll);

/**
* Multiply the right matrix by the left and place the result in a third matrix.
* @param left The left source matrix
* @param right The right source matrix
* @return the destination matrix
*/
extern "C" Matrix4 mulMat(const Matrix4& left, const Matrix4& right);

class SimpleFBO
{
public:
	SimpleFBO(TextureGL* pTex);

	void Begin();

	void End();

	~SimpleFBO();

	TextureGL* m_Tex;

private:
	GLuint m_Framebuffer;
};

extern "C" TextureGL empty_texture();
extern "C" void release_texture(TextureGL& tex);

static const float PI = 3.1415926f;

typedef struct Billboard
{
	float scale;
	float depth;
	float yaw;
	float pitch;
	float roll;
}Billboard;

template<typename T>
T ogl_min(T a, T b)
{
	return a < b ? a : b;
}

template<typename T>
T ogl_max(T a, T b)
{
	return a > b ? a : b;
}

extern "C" void create_texture_internal(TextureGL& tex, int width, int height, int format, const char* pData);

typedef struct RenderParams
{
	Matrix4 sceneMVP;
	Matrix4 billboardMVP;
	Matrix4 tagMVP;
	Matrix4 leftSplitMVP;
	Matrix4 rightSplitMVP;

	Vertex  billBoardData[4];
	ScreenResult result;
}RenderParams;

extern "C" void update_billboard(RenderParams& render, ScreenResult& screen,
	const Size& window, const Projection& proj, const Matrix4& sceneRotation, const Billboard& billboard);

#endif