#pragma once
#include <glm.hpp>

namespace jet
{
	namespace util
	{
		template<typename Type>
		struct Plane3D
		{
			glm::tvec3<Type, glm::highp> normal;
			Type w;

			Plane3D: normal(static_cast<Type>(0)), w(static_cast<Type>(0)){}
			Plane3D(glm::tvec3<Type, highp> _normal, Type _w) : normal(_normal), w(_w){}
		};

		typedef Plane3D<float> Plane3f;
	}
}