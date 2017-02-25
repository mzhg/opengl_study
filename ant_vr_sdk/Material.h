#pragma once
#include <memory>
namespace jet
{
	namespace util
	{
		class Material
		{
		public:
			Material();
			~Material();
		};

		typedef std::shared_ptr<Material> MaterialPtr;
	}
}

