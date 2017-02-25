#include "MeshManager.h"
#include "Numeric.h"

namespace jet
{
	namespace util
	{
		/*
		int MeshBatching::indexOf(Shape3D* pShape)
		{
			
		}

		bool MeshBatching::contain(Shape3D* pShape)
		{
			if (pShape == nullptr)
				return false;

			for (unsigned int i = 0; i < m_pShapes.size(); i++)
			{
				if (m_pShapes[i] == pShape)
				{
					return true;
				}
			}

			return false;
		}

		void MeshBatching::add(Shape3D* pShape)
		{
			if (!pShape && !contain(pShape))
			{
				m_uiInstanceAddedCount++;
				m_pShapes.push_back(pShape);

				InstanceData* pInstanceData = new InstanceData;
				pInstanceData->uiIndex = m_pShapes.size();
				pInstanceData->pbTransformUpdated = &m_bTransformUpdated;
				pInstanceData->TransformData = pShape->getWorldTransform();
				pInstanceData->UpdateMethod = MeshUpdateMethod::ADD;
				pShape->addTransformChangedListener(pInstanceData);
				m_InstanceData.push_back(pInstanceData);

				auto it = m_MemoryData.find(pShape->getUniqueName());
				if (it != m_MemoryData.end())
				{
					it->second->pShapes.push_back(pShape);
				}
				else
				{
					MemoryData* pMemoryData = new MemoryData;
					pMemoryData->uiIndex = m_MemoryData.size();

					VertexData<MeshDataV3N3T2> vertexData = pShape->getVertexData();
					pMemoryData->ElementType = vertexData.ElementType;
					pMemoryData->MeshType = MeshDataType::V3N3T2;
					pMemoryData->pArrayData = (const unsigned char*)vertexData.pVertexData;
					pMemoryData->pElementData = vertexData.pElementData;
					pMemoryData->uiArraySize = vertexData.uiVertexCount * sizeof(MeshDataV3N3T2);
					pMemoryData->uiElementSize = vertexData.uiElementCount * MeasureDataTypeSize(vertexData.ElementType);
					pMemoryData->pShapes.push_back(pShape);
					m_MemoryData.insert(std::pair<std::string, MemoryData*>(pShape->getUniqueName(), pMemoryData));
				}

				if (it->second->pShapes.size() == 1)
				{
					m_uiMemoryDataAddedCount ++;
				}
#if 0
				MeshData* meshData = new MeshData;
				meshData->ElementType = vertexData.ElementType;
				meshData->MeshType = MeshDataType::V3N3T2;
				meshData->pArrayData = (const unsigned char*)vertexData.pVertexData;
				meshData->pbTransformUpdated = &m_bTransformUpdated;
				meshData->pElementData = vertexData.pElementData;
				meshData->TransformData = pShape->getWorldTransform();
				meshData->uiArraySize = vertexData.uiVertexCount * sizeof(MeshDataV3N3T2);
				meshData->uiElementSize = vertexData.uiElementCount * MeasureDataTypeSize(vertexData.ElementType);
				meshData->uiIndex = m_pShapes.size();
				meshData->uiBaseVertex = getLastElementIndex();

				m_ArrayBufferMemory.push_back(meshData);

				auto it = m_ShapeBufferDescs.find(pShape->getUniqueName());
				if (it != m_ShapeBufferDescs.end())
				{
					it->second->iMemoryIndex = meshData->uiIndex;
					it->second->pShapes.push_back(pShape);
				}
				else
				{
					ShapeWrapper* pWrapper = new ShapeWrapper;
					pWrapper->iMemoryIndex = meshData->uiIndex;
					pWrapper->pShapes.push_back(pShape);
					pWrapper->UpdateMethod = MeshUpdateMethod::ADD;
				}
#endif
			}
		}

		void MeshBatching::remove(Shape3D* pShape)
		{
			if (!pShape)
				return;

			int index = indexOf(pShape);
			if (index >= 0)
			{
				m_InstanceData[index]->UpdateMethod = MeshUpdateMethod::REMOVE;
				m_uiInstanceRemovedCount++;

				auto it = m_MemoryData.find(pShape->getUniqueName());
				assert(it != m_MemoryData.end());
				assert(Numeric::remove(it->second->pShapes, pShape));

				if (it->second->pShapes.empty())
				{
					//					delete it->second;
					//					m_MemoryData.erase(it);

					m_uiMemoryDataRemovedCount++;
				}
			}
		}
#if 0
		GLuint MeshBatching::getLastElementIndex() const
		{
			if (m_ArrayBufferMemory.empty())
				return 0;
			else
			{
				const MeshData* last = m_ArrayBufferMemory.back();
				return last->uiBaseVertex + last->uiElementSize / MeasureDataTypeSize(last->ElementType);
			}
		}
#endif


		MeshManager::~MeshManager()
		{
		}

		*/
	}
}
