#pragma once

#include "GLStates.h"
#include "SpatialManager.h"

namespace jet
{
	namespace util
	{
		class RenderContext
		{
		public:
			RenderContext();
			~RenderContext();

			void render();
		private:

//			glm::mat4
			SpatialManager* m_pSpatialManager;
		};
	}
}

