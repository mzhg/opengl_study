#pragma once
#include <vector>
#include <string>
#include "Transform.h"
#include <memory>
#include "geometry3d.h"
#include "BoundingVolume.h"

namespace jet
{
	namespace util
	{
		/**
		* The <code>FrustumIntersect</code> enum is returned as a result
		* of a culling check operation,
		* see {@link #contains(com.jme3.bounding.BoundingVolume) }
		*/
		enum class FrustumIntersect 
		{

			/**
			* defines a constant assigned to spatials that are completely outside
			* of this camera's view frustum.
			*/
			OUTSIDE,
			/**
			* defines a constant assigned to spatials that are completely inside
			* the camera's view frustum.
			*/
			INSIDE,
			/**
			* defines a constant assigned to spatials that are intersecting one of
			* the six planes that define the view frustum.
			*/
			INTERSECTS
		};

		class Camera
		{
		public:
			Camera(int32_t width, int32_t height);

			void setName(const std::string& name) { m_strName = name; }
			void setName(const char* name) { assert(name); m_strName = name; }
			const std::string& getName() const { return m_strName; }
#if 0
			// Sets a clipPlane for this camera.
			// The clipPlane is used to recompute the projectionMatrix using the plane as the near plane
			// This technique is known as the oblique near-plane clipping method introduced by Eric Lengyel.
			void setClipPlane(Planef clipPlane, Planef::Side side);

			// Sets a clipPlane for this camera.
			// The clipPlane is used to recompute the projectionMatrix using the plane as the near plane
			// This technique is known as the oblique near-plane clipping method introduced by Eric Lengyel.
			void setClipPlane(Planef clipPlane);
#endif
			// Resizes this camera's view with the given width and height. This is
			// similar to constructing a new camera, but reusing the same Object. This
			// method is called by an associated RenderManager to notify the camera of
			// changes in the display dimensions.
			void resize(int32_t width, int32_t height, bool fixAspect = true);

#define GEN_FRUSTUM_FUNC(plane) \
	float getFrustum##plane () const {return m_fFrustume##plane ;}\
	void setFrustum##plane (float frustum##plane );

			GEN_FRUSTUM_FUNC(Bottom);
			GEN_FRUSTUM_FUNC(Top);
			GEN_FRUSTUM_FUNC(Far);
			GEN_FRUSTUM_FUNC(Near);
			GEN_FRUSTUM_FUNC(Left);
			GEN_FRUSTUM_FUNC(Right);
#undef GEN_FRUSTUM_FUNC

			// Get the position of the camera in world coordinate.
			const glm::vec3& getLocation() const { return m_f3Location; }
			// Get the rotation part of the camera.
			const glm::quat& getRotation() const { return m_Rotation; }

			// Set the location of the camera.
			void setLocation(const glm::vec3& f3Location);
			// Set the rotation of the camera.
			void setRotation(const glm::quat& rotation);

			void setLookAt(const glm::vec3& pos, const glm::vec3& center, const glm::vec3& up = glm::vec3(0, 1, 0));
			
			void setParentCameraView(const glm::mat4& mat);
			const glm::mat4& getParentCameraView() { return m_ParentViewMatrix; }

			void setFrustum(float fNear, float fFar, float fLeft, float fRight, float fTop, float fBottom);

			void setFrustumPerspective(float fFovY, float fAspect, float fNear, float fFar);

			void update();

			// returns the state of the frustum planes. So checks can be made as to which frustum plane has been examined for
			// culling thus far.
			int32_t getPlaneState() const { return m_PlaneState; }

			// sets the state to keep track of tested planes for culling.
			void setPlaneState(int32_t planeState) { m_PlaneState = planeState; }

			// Tests a bounding volume against the planes of the camera's frustum. The frustum's planes are set such that the normals all
			// face in towards the viewable scene. Therefore, if the bounding volume is on the negative side of the plane is can be culled out.
			FrustumIntersect contains(BoundingVolume* pBound);

			const Planef& getWorldPlane(int32_t planeId) const { return m_WorldPlanes[planeId];}

			// The view matrix transforms world space into eye space.
			// This matrix is usually defined by the position and orientation of the camera with the given parentCameraView matrix.
			const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }

			/**
			* Overrides the projection matrix used by the camera. Will
			* use the matrix for computing the view projection matrix as well.
			*/
			void setProjectionMatrix(const glm::mat4& mat);

