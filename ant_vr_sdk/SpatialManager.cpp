#include "SpatialManager.h"
#include "BatchBuffer.h"

namespace jet
{
	namespace util
	{
		SpatialManager::SpatialManager()
		{
		}


		SpatialManager::~SpatialManager()
		{
			for (auto it : m_NonbatchedBuffers)
			{
				delete it.second;
			}

			m_NonbatchedBuffers.clear();

			for (auto it : m_BatchedBuffers)
			{
				delete it.second;
			}

			m_BatchedBuffers.clear();
		}

		struct MeshMomeryNode
		{
			GeometryMemoryData MemoryData;
			MeshMomeryNode* pNext;

			MeshMomeryNode(const GeometryMemoryData& memoryData = GeometryMemoryData(), MeshMomeryNode* next = nullptr) :
				MemoryData(memoryData), pNext(next){}
		};

		class MeshMomeryData
		{
		public:
			MeshMomeryData() : pHead(nullptr), pTail(nullptr){}

			const MeshMomeryNode* find(MeshAttrib attrib) const
			{
				if (!pHead)
				{
					return nullptr;
				}

				MeshMomeryNode* pNext = pHead;
				while (pNext)
				{
					if (pNext->MemoryData.Attrib == attrib)
					{
						return pNext;
					}
					else
					{
						pNext = pNext->pNext;
					}
				}

				return nullptr;
			}

			MeshMomeryNode* add(const GeometryMemoryData& memoryData)
			{
				if (!pHead)
				{
					pHead = new MeshMomeryNode(memoryData);
					return pHead;
				}

				if (!pTail)
				{
					pTail = new MeshMomeryNode(memoryData);
					pHead->pNext = pTail;
					return pTail;
				}

				MeshMomeryNode* pNext = new MeshMomeryNode(memoryData);
				pTail->pNext = pNext;
				pTail = pNext;
				return pTail;
			}

			~MeshMomeryData()
			{
				MeshMomeryNode* pCurrent = pHead;
				while (pCurrent)
				{
					MeshMomeryNode* pDelNode = pCurrent;
					pCurrent = pCurrent->pNext;
					delete pDelNode;
				}

				pHead = nullptr;
				pTail = nullptr;
			}

		private:
			MeshMomeryNode* pHead;
			MeshMomeryNode* pTail;
		};

		static GeometryMemoryData g_InvalidMemoryData = GeometryMemoryData();
		static std::unordered_map<ShapeKey, MeshMomeryData*> g_GeometryMemoryData;

		const GeometryMemoryData& SpatialManager::getGeometryMemoryData(const Shape3D* pShape, MeshAttrib attrib, bool indexed)
		{
			assert(pShape);
			const ShapeKey key = ShapeKey(pShape->getUniqueName(), indexed);
			auto it = g_GeometryMemoryData.find(key);
			if (it == g_GeometryMemoryData.end())
			{
				uint32_t vertexCount = pShape->getVertexCount(indexed);
				if (vertexCount == 0)
				{
					return g_InvalidMemoryData;
				}
				else
				{
					MeshMomeryData* pData = new MeshMomeryData();
					g_GeometryMemoryData.insert(std::pair<ShapeKey, MeshMomeryData*>(key, pData));
					it = g_GeometryMemoryData.find(key);
				}
			}
			
			MeshMomeryData* pData = it->second;
			assert(pData);

			const MeshMomeryNode* pNode = pData->find(attrib);
			if (!pNode)
			{
				// create a new data
				MeteDataPtr meteData = pShape->getVertexData(attrib, indexed);
				assert(meteData.get());

				MeshMomeryNode* pLast = pData->add(meteData);
				return pLast->MemoryData;
			}
			else
			{
				return pNode->MemoryData;
			}
		}

		void SpatialManager::releaseGeometryMomeries()
		{
			for (auto it : g_GeometryMemoryData)
			{
				delete it.second;
			}

			g_GeometryMemoryData.clear();
		}

		struct BatchedGeometries
		{
			Node* pNode;
			std::vector<Geometry*> GeometryList;

			BatchedGeometries(Node* node = nullptr) :pNode(node){}
		};

