#pragma once
#include <stdint.h>
#include <glm.hpp>
#include <memory>
#include "Transform.h"
#include "geometry3d.h"

namespace jet
{
	namespace util
	{
		// BoundingVolume defines an interface for dealing with containment of a collection of points.
		class BoundingVolume
		{
		public:
			/**
			* The type of bounding volume being used.
			*/
			enum class Type {
				/**
				* {@link BoundingSphere}
				*/
				Sphere,

				/**
				* {@link BoundingBox}.
				*/
				AABB
			};

			BoundingVolume();
			BoundingVolume(const glm::vec3& center);

			/**
			* Grabs the checkplane we should check first.
			*
			*/
			int32_t getCheckPlane() const {	return m_iCheckPlane;}

			/**
			* Sets the index of the plane that should be first checked during rendering.
			*
			* @param value
			*/
			void setCheckPlane(int32_t value) {	m_iCheckPlane = value;}

			/**
			* getType returns the type of bounding volume this is.
			*/
			virtual Type getType() const = 0;

			/**
			*
			* <code>transform</code> alters the location of the bounding volume by a
			* rotation, translation and a scalar.
			*
			* @param trans
			*            the transform to affect the bound.
			*/
			virtual void transform(const Transform& trans) = 0;

			/**
			*
			* <code>transform</code> alters the location of the bounding volume by a
			* rotation, translation and a scalar.
			*
			* @param trans
			*            the transform to affect the bound.
			*/	
			virtual void transform(const glm::mat4& trans) = 0;

			virtual Planef::Side whichSide(Planef plane) = 0;

			virtual BoundingVolume* clone(){ return this; }

			~BoundingVolume();

		protected:
			int32_t m_iCheckPlane;
			glm::vec3 m_f3Center;
		};

		typedef std::shared_ptr<BoundingVolume> BoundingVolumePtr;
	}
}

