#pragma once
#include "Node.h"
#include "gl_state_define.h"
#include <sstream>

namespace jet
{
	namespace util
	{
		class Scene
		{
		public:
			Scene(SpatialManager* pManager);
			~Scene();
			/*
			void setRootNode(Node* pRoot);
			void setCamera(CameraPtr pCamera);
			*/

			Node* getRoot(){ return m_RootNode; }
			Camera* getCamera() { return m_pCamera.get(); }
			void setSize(uint32_t width, uint32_t height);
			void setPerspective(float fov, float near, float far);
			void setCameraView(const glm::mat4& viewMat);
			// Update the transform of the nodes and other stuffs.
			void update();

		private:
			/*
			glm::mat4 getCameraViewMatrix() const;
			glm::mat4 getProjectionMatrix() const;
			*/
		private:
			Node* m_RootNode;
			CameraPtr m_pCamera;
		};
	}
}

