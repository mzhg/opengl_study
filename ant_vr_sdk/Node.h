
#pragma once

#include <string>
#include <glm.hpp>

namespace jet
{
	namespace util
	{
		class Node
		{
		public:
			Node();
			virtual ~Node();

		protected:
			glm::vec3 m_f3Translate;
			glm::vec3 m_f3Rotate;
			glm::vec3 m_f3Scale;
		};
	}
}

