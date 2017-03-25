#pragma once
#include "Node.h"
#include "gl_state_define.h"
#include "Geometry.h"
#include "BufferGL.h"

#include <sstream>
#include <map>
#include <unordered_map>
#include <vector>

namespace jet
{
	namespace util
	{
		class SceneManager
		{
		public:
			SceneManager();
			~SceneManager();

			void parseNode(Node* pRoot);

			static void destroy();
		private:
			struct GeometryWrapper
			{
				Geometry* pGeometry;
				bool bNode;
				Node* pBatchParent;

				GeometryWrapper(Geometry* _pGeometry = nullptr, bool bNode = false, Node* _pBatchParent = nullptr) :
					pGeometry(_pGeometry), bNode(bNode), pBatchParent(_pBatchParent){}
			};


			struct SceneNodes
			{
				std::vector<GeometryWrapper> AllGeometry;
				std::vector<Geometry*> pDynamicList;
				std::vector<GeometryWrapper> AddedGeometry;
				std::vector<GeometryWrapper> RemovedGeometry;
				// key == 0: sparse batching
				// key != 0: BatchNode batching.
				std::map<uint64_t, std::vector<Geometry*>> pBatchedList;
			};

			struct BatchedMeshBuffer
			{
	//			BufferBatching* pArrayBuffer;
	//			BufferBatching* pElementBuffer;
			};

			struct MeshBufferGL
			{
	//			BufferBatching* pArrayBuffer;
				BufferBean ArrayBean;
	//			BufferBatching* pElementBuffer;
				BufferBean ElementBean;
			};

			struct MeshMemory
			{
				MeteDataPtr pAttribData;
				MeteDataPtr pElementData;
			};

			std::map<std::string, MeshBufferGL> m_MeshToBufferGL;  //dynamic mesh buffer, not contain the batched mesh.
			std::map<std::string, MeshMemory>   m_MeshToMemory;  // All of the mesh memory.

			SceneNodes m_SceneNodes;
			std::vector<Geometry*> m_DynamicOpaque; // Triangls, Points, and Lines.
			std::vector<Geometry*> m_DynamicTranslucent;  // Only Triangles.

			struct BatchedMesh
			{
				std::vector<Geometry*> pGeometries;  // The geometries contained in this Batch.

				std::vector<ShapePtr>  pShapes;      // The shapes contained in this BufferBatch
				std::vector<int>       pShapeInstanceCount;
				
				std::vector<BufferBean> ArrayBeans;  // The 
				std::vector<BufferBean> ElementBeans;
				BatchedMeshBuffer  MeshBuffer;            // 

	//			BufferBatching*  pOverrideElementBuffer;  //combined + elements
			};

			typedef std::shared_ptr<BatchedMesh> BatchedMeshPtr;

			std::vector<BatchedMeshPtr> m_pBatchedMeshPoint;
			std::vector<BatchedMeshPtr> m_pBatchedMeshLine;
			std::vector<BatchedMeshPtr> m_pBatchedMeshOpaque;
			std::vector<BatchedMeshPtr> m_pBatchedMeshTranslucent;

		};
	}
}

