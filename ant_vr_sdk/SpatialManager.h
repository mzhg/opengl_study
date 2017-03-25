#pragma once

#include "Node.h"
#include "gl_state_define.h"
#include "Geometry.h"
#include "BufferGL.h"
#include "BatchBuffer.h"

#include <sstream>
#include <map>
#include <unordered_map>
#include <vector>

namespace jet
{
	namespace util
	{

		struct GeometryMemoryData
		{
			// Attrib Type: Mode Type: Indexed
			MeteDataPtr MemoryData[static_cast<int>(AttribType::COUNT)];
			MeshAttrib Attrib[static_cast<int>(AttribType::COUNT)];
		
			Shape3D::Mode Mode;

			GeometryMemoryData(Shape3D::Mode mode = Shape3D::Mode::TRIANGLES) :
				Mode(mode) 
			{
				memset(Attrib, 0, sizeof(Attrib));
			}
		};

		struct ShapeKey
		{
			std::string Name;
			bool Indexed;

			ShapeKey(const std::string& name, bool indexed = true) :
				Name(name), Indexed(indexed){}

			bool operator == (const ShapeKey& o) const
			{
				return Name == o.Name && Indexed == o.Indexed;
			}
		};

		extern size_t hash_value(const ShapeKey&);

		class SpatialManager
		{
		public:
			SpatialManager();
			~SpatialManager();


			static const GeometryMemoryData& getGeometryMemoryData(const ShapeKey& key);

			void addSpatial(Spatial* pGeom);
			void removeSpatial(Spatial* pGeom);

			/**
			* Called by {@link Geometry geom} to specify that its world transform
			* has been changed.
			*
			* @param geom The Geometry whose transform changed.
			*/
			void onTransformChange(Geometry* pGeom);

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
			void onMaterialChange(Geometry * pGeom);

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
			void onMeshChange(Geometry * pGeom);

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
			void onGeometryUnassociated(Geometry * pGeom);

		private:
			enum class SpatialType
			{
				BATCH_NODE,
				NODE,
				GEOMETRY
			};

			struct SpatialData
			{
				SpatialType Type;
				TransformMeteData Transform;

			};

			std::unordered_map<Geometry*, BatchBuffer*> m_BatchBuffers;
			std::unordered_map<ShapeKey, GeometryMemoryData> m_GeometryMemoryData;
			std::unordered_map<void*, bool> m_NodeMap;
		};
	}
}

