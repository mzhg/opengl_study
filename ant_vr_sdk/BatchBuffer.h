#pragma once

#include "BufferGL.h"
#include <unordered_map>
#include <string>
#include "Geometry.h"
#include "SpatialManager.h"

namespace jet
{
	namespace util
	{
		
		class BatchBuffer
		{
		public:
			enum BatchType
			{
				SINGLE,
				INSTANCE,
				DYNAMIC,
				STATIC,
				ELEMENT_COMBINE,
				MULTI_DRAW
			};

			BatchBuffer(SpatialManager* pManager) : m_SpatialManager(pManager)
			{
				assert(pManager);
			}

			static BatchBuffer* create(SpatialManager* pManager, BatchType type = MULTI_DRAW);

			virtual bool addGeometry(Geometry*) = 0;
			virtual bool removeGeometry(Geometry*) = 0;

			virtual size_t getGeometryAssemblyCount() const = 0;
			virtual GeometryAssembly* getGeometryAssembly(uint32_t index) = 0;
			// Find the geometry attribute data by the specified Geometry.
			virtual GeometryAssembly* getGeometryAttribData(Geometry*) = 0; 

			/**
			* Called by {@link Geometry geom} to specify that its world transform
			* has been changed.
			*
			* @param geom The Geometry whose transform changed.
			*/
			virtual void onTransformChange(Geometry* pGeom) = 0;

			/**
			* Called by {@link Geometry geom} to specify that its
			* {@link Geometry#setMaterial(com.jme3.material.Material) material}
			* has been changed.
			*
			* @param geom The Geometry whose material changed.
			*
			* @throws UnsupportedOperationException If this implementation does
			* not support dynamic material changes.
			*/
			virtual void onMaterialChange(Geometry * pGeom) = 0;

			/**
			* Called by {@link Geometry geom} to specify that its
			* {@link Geometry#setMesh(com.jme3.scene.Mesh) mesh}
			* has been changed.
			*
			* This is also called when the geometry's
			* {@link Geometry#setLodLevel(int) lod level} changes.
			*
			* @param geom The Geometry whose mesh changed.
			*
			* @throws UnsupportedOperationException If this implementation does
			* not support dynamic mesh changes.
			*/
			virtual void onMeshChange(Geometry * pGeom) = 0;

			/**
			* Called by {@link Geometry geom} to specify that it
			* has been unassociated from its <code>GeoemtryGroupNode</code>.
			*
			* Unassociation occurs when the {@link Geometry} is
			* {@link Spatial#removeFromParent() detached} from its parent
			* {@link Node}.
			*
			* @param geom The Geometry which is being unassociated.
			*/
			virtual void onGeometryUnassociated(Geometry * pGeom)= 0;

			virtual void update(/*arguments */) = 0;


		protected:

			SpatialManager* m_SpatialManager;
#if 0
			std::unordered_map<Geometry*, GeometryInstanceData*> m_InstanceMap;
			GeometryMemoryBuffer* m_pGeometryMemoryBuffer;
#endif
		};
	}
}


