#pragma once

#include "Node.h"

namespace jet
{
	namespace util
	{
		class Shape3D : public Node
		{
		public:
			Shape3D();
			~Shape3D();

			virtual const std::string& getUniqueName() const = 0;
		};
	}
}

