#pragma once

#include "Node.h"
#include "gl_state_define.h"
#include "Geometry.h"
#include "BufferGL.h"
#include "RenderQueue.h"

#include <sstream>
#include <map>
#include <unordered_map>
#include <vector>

#if 0
struct jet::util::ShapeKey;



#endif

namespace jet
{
	namespace util
	{

		class BatchBuffer;
		struct GeometryMemoryData
		{
			// Attrib Type: Mode Type: Indexed
			MeteDataPtr MemoryData;
			MeshAttrib Attrib;

			GeometryMemoryData(MeteDataPtr memoryData = MeteDataPtr(), MeshAttrib attrib = MeshAttrib::NONE) :
				MemoryData(memoryData), Attrib(attrib){}
		};

		typedef struct BufferPool
		{
			AttribDesc Desc;
			BufferGPU* pBuffer;
		}BufferPool;

		typedef struct GeometryBufferData
		{
			uint32_t Count;
			BufferPool* pBufferPools;
		}GeometryBufferData;

		typedef struct GeometryInstanceData
		{
			GeometryBufferData GeometryBuffer;
			GeometryBufferData InstanceBuffer;
		}GeometryInstanceData;

		typedef struct GeometryMemoryBuffer
		{
			//			std::unordered_map<ShapeKey, BufferBean> GeometryInfos;
			BufferSharedNonUniformPool* pBufferPool;

			std::unordered_map<ShapeKey, GeometryBufferData> GeometryInfos;

			GeometryMemoryBuffer() :pBufferPool(nullptr){}

			~GeometryMemoryBuffer()
			{
				if (pBufferPool)
				{
					delete pBufferPool;
				}
			}
		}GeometryMemoryBuffer;

		class GeometryAssembly
		{
		public:
			virtual bool contain(Geometry*) const = 0;
			// Bind the geometries buffer data
			virtual void bind() = 0;
			// Unbind the buffer data
			virtual void unbind() = 0;
			// Invoking the opengl draw command.
			virtual void draw() = 0;

			virtual bool isEmpty() = 0;

#if 0
			void addBuffer(uint32_t offset, BufferSharedNonUniformPool* pBufferPool);
			void clear() { m_BufferPoolList.clear(); }

		private:
			struct BufferPool
			{

				BufferSharedNonUniformPool* pBufferPool;
			};

			std::vector<BufferPool> m_BufferPoolList;
#endif
		};

		class SpatialManager
		{
		public:
			SpatialManager();
			~SpatialManager();


			static const GeometryMemoryData& getGeometryMemoryData(const Shape3D* pShape, MeshAttrib attrib, bool indexed = true);

			// Find the geometry attribute data by the specified Geometry.
			virtual GeometryAssembly* getGeometryAttribData(Geometry*);

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

			RenderQueue& getRenderQueue(){ return m_RenderQueue; }

	//		friend class RenderQueue;
			static void releaseGeometryMomeries();

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

			// contain the spares geometries.
			std::unordered_map<Geometry*, BatchBuffer*> m_NonbatchedBuffers;
			// contain the batched nodes.
			std::unordered_map<void*, BatchBuffer*> m_BatchedBuffers;
				 
			std::unordered_map<void*, bool> m_NodeMap;

			RenderQueue m_RenderQueue;
		};
	}
}

