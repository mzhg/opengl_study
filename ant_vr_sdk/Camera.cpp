#include "Camera.h"
#include <map>
#include "Numeric.h"
namespace jet
{
	namespace util
	{
		Camera::Camera(int32_t width, int32_t height) :
			m_bViewportChanged(true),
			m_bParallelProjection(true),
			m_fFrustumeNear(1.0f),
			m_fFrustumeFar(2.0f),
			m_fFrustumeLeft(-0.5f),
			m_fFrustumeRight(0.5f),
			m_fFrustumeTop(0.5f),
			m_fFrustumeBottom(-0.5f),
			m_fViewPortLeft(0.0f),
			m_fViewPortRight(1.0f),
			m_fViewPortTop(1.0f),
			m_fViewPortBottom(0.0f),
			m_iWidth(width),
			m_iHeight(height)
		{
			onFrustumChange();
			onFrameChange();

			memset(m_fCoeffLeft, 0, sizeof(m_fCoeffLeft));
			memset(m_fCoeffRight, 0, sizeof(m_fCoeffRight));
			memset(m_fCoeffBottom, 0, sizeof(m_fCoeffBottom));
			memset(m_fCoeffTop, 0, sizeof(m_fCoeffTop));
		}

#if 0
		// Sets a clipPlane for this camera.
		// The clipPlane is used to recompute the projectionMatrix using the plane as the near plane
		// This technique is known as the oblique near-plane clipping method introduced by Eric Lengyel.
		void Camera::setClipPlane(Planef clipPlane, Planef::Side side)
		{
			float sideFactor = 1.0f;
			if (side == Planef::Side::NEGATIVE) 
			{
				sideFactor = -1.0f;
			}
			//we are on the other side of the plane no need to clip anymore.
			if (Planef::whichSide(m_f3Location, clipPlane) == side) 
			{
				return;
			}

			TempVars vars = TempVars.get();
			try {
				Matrix4f p = projectionMatrixOverride.set(projectionMatrix);

				Matrix4f ivm = viewMatrix;

				Vector3f point = clipPlane.getNormal().mult(clipPlane.getConstant(), vars.vect1);
				Vector3f pp = ivm.mult(point, vars.vect2);
				Vector3f pn = ivm.multNormal(clipPlane.getNormal(), vars.vect3);
				Vector4f clipPlaneV = vars.vect4f1.set(pn.x * sideFactor, pn.y * sideFactor, pn.z * sideFactor, -(pp.dot(pn)) * sideFactor);

				Vector4f v = vars.vect4f2.set(0, 0, 0, 0);

				v.x = (Math.signum(clipPlaneV.x) + p.m02) / p.m00;
				v.y = (Math.signum(clipPlaneV.y) + p.m12) / p.m11;
				v.z = -1.0f;
				v.w = (1.0f + p.m22) / p.m23;

				float dot = clipPlaneV.dot(v);//clipPlaneV.x * v.x + clipPlaneV.y * v.y + clipPlaneV.z * v.z + clipPlaneV.w * v.w;
				Vector4f c = clipPlaneV.multLocal(2.0f / dot);

				p.m20 = c.x - p.m30;
				p.m21 = c.y - p.m31;
				p.m22 = c.z - p.m32;
				p.m23 = c.w - p.m33;
				setProjectionMatrix(p);
			}
			finally {
				vars.release();
			}
		}

		// Sets a clipPlane for this camera.
		// The clipPlane is used to recompute the projectionMatrix using the plane as the near plane
		// This technique is known as the oblique near-plane clipping method introduced by Eric Lengyel.
		void Camera::setClipPlane(Planef clipPlane)
		{
			setClipPlane(clipPlane, Planef::whichSide(m_f3Location, clipPlane));
		}
#endif
		// Resizes this camera's view with the given width and height. This is
		// similar to constructing a new camera, but reusing the same Object. This
		// method is called by an associated RenderManager to notify the camera of
		// changes in the display dimensions.
		void Camera::resize(int32_t width, int32_t height, bool fixAspect)
		{
			if (m_iWidth == width && m_iHeight == height)
				return;

			m_iWidth = width;
			m_iHeight = height;
//			onViewPortChange();

			if (fixAspect /*&& !parallelProjection*/) 
			{
				m_fFrustumeRight = m_fFrustumeTop * Numeric::ratio(width, height);
				m_fFrustumeLeft = -m_fFrustumeRight;
				onFrustumChange();
			}
		}

		// Set the location of the camera.
		void Camera::setLocation(const glm::vec3& f3Location)
		{
			if (m_f3Location != f3Location)
			{
				m_f3Location = f3Location;
				onFrameChange();
			}
		}

		// Set the rotation of the camera.
		void Camera::setRotation(const glm::quat& rotation)
		{
			if (m_Rotation != rotation)
			{
				m_Rotation = rotation;
				onFrameChange();
			}
		}

		void Camera::setLookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up)
		{
			glm::mat4 viewMat = glm::lookAt(pos, center, up);
			if (viewMat != m_ViewMatrix)
			{
				m_ViewMatrix = viewMat;
				onFrameChange();
			}
		}

		void Camera::setParentCameraView(const glm::mat4& mat)
		{
			if (m_ParentViewMatrix != mat)
			{
				m_ParentViewMatrix = mat;
				onFrameChange();
			}
		}

		void Camera::setFrustum(float fNear, float fFar, float fLeft, float fRight, float fTop, float fBottom)
		{
			if (m_fFrustumeNear != fNear || m_fFrustumeFar != fFar || m_fFrustumeRight  != fRight||
				m_fFrustumeLeft != fLeft || m_fFrustumeTop != fTop || m_fFrustumeBottom != fBottom)
			{
				m_fFrustumeNear = fNear;
				m_fFrustumeFar = fFar;
				m_fFrustumeLeft = fLeft;
				m_fFrustumeRight = fRight;
				m_fFrustumeTop = fTop;
				m_fFrustumeBottom = fBottom;
				onFrustumChange();
			}
		}

		void Camera::setFrustumPerspective(float fFovY, float fAspect, float fNear, float fFar)
		{
			assert(!glm::isinf(fAspect) && !glm::isnan(fAspect));
			float h = glm::tan(glm::radians(fFovY) * .5f) * fNear;
			float w = h * fAspect;

			m_bParallelProjection = false;
			setFrustum(fNear, fFar, -w, w, h, -h);
		}

		void Camera::update()
		{
			onFrustumChange();
		}

		// Tests a bounding volume against the planes of the camera's frustum. The frustum's planes are set such that the normals all
		// face in towards the viewable scene. Therefore, if the bounding volume is on the negative side of the plane is can be culled out.
		FrustumIntersect Camera::contains(BoundingVolume* pBound)
		{
			if (pBound == nullptr)
				return FrustumIntersect::INSIDE;

			int mask;
			FrustumIntersect rVal = FrustumIntersect::INSIDE;

			for (int32_t planeCounter = FRUSTUM_PLANES; planeCounter >= 0; planeCounter--) 
			{
				if (planeCounter == pBound->getCheckPlane()) 
				{
					continue; // we have already checked this plane at first iteration
				}
				int32_t planeId = (planeCounter == FRUSTUM_PLANES) ? pBound->getCheckPlane() : planeCounter;
				//            int planeId = planeCounter;

				mask = 1 << (planeId);
				if ((m_PlaneState & mask) == 0) 
				{
					Planef::Side side = pBound->whichSide(m_WorldPlanes[planeId]);

					if (side == Planef::Side::NEGATIVE) 
					{
						//object is outside of frustum
						pBound->setCheckPlane(planeId);
						return FrustumIntersect::OUTSIDE;
					}
					else if (side == Planef::Side::POSITIVE)
					{
						//object is visible on *this* plane, so mark this plane
						//so that we don't check it for sub nodes.
						m_PlaneState |= mask;
					}
					else 
					{
						rVal = FrustumIntersect::INTERSECTS;
					}
				}
			}

			return rVal;
		}

		/**
		* Overrides the projection matrix used by the camera. Will
		* use the matrix for computing the view projection matrix as well.
		*/
		void Camera::setProjectionMatrix(const glm::mat4& mat)
		{
			if (!m_bOverrideProjection || m_ProjectionMatrixOverride != mat)
			{
				m_ProjectionMatrixOverride = mat;
				m_bOverrideProjection = true;
				updateViewProjection();
			}
		}

		// Reset the overrided projection matrix to the internal projection matrix.
		void Camera::resetProjectionMatrix()
		{
			if (m_bOverrideProjection)
			{
				m_bOverrideProjection = false;
				updateViewProjection();
			}
		}

		// Updates the view projection matrix.
		void Camera::updateViewProjection()
		{
			if (m_bOverrideProjection)
			{
				m_ViewProjectionMatrix = m_ProjectionMatrixOverride * m_ViewMatrix;
			}
			else
			{
				m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
			}
		}