			// Reset the overrided projection matrix to the internal projection matrix.
			void resetProjectionMatrix();
			// the projection matrix of the camera.
			const glm::mat4& getProjectionMatrix() const { return m_bOverrideProjection ? m_ProjectionMatrixOverride : m_ProjectionMatrix; }
			// Updates the view projection matrix.
			void updateViewProjection();

			// The result of multiplying the projection matrix by the view matrix.
			const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }
			/**
			* onFrustumChange updates the frustum to reflect any changes
			* made to the planes. The new frustum values are kept in a temporary
			* location for use when calculating the new frame. The projection
			* matrix is updated to reflect the current values of the frustum.
			*/
			void onFrustumChange();
			// onFrameChange updates the view frame of the camera.
			void onFrameChange();

			bool isParallelProjection() const { return m_bParallelProjection; }
			void setParallelProjection(bool value);

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
			glm::vec3 getWorldCoordinates(const glm::vec2& f2ScreenPos, float fProjectionZPos);

			// Converts the given position from world space to screen space.
			glm::vec3 getScreenCoordinates(const glm::vec3& worldPos);

			int32_t getWidth() const { return m_iWidth; }
			int32_t getHeight() const { return m_iHeight; }
			virtual ~Camera(){}
		protected:

			/**
			* LEFT_PLANE represents the left plane of the camera frustum.
			*/
			static const int LEFT_PLANE = 0;
			/**
			* RIGHT_PLANE represents the right plane of the camera frustum.
			*/
			static const int RIGHT_PLANE = 1;
			/**
			* BOTTOM_PLANE represents the bottom plane of the camera frustum.
			*/
			static const int BOTTOM_PLANE = 2;
			/**
			* TOP_PLANE represents the top plane of the camera frustum.
			*/
			static const int TOP_PLANE = 3;
			/**
			* FAR_PLANE represents the far plane of the camera frustum.
			*/
			static const int FAR_PLANE = 4;
			/**
			* NEAR_PLANE represents the near plane of the camera frustum.
			*/
			static const int NEAR_PLANE = 5;
			/**
			* FRUSTUM_PLANES represents the number of planes of the camera frustum.
			*/
			static const int FRUSTUM_PLANES = 6;
			/**
			* MAX_WORLD_PLANES holds the maximum planes allowed by the system.
			*/
			static const int MAX_WORLD_PLANES = 6;

			glm::vec3 m_f3Location;  // Camera's location
			glm::quat m_Rotation;    // The orientation of the camera.
			float     m_fFrustumeNear; // Distance from camera to near frustum plane.
			float	  m_fFrustumeFar;  // Distance from camera to far frustum plane.
			float     m_fFrustumeLeft; //Distance from camera to left frustum plane.
			float     m_fFrustumeRight;  //Distance from camera to right frustum plane.
			float     m_fFrustumeTop;   // Distance from camera to top frustum plane.
			float     m_fFrustumeBottom;   // Distance from camera to bottom frustum plane.

			/**
			* Percent value on display where horizontal viewing starts for this camera.
			* Default is 0.
			*/
			float	  m_fViewPortLeft;
			/**
			* Percent value on display where horizontal viewing ends for this camera.
			* Default is 1.
			*/
			float	  m_fViewPortRight;
			/**
			* Percent value on display where vertical viewing ends for this camera.
			* Default is 1.
			*/
			float     m_fViewPortTop;
			/**
			* Percent value on display where vertical viewing begins for this camera.
			* Default is 0.
			*/
			float     m_fViewPortBottom;
			/**
			* Array holding the planes that this camera will check for culling.
			*/
			Planef    m_WorldPlanes[MAX_WORLD_PLANES];
			
			int32_t	  m_iWidth;
			int32_t	  m_iHeight;
			bool	  m_bViewportChanged;
			
			glm::mat4 m_ProjectionMatrixOverride;
			
			glm::mat4 m_ParentViewMatrix;
			glm::mat4 m_ViewMatrix;
			glm::mat4 m_ProjectionMatrix;
			glm::mat4 m_ViewProjectionMatrix;
//			private BoundingBox guiBounding = new BoundingBox();
			/** The camera's name. */
			std::string m_strName;

			/**
			* A mask value set during contains() that allows fast culling of a Node's
			* children.
			*/
		private:
			int32_t m_PlaneState;/**
			* store the value for field parallelProjection
			*/
			bool m_bParallelProjection;
			bool m_bOverrideProjection;

			float     m_fCoeffLeft[2];
			float     m_fCoeffRight[2];
			float     m_fCoeffTop[2];
			float     m_fCoeffBottom[2];
		};

		typedef std::shared_ptr<Camera> CameraPtr;
	}
}

