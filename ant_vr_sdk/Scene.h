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
			Scene();
			~Scene();

			void setRootNode(Node* pRoot);
			void setCamera(CameraPtr pCamera);

		private:
			Node* m_RootNode;
			CameraPtr m_pCamera;
		};
	}
}

