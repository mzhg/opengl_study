#include "simple_sdk_common.h"

#ifdef __cplusplus
extern "C"{
#endif

TextureGL empty_texture()
{
	TextureGL tex;
	tex.width = 0;
	tex.height = 0;
	tex.textureId = 0;
	tex.owend = false;

	return tex;
}

void release_texture(TextureGL& tex)
{
	if (tex.owend)
	{
		GL(glDeleteTextures(1, &tex.textureId));
		tex.textureId = 0;
	}
}

Matrix4 identity()
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

Matrix4 perspective(float fov, float aspect, float zNear, float zFar) {
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

Matrix4 ortho(float left, float right, float bottom, float top, float near, float far) {
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
Matrix4 rotationYawPitchRoll(float yaw, float pitch, float roll){
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
Matrix4 mulMat(const Matrix4& left, const Matrix4& right) {
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

void decompseRigidMatrix(const Matrix4& src, Vertex3& origin, Vertex3& xAxis, Vertex3& yAxis, Vertex3& zAxis)
{
	float m00 = src.m00;
	float m01 = src.m10;
	float m02 = src.m20;
	float m10 = src.m01;
	float m11 = src.m11;
	float m12 = src.m21;
	float m20 = src.m02;
	float m21 = src.m12;
	float m22 = src.m22;
	float m30 = -(src.m30 * src.m00) - (src.m31 * src.m01) - (src.m32 * src.m02);
	float m31 = -(src.m30 * src.m10) - (src.m31 * src.m11) - (src.m32 * src.m12);
	float m32 = -(src.m30 * src.m20) - (src.m31 * src.m21) - (src.m32 * src.m22);

	origin.set(m30, m31, m32);
	xAxis.set(m00, m01, m02);
	yAxis.set(m10, m11, m12);
	zAxis.set(m20, m21, m22);
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

void create_texture_internal(TextureGL& tex, int width, int height, int format, const char* pData)
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

void check_gl_error(const char* file, const char* line)
{
	char errorMsg[40];
	GLenum error = glGetError();
	switch (error)
	{
	case GL_NO_ERROR:
		return;
		break;
	case GL_INVALID_ENUM:
		strcpy(errorMsg, "GL_INVALID_ENUM");
		break;
	case GL_INVALID_VALUE:
		strcpy(errorMsg, "GL_INVALID_VALUE");
		break;
	case GL_INVALID_OPERATION:
		strcpy(errorMsg, "GL_INVALID_OPERATION");
		break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
		strcpy(errorMsg, "GL_INVALID_FRAMEBUFFER_OPERATION");
		break;
#if 0
	case GL_OUT_OF_MEMORY:
		strcpy(errorMsg, "GL_OUT_OF_MEMORY");
		break;
	case GL_STACK_UNDERFLOW:
		strcpy(errorMsg, "GL_STACK_UNDERFLOW");
		break;
	case GL_STACK_OVERFLOW:
		strcpy(errorMsg, "GL_STACK_OVERFLOW");
		break;
#endif
	default:
		sprintf(errorMsg, "Other: %d", error);
		break;
	}

	AND_LOG("Error msg occured at line %s in the file %s: %s\n", line, file, errorMsg);
}

//--------------------------------------------------------------------------------------
// Given a ray origin (orig) and direction (dir), and three vertices of a triangle, this
// function returns TRUE and the interpolated texture coordinates if the ray intersects 
// the triangle
//--------------------------------------------------------------------------------------
bool ogl_intersect_triangle(const Vertex3& orig, const Vertex3& dir,
	const Vertex3& v0, const Vertex3& v1, const Vertex3& v2,
	float* t, float* u, float* v)
{
	// Find vectors for two edges sharing vert0
	Vertex3 edge1 = v1 - v0;
	Vertex3 edge2 = v2 - v0;

	// Begin calculating determinant - also used to calculate U parameter
	Vertex3 pvec = cross(dir, edge2);
//	D3DXVec3Cross(&pvec, &dir, &edge2);

	// If determinant is near zero, ray lies in plane of triangle
	float det = dot(edge1, pvec);

	Vertex3 tvec;
	if (det > 0)
	{
		tvec = orig - v0;
	}
	else
	{
		tvec = v0 - orig;
		det = -det;
	}

	if (det < 0.0001f)
		return false;

	// Calculate U parameter and test bounds
	*u = dot(tvec, pvec);
	if (*u < 0.0f || *u > det)
		return false;

	// Prepare to test V parameter
	Vertex3 qvec = cross(tvec, edge1);
//	D3DXVec3Cross(&qvec, &tvec, &edge1);

	// Calculate V parameter and test bounds
	*v = dot(dir, qvec);
	if (*v < 0.0f || *u + *v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = dot(edge2, qvec);
	float fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}


#ifdef __cplusplus
}
#endif

SimpleFBO::SimpleFBO(TextureGL* pTex)
{
	GL(glGenFramebuffers(1, &m_Framebuffer));
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer));
	{
		GL(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, pTex->textureId, 0));
	}
	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

void SimpleFBO::Begin()
{
	GL(glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer));
}

void SimpleFBO::End()
{
	GL(glBindFramebuffer(GL_FRAMEBUFFER, 0));
}

SimpleFBO::~SimpleFBO()
{
	GL(glDeleteFramebuffers(1, &m_Framebuffer));
}

Vertex3 operator- (const Vertex3& a, const Vertex3& b)
{
	Vertex3 out;
	out.x = a.x - b.x;
	out.y = a.y - b.y;
	out.z = a.z - b.z;
	return out;
}
Vertex3 operator+ (const Vertex3& a, const Vertex3& b)
{
	Vertex3 out;
	out.x = a.x + b.x;
	out.y = a.y + b.y;
	out.z = a.z + b.z;

	return out;
}
Vertex3 cross(const Vertex3& left, const Vertex3& right)
{
	Vertex3 out;
	out.set(
		left.getY() * right.getZ() - left.getZ() * right.getY(),
		right.getX() * left.getZ() - right.getZ() * left.getX(),
		left.getX() * right.getY() - left.getY() * right.getX()
		);

	return out;
}

float   dot(const Vertex3& a, const Vertex3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}