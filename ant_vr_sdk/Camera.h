#pragma once
#include <vector>
#include <string>
#include "Transform.h"
#include <memory>
#include "geometry3d.h"

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
			Camera();
			~Camera();

			
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
			
			int		  m_iWidth;
			int		  m_iHeight;
			bool	  m_bViewportChanged;
			
			glm::mat4 m_ProjectionMatrixOverride;
			
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
			int m_PlaneState;/**
			* store the value for field parallelProjection
			*/
			bool m_bParallelProjection;
			bool m_bOverrideProjection;

		};

		typedef std::shared_ptr<Camera> CameraPtr;
	}
}

