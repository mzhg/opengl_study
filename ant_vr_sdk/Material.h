#pragma once
#include <memory>
namespace jet
{
	namespace util
	{
		enum class AttribType
		{
			POSITION,
			NORMAL,
			TEXCOORD,
			COLOR,
			TANGENT,
			ELEMENT_INDICES,
			COUNT
		};

		class Material
		{
		public:
			Material();
			~Material();
		};

		typedef std::shared_ptr<Material> MaterialPtr;
	}
}

