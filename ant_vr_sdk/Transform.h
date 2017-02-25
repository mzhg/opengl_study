#pragma once
#include <glm.hpp>
#include <gtc\quaternion.hpp>
#include <gtc\matrix_transform.hpp>
namespace jet
{
	namespace util
	{

		typedef struct TransformMeteData
		{
			glm::vec3 f3Translate;
			glm::quat f4Rotate;
			glm::vec3 f3Scale;
		}TransformMeteData;

		class Transform
		{
		public:
			Transform(const glm::vec3& f3translate = glm::vec3(0.0f, 0.0f, 0.0f), const glm::quat& f4rotate = glm::quat(0,0,0,1), const glm::vec3& f3scale = glm::vec3(1,1,1)) :
				m_f3Translate(f3translate), m_f4Rotate(f4rotate), m_f3Scale(f3scale){}

			void setFromMatrix(const glm::mat4& mat);
			const glm::mat4& getCombinedMatrix();

			void setTranslate(float x, float y, float z)
			{
				glm::vec3 translate(x, y, z);
				setTranslate(translate);
			}
			void setTranslate(const glm::vec3& translate)
			{
				if (translate != m_f3Translate)
				{
					m_f3Translate = translate;
					m_bDirty = true;
				}
			}
			void setTranslateX(float x)
			{
				if (x != m_f3Translate.x)
				{
					m_f3Translate.x = x;
					m_bDirty = true;
				}
			}
			void setTranslateY(float y)
			{
				if (y != m_f3Translate.y)
				{
					m_f3Translate.y = y;
					m_bDirty = true;
				}
			}
			void setTranslateZ(float z)
			{
				if (z != m_f3Translate.z)
				{
					m_f3Translate.z = z;
					m_bDirty = true;
				}
			}

			void move(float x, float y, float z)
			{
				move(glm::vec3(x, y, z));
			}

			void move(const glm::vec3& f3Offset)
			{
				glm::vec3 f3NewTranslate = m_f3Translate + f3Offset;
				setTranslate(f3NewTranslate);
			}
			const glm::vec3& getTranslate() const{ return m_f3Translate; }

			void setRotate(float x, float y, float z, float angle)
			{
				glm::quat rotate(x, y, z, angle);
				setRotate(rotate);
			}

			void setRotateFromAxis(float fAngle, const glm::vec3& f3Axis)
			{

				glm::quat rotate = glm::angleAxis(fAngle, f3Axis);
				setRotate(rotate);
			}

			void setRotate(const glm::quat& quat)
			{
				if (quat != m_f4Rotate)
				{
					m_f4Rotate = quat;
					m_bDirty = true;
				}
			}
#if 0
			void setRotateAxis(float x, float y, float z)
			{
				glm::vec3 axis(x, y, z);
				setRotateAxis(axis);
			}

			void setRotateAxis(const glm::vec3& axis)
			{
				if (axis.x != m_f4Rotate.x || axis.y != m_f4Rotate.y || axis.z != m_f4Rotate.z)
				{
					m_f4Rotate.x = axis.x;
					m_f4Rotate.y = axis.y;
					m_f4Rotate.z = axis.z;

					m_bDirty = true;
				}
			}

			void setRotateAngle(float angle)
			{
				if (m_f4Rotate.w != angle)
				{
					m_f4Rotate.w = angle;
					m_bDirty = true;
				}
			}
#endif
			void setRotateFromMatrix(const glm::mat3& mat);
			void setRotateFromMatrix(const glm::mat4& mat);
			void setRotateFromEulerAngle(float yaw, float roll, float pitch);

			/**
			* <code>lookAt</code> is a convienence method for auto-setting the
			* quaternion based on a direction and an up vector. It computes
			* the rotation to transform the z-axis to point into 'direction'
			* and the y-axis to 'up'.
			*
			* @param direction
			*            where to look at in terms of local coordinates
			* @param up
			*            a vector indicating the local up direction.
			*            (typically {0, 1, 0} in jME.)
			*/
			void setRotateFromLookAt(const glm::vec3& f3Direction, const glm::vec3& f3Up);

			const glm::quat& getRotate() const{ return m_f4Rotate; }

			void setScale(float scalex, float scaley, float scalez)
			{
				glm::vec3 scale(scalex, scaley, scalez);
				setScale(scale);
			}

			void setScale(const glm::vec3& scale)
			{
				if (scale != m_f3Scale)
				{
					m_f3Scale = scale;
					m_bDirty = true;
				}
			}
			void setScaleX(float x)
			{
				if (x != m_f3Scale.x)
				{
					m_f3Scale.x = x;
					m_bDirty = true;
				}
			}

			void setScaleY(float y)
			{
				if (y != m_f3Scale.y)
				{
					m_f3Scale.y = y;
					m_bDirty = true;
				}
			}

			void setScaleZ(float z)
			{
				if (z != m_f3Scale.z)
				{
					m_f3Scale.z = z;
					m_bDirty = true;
				}
			}

			void scale(float x, float y, float z)
			{
				scale(glm::vec3(x, y, z));
			}

			void scale(const glm::vec3& f3Scale)
			{
				glm::vec3 f3NewScale = f3Scale * m_f3Scale;
				setScale(f3NewScale);
			}

			void rotate(const glm::quat& rotate)
			{
				glm::quat newRotate = m_f4Rotate * rotate;
				setRotate(newRotate);
			}

			const glm::vec3& getScale() const{ return m_f3Scale; }

			bool isDirty() const{ return m_bDirty; }

			static glm::vec3 transform(const Transform& trans, const glm::vec3& v)
			{
				return trans.m_f4Rotate * (v * trans.m_f3Scale) + trans.m_f3Translate;
			}

			static glm::vec3 transformInverse(const Transform& trans, const glm::vec3& v);

			/**
			* Changes the values of this matrix acording to it's parent.  Very similar to the concept of Node/Spatial transforms.
			* @param parent The parent matrix.
			* @return This Transform, after combining.
			*/
			Transform& combineWithParent(const Transform& parent);

			bool operator==(const Transform& t)
			{
				return m_f3Scale == t.m_f3Scale &&
					m_f4Rotate == t.m_f4Rotate &&
					m_f3Translate == t.m_f3Translate;
			}

			Transform& operator=(const Transform& t)
			{
				if (*this == t)
				{
					return *this;
				}
				else
				{
					m_f3Translate = t.m_f3Translate;
					m_f4Rotate = t.m_f4Rotate;
					m_f3Scale = t.m_f3Scale;
					m_bDirty = true;
					return *this;
				}
			}
		protected:
			glm::vec3 m_f3Translate;
			glm::quat m_f4Rotate;
			glm::vec3 m_f3Scale;

		private:
			bool m_bDirty;
			glm::mat4 m_CombinedMatrix;
		};

		glm::vec3 operator * (const Transform& trans, const glm::vec3& v);
	}
}