		static void recursiveSearchNode(Spatial* pNode, std::vector<Geometry*>& geometries, std::vector<BatchedGeometries>& batchGeometries)
		{
			Geometry* pGeo = dynamic_cast<Geometry*>(pNode);
			if (pGeo)
			{
				geometries.push_back(pGeo);
				return;
			}

			Node* pCurrentNode = dynamic_cast<Node*>(pNode);
			assert(pCurrentNode);
			if (pCurrentNode->isBatchNode())
			{
				size_t curr_loc = geometries.size();
				std::vector<Spatial*>& pChildren = pCurrentNode->getChildren();
				for (size_t i = 0; i < pChildren.size(); i++)
				{
					recursiveSearchNode(pChildren[i], geometries, batchGeometries);
				}
				size_t end_loc = geometries.size();

				assert(end_loc >= curr_loc);
				if (curr_loc != end_loc)
				{
					batchGeometries.push_back(BatchedGeometries());
					batchGeometries.back().pNode = pCurrentNode;
					std::vector<Geometry*>& geometryList = batchGeometries.back().GeometryList;
					geometryList.insert(geometryList.end(), geometries.begin() + curr_loc, geometries.end());
					geometries.erase(geometries.begin() + curr_loc, geometries.end());
				}
			}
			else
			{
				std::vector<Spatial*>& pChildren = pCurrentNode->getChildren();
				for (size_t i = 0; i < pChildren.size(); i++)
				{
					recursiveSearchNode(pChildren[i], geometries, batchGeometries);
				}
			}
		}

		// Find the geometry attribute data by the specified Geometry.
		GeometryAssembly* SpatialManager::getGeometryAttribData(Geometry*)
		{
			// Always return nullptr.
			return nullptr;
		}

		void SpatialManager::addSpatial(Spatial* pSpatial)
		{
			Geometry* pGeoNode = dynamic_cast<Geometry*>(pSpatial);
			if (pGeoNode)
			{
				auto it = m_NonbatchedBuffers.find(pGeoNode);
				if (it == m_NonbatchedBuffers.end())
				{
					pGeoNode->associateWithSpatialManager(this, static_cast<int>(m_NonbatchedBuffers.size()));
					BatchBuffer* pBatch = BatchBuffer::create(this, BatchBuffer::SINGLE);
					m_NonbatchedBuffers.insert(std::pair<Geometry*, BatchBuffer*>(pGeoNode, pBatch));
					pBatch->addGeometry(pGeoNode);
				}

				return;
			}

			Node* pNode = dynamic_cast<Node*>(pSpatial);
			if (pNode)
			{
				if (pNode->isBatchNode())
				{
					assert(false);
					//Not implemented so far.
				}
				else
				{
					std::vector<Spatial*>& pChildren = pNode->getChildren();
					for (size_t i = 0; i < pChildren.size(); i++)
					{
						addSpatial(pChildren[i]);
					}
				}
			}
		}

		void SpatialManager::removeSpatial(Spatial* pSpatial)
		{
			Geometry* pGeoNode = dynamic_cast<Geometry*>(pSpatial);
			if (pGeoNode)
			{
				auto it = m_NonbatchedBuffers.find(pGeoNode);
				if (it != m_NonbatchedBuffers.end())
				{
					delete it->second;
					m_NonbatchedBuffers.erase(it);
				}

				return;
			}

			Node* pNode = dynamic_cast<Node*>(pSpatial);
			if (pNode)
			{
				if (pNode->isBatchNode())
				{
					assert(false);
					//Not implemented so far.
				}
				else
				{
					std::vector<Spatial*>& pChildren = pNode->getChildren();
					for (size_t i = 0; i < pChildren.size(); i++)
					{
						removeSpatial(pChildren[i]);
					}
				}
			}
		}

		/**
		* Called by {@link Geometry geom} to specify that its world transform
		* has been changed.
		*
		* @param geom The Geometry whose transform changed.
		*/
		void SpatialManager::onTransformChange(Geometry* pGeom)
		{
			// We just only support non-batched geometry buffer so far 
		}

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
		void SpatialManager::onMaterialChange(Geometry * pGeom)
		{
			// Nothing need todo so far.
		}

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
		void SpatialManager::onMeshChange(Geometry * pGeom)
		{
			auto it = m_NonbatchedBuffers.find(pGeom);
			if (it != m_NonbatchedBuffers.end())
			{
				it->second->onMeshChange(pGeom);
			}
		}

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
		void SpatialManager::onGeometryUnassociated(Geometry * pGeom)
		{
			removeSpatial(pGeom);
		}
	}
}

