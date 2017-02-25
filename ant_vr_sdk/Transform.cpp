#include "Transform.h"
#define GLM_ENABLE_EXPERIMENTAL 1
#include <gtx\matrix_decompose.hpp>

namespace jet
{
	namespace util
	{

		void Transform::setFromMatrix(const glm::mat4& mat)
		{
			glm::vec3 skew;
			glm::vec4 perspective;

			bool sucess = glm::decompose(mat, m_f3Scale, m_f4Rotate, m_f3Translate, skew, perspective);
			assert(sucess);
			m_bDirty = false;
		}
		const glm::mat4& Transform::getCombinedMatrix()
		{
			if (m_bDirty)
			{
				glm::mat3 scalerMat = glm::mat3(m_f3Scale.x, 0.0f, 0.0f,
												0.0f, m_f3Scale.y, 0.0f,
												0.0f, 0.0f, m_f3Scale.z);
				glm::mat3 rotateMat = glm::mat3_cast(m_f4Rotate);
				glm::mat3 combinedMat = rotateMat * scalerMat;

				m_CombinedMatrix = glm::mat4(combinedMat);
				m_CombinedMatrix[3][0] = m_f3Translate.x;
				m_CombinedMatrix[3][1] = m_f3Translate.y;
				m_CombinedMatrix[3][2] = m_f3Translate.z;
				m_bDirty = false;
			}

			return m_CombinedMatrix;
		}

		void Transform::setRotateFromMatrix(const glm::mat3& mat)
		{
			glm::quat rotate = glm::quat(mat);
			setRotate(rotate);
		}
		void Transform::setRotateFromMatrix(const glm::mat4& mat)
		{
			glm::quat rotate = glm::quat(mat);
			setRotate(rotate);
		}

		void Transform::setRotateFromEulerAngle(float yaw, float roll, float pitch)
		{
			glm::quat rotate = glm::quat(glm::vec3(yaw, roll, pitch));
			setRotate(rotate);
		}

		void Transform::setRotateFromLookAt(const glm::vec3& f3Direction, const glm::vec3& f3Up)
		{
			glm::vec3 zAxis = glm::normalize(f3Direction);
			glm::vec3 xAxis = glm::normalize(glm::cross(f3Up, f3Direction));
			glm::vec3 yAxis = glm::cross(zAxis, xAxis);

			glm::mat3 rotateMat = glm::mat3(xAxis, yAxis, zAxis);
			glm::quat rotate = glm::quat(rotateMat);
			setRotate(rotate);
		}

		glm::vec3 Transform::transformInverse(const Transform& trans, const glm::vec3& v)
		{
			glm::vec3 in = v - trans.m_f3Translate;
			glm::quat rot_invert = glm::inverse(trans.m_f4Rotate);
			return (rot_invert * in) / trans.m_f3Scale;
		}

		/**
		* Changes the values of this matrix acording to it's parent.  Very similar to the concept of Node/Spatial transforms.
		* @param parent The parent matrix.
		* @return This Transform, after combining.
		*/
		Transform& Transform::combineWithParent(const Transform& parent)
		{
			glm::vec3 f3scale = m_f3Scale * parent.m_f3Scale;
			glm::quat rotate = parent.m_f4Rotate * m_f4Rotate;

			glm::vec3 f3Translate = m_f3Translate * parent.m_f3Scale;
			f3Translate = parent.m_f4Rotate * f3Translate + parent.m_f3Translate;

			setScale(f3scale);
			setRotate(rotate);
			setTranslate(f3Translate);

			return *this;
		}
	}
}
