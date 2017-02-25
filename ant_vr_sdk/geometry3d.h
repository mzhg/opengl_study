#pragma once
#include <glm.hpp>

namespace jet
{
	namespace util
	{
		/**
		* <code>Plane</code> defines a plane where Normal dot (x,y,z) = Constant.
		* This provides methods for calculating a "distance" of a point from this
		* plane. The distance is pseudo due to the fact that it can be negative if the
		* point is on the non-normal side of the plane.
		*
		* @author Mark Powell
		* @author Joshua Slack
		*/
		template<typename Type>
		struct Plane
		{

			enum class Side
			{
				POSITIVE,
				NEGATIVE,
				NONE
			};
		
			Plane() : f3Normal(static_cast<Type>(0)), fConstant(static_cast<Type>(0)){}
			Plane(glm::tvec3<Type, glm::highp> _normal, Type _w) : f3Normal(_normal), fConstant(_w){}

			static glm::tvec3<Type, glm::highp> getClosestPoint(const glm::tvec3<Type, glm::highp>& point, const Plane<Type>& plane)
			{
				Type t = (plane.fConstant - glm::dot(plane.f3Normal, point)) / glm::dot(plane.f3Normal, plane.f3Normal);
				return plane.f3Normal * t + point;
			}

			static glm::tvec3<Type, glm::highp> reflect(const glm::tvec3<Type, glm::highp>& point, const Plane<Type>& plane)
			{
				Type d = pseudoDistance(point, plane);
				return plane.f3Normal * (-d * static_cast<Type>(2.0)) + point;
			}

			static Type pseudoDistance(const glm::tvec3<Type, glm::highp>& point, const Plane<Type>& plane)
			{
				return glm::dot(plane.f3Normal, point) - plane.fConstant;
			}

			static Side whichSide(const glm::tvec3<Type, glm::highp>& point)
			{
				Type zero = static_cast<Type>(0);
				Type dis = pseudoDistance(point);
				if (dis < zero) 
				{
					return Side::NEGATIVE;
				}
				else if (dis > zero) 
				{
					return Side::POSITIVE;
				}
				else 
				{
					return Side::NONE;
				}
			}

			glm::tvec3<Type, glm::highp> f3Normal;
			Type fConstant;
		};

		typedef Plane<float> Planef;
		typedef Plane<double> Planed;
	}
}