#pragma once
#include <vector>
#include <string>
#include "Transform.h"
#include <memory>

namespace jet
{
	namespace util
	{
		class LightList
		{
		public:
			LightList();
			~LightList();

			void sort(bool flag);
		};

		typedef std::shared_ptr<LightList> LightListPtr;
	}
}