		/**
		* onFrustumChange updates the frustum to reflect any changes
		* made to the planes. The new frustum values are kept in a temporary
		* location for use when calculating the new frame. The projection
		* matrix is updated to reflect the current values of the frustum.
		*/
		void Camera::onFrustumChange()
		{
			if (!m_bParallelProjection)
			{
				float nearSquared = m_fFrustumeNear * m_fFrustumeNear;
				float leftSquared = m_fFrustumeLeft * m_fFrustumeLeft;
				float rightSquared = m_fFrustumeRight * m_fFrustumeRight;
				float bottomSquared = m_fFrustumeBottom * m_fFrustumeBottom;
				float topSquared = m_fFrustumeTop * m_fFrustumeTop;

				float inverseLength = glm::inversesqrt(nearSquared + leftSquared);
				m_fCoeffLeft[0] = -m_fFrustumeNear * inverseLength;
				m_fCoeffLeft[1] = -m_fFrustumeLeft * inverseLength;

				inverseLength = glm::inversesqrt(nearSquared + rightSquared);
				m_fCoeffRight[0] = m_fFrustumeNear * inverseLength;
				m_fCoeffRight[1] = m_fFrustumeRight * inverseLength;

				inverseLength = glm::inversesqrt(nearSquared + bottomSquared);
				m_fCoeffBottom[0] = m_fFrustumeNear * inverseLength;
				m_fCoeffBottom[1] = -m_fFrustumeBottom * inverseLength;

				inverseLength = glm::inversesqrt(nearSquared + topSquared);
				m_fCoeffTop[0] = -m_fFrustumeNear * inverseLength;
				m_fCoeffTop[1] = m_fFrustumeTop * inverseLength;

		 		m_ProjectionMatrix = glm::frustum(m_fFrustumeLeft, m_fFrustumeRight, m_fFrustumeBottom, m_fFrustumeTop, m_fFrustumeNear, m_fFrustumeFar);
			}
			else
			{
				m_fCoeffLeft[0] = 1;
				m_fCoeffLeft[1] = 0;

				m_fCoeffRight[0] = -1;
				m_fCoeffRight[1] = 0;

				m_fCoeffBottom[0] = 1;
				m_fCoeffBottom[1] = 0;

				m_fCoeffTop[0] = -1;
				m_fCoeffTop[1] = 0;

				m_ProjectionMatrix = glm::orthoLH(m_fFrustumeLeft, m_fFrustumeRight, m_fFrustumeBottom, m_fFrustumeTop, m_fFrustumeNear, m_fFrustumeFar);
			}
		}
		// onFrameChange updates the view frame of the camera.
		void Camera::onFrameChange()
		{
			// TODO
		}

		void Camera::setParallelProjection(bool value)
		{
			if (m_bParallelProjection != value)
			{
				m_bParallelProjection = value;
				onFrustumChange();
			}
		}

		/**
		* Computes a position in World space given a screen position in screen space (0,0 to width, height)
		* and a z position in projection space ( 0 to 1 non linear).
		* This former value is also known as the Z buffer value or non linear depth buffer.
		* for more explanations on non linear z buffer see
		* http://www.sjbaker.org/steve/omniv/love_your_z_buffer.html
		*
		* To compute the projection space z from the view space z (distance from cam to object)

		* @param f2ScreenPos 2d coordinate in screen space
		* @param fProjectionZPos non linear z value in projection space
		* @return the position in world space.
		*/
		glm::vec3 Camera::getWorldCoordinates(const glm::vec2& f2ScreenPos, float fProjectionZPos)
		{
			glm::vec4 out;
			glm::mat4 inverseMat = glm::inverse(m_ViewProjectionMatrix);
			out.x = (f2ScreenPos.x / getWidth() - m_fViewPortLeft) / (m_fViewPortRight - m_fViewPortLeft) * 2.0f - 1.0f;
			out.y = (f2ScreenPos.y / getHeight() - m_fViewPortBottom) / (m_fViewPortTop - m_fViewPortBottom) * 2.0f - 1.0f;
			out.z = fProjectionZPos * 2.0f - 1.0f;
			out.w = 1.0f;

			out = inverseMat * out;
			out /= out.w;
			return glm::vec3(out);
		}

		// Converts the given position from world space to screen space.
		glm::vec3 Camera::getScreenCoordinates(const glm::vec3& worldPos)
		{
			glm::vec4 projPos = m_ViewProjectionMatrix * glm::vec4(worldPos, 1.0f);
			projPos /= projPos.w;


			glm::vec3 out;
			out.x = ((projPos.x + 1.0f) * (m_fViewPortRight - m_fViewPortLeft) / 2.0f + m_fViewPortLeft) * getWidth();
			out.y = ((projPos.y + 1.0f) * (m_fViewPortTop - m_fViewPortBottom) / 2.0f + m_fViewPortBottom) * getHeight();
			out.z = (projPos.z + 1.0f) / 2.0f;

			return out;
		}

#define GEN_FRUSTUM_FUNC_IMPL(plane) \
	void Camera::setFrustum##plane (float frustum##plane) {\
		m_fFrustume##plane = frustum##plane;\
		onFrustumChange();\
	}
	
		GEN_FRUSTUM_FUNC_IMPL(Bottom);
		GEN_FRUSTUM_FUNC_IMPL(Top);
		GEN_FRUSTUM_FUNC_IMPL(Far);
		GEN_FRUSTUM_FUNC_IMPL(Near);
		GEN_FRUSTUM_FUNC_IMPL(Left);
		GEN_FRUSTUM_FUNC_IMPL(Right);
#undef GEN_FRUSTUM_FUNC_IMPL
	}
}
