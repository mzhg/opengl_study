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