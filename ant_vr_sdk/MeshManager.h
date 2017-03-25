#pragma once

#include "BufferGL.h"
#include "Shape3D.h"
#include <vector>
#include <map>

namespace jet
{
	namespace util
	{
		// abstract class represents the mesh batching.
//		template<PolygonMode Mode>
		class MeshBatching
		{
		public:
			MeshBatching()
			{
				clearBufferTag();
			}

//			void bind();
//			void unbind();

			virtual void build() = 0;

			int indexOf(Shape3D* pShape)
			{
				if (pShape == nullptr)
					return -1;

				return Numeric::indexOf(m_pShapes, pShape);
			}

			bool contain(Shape3D* pShape)
			{
				return indexOf(pShape) >= 0;
			}

			void add(Shape3D* pShape)
			{
				int idx = -1;
				if (!pShape && (idx = indexOf(pShape)) == -1)
				{
					m_uiInstanceAddedCount++;
					m_pShapes.push_back(pShape);

					InstanceData* pInstanceData = new InstanceData;
					pInstanceData->uiIndex = m_pShapes.size();
					pInstanceData->pbTransformUpdated = &m_bTransformUpdated;
//					pInstanceData->TransformData = pShape->getWorldTransform();
					pInstanceData->UpdateMethod = MeshUpdateMethod::ADD;
//					pShape->addTransformChangedListener(pInstanceData);
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

						VertexData<MeshDataV3N3T2> vertexData;/* = pShape->getVertexData()*/;
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
						m_uiMemoryDataAddedCount++;
					}
				}
				else if (idx >= 0)
				{
					if (m_InstanceData[idx]->UpdateMethod == MeshUpdateMethod::REMOVE)
					{
						m_InstanceData[idx]->UpdateMethod = MeshUpdateMethod::NONE;
						assert(m_uiInstanceRemovedCount);

						m_uiInstanceRemovedCount--;
					}
				}
			}
			void remove(Shape3D* pShape)
			{
				if (!pShape)
					return;

				int index = indexOf(pShape);
				if (index >= 0)
				{
					if (m_InstanceData[index]->UpdateMethod == MeshUpdateMethod::ADD)
					{
						assert(m_uiInstanceAddedCount);
						m_uiInstanceAddedCount--;

						m_pShapes.erase(m_pShapes.begin() + index);
						delete m_InstanceData[index];
						m_InstanceData.erase(m_InstanceData.begin() + index);

						auto it = m_MemoryData.find(pShape->getUniqueName());
						assert(it != m_MemoryData.end());
						assert(Numeric::remove(it->second->pShapes, pShape));
						if (it->second->pShapes.empty())
						{
							delete it->second;
							m_MemoryData.erase(it);
						}
						return;
					}
					else if (m_InstanceData[index]->UpdateMethod != MeshUpdateMethod::REMOVE)  // Avoid dumplicate deletion.
					{
						m_InstanceData[index]->UpdateMethod = MeshUpdateMethod::REMOVE;
						m_uiInstanceRemovedCount++;
					}
					
				}
			}

			virtual ~MeshBatching()
			{
				for (auto it = m_InstanceData.begin(); it != m_InstanceData.end(); it++)
				{
					InstanceData* pData = *it;
					delete pData;
				}

				m_InstanceData.clear();

				for (auto it = m_MemoryData.begin(); it != m_MemoryData.end(); it++)
				{
					delete it->second;
				}

				m_MemoryData.clear();
				m_pShapes.clear();
			}

		protected:
			void clearBufferTag()
			{
//				m_bTransformUpdated = false;
				m_uiInstanceAddedCount = 0;
				m_uiInstanceRemovedCount = 0;
				m_uiMemoryDataAddedCount = 0;
				m_uiMemoryDataRemovedCount = 0;
			}

		protected:

//			GLuint getLastElementIndex() const;
			enum class MeshUpdateMethod
			{
				NONE,
				ADD,
				UPDATE_ATTRIB,
//				UPDATE_VERTEX,
				REMOVE
			};

			struct BufferRange
			{
				GLuint uiIndex;  // The index 
				// The offset in the buffer 
				GLuint uiArrayOffset;
				GLuint uiArraySize;
//				GLuint uiStride;

				GLuint uiElementOffset;
				GLuint uiElementSize;
				DataType    ElementType;

				BufferRange() :
					uiArrayOffset(0),
					uiArraySize(0),
//					uiStride(0),
					uiElementOffset(0),
					uiElementSize(0)
					,ElementType(DataType::UNKOWN)
				{}
			};
			
			VertexArrayGL* m_pVertexArray;

			enum class MeshDataType
			{
				V3N3T2,
				V4N3T2
			};
#if 0
			ArrayBufferGL* m_pArrayBuffer;
			std::vector<ElementBufferGL*> m_pElementBuffers;

			struct ShapeWrapper
			{
				std::vector<Shape3D*> pShapes;
				BufferRange BufferInfo;
				GLuint uiBaseVertex;
				MeshUpdateMethod   UpdateMethod;
				GLint iMemoryIndex;
				ShapeWrapper() :
					BufferInfo(),
					uiBaseVertex(0),
					UpdateMethod(MeshUpdateMethod::NONE),
					iMemoryIndex(-1)
				{}
			};

			struct MeshData : public TransformChangedListener
			{
				unsigned uiIndex;
				unsigned uiArraySize;
				const unsigned char* pArrayData;
				MeshDataType MeshType;

				unsigned uiBaseVertex;
				unsigned uiElementSize;
				const unsigned char* pElementData;
				DataType    ElementType;

				Transform TransformData;
				bool* pbTransformUpdated;
				bool  bDirty;
				BufferRange BufferInfo;

				void onTransformChanged() override
				{
					if (pbTransformUpdated)
					{
						*pbTransformUpdated = true;
					}

					bDirty = true;
				}
			};
#endif
			struct InstanceData
			{
				unsigned uiIndex;
				Transform TransformData;
				MeshUpdateMethod UpdateMethod;
				bool* pbTransformUpdated;
				InstanceData() :
					uiIndex(-1),
					TransformData(),
					UpdateMethod(MeshUpdateMethod::NONE),
					pbTransformUpdated(nullptr)
				{}

				void onTransformChanged()
				{
					if (pbTransformUpdated)
					{
						*pbTransformUpdated = true;
					}

					if (UpdateMethod != MeshUpdateMethod::ADD || UpdateMethod != MeshUpdateMethod::REMOVE)
					{
						UpdateMethod = MeshUpdateMethod::UPDATE_ATTRIB;
					}
				}
			};

			struct MemoryData
			{
				unsigned uiIndex;
				std::vector<Shape3D*> pShapes;
				unsigned uiArraySize;
				const unsigned char* pArrayData;
				MeshDataType MeshType;

				unsigned uiElementSize;
				const unsigned char* pElementData;
				DataType    ElementType;
			};

			bool isInstanceDirty() const { return m_uiInstanceAddedCount || m_uiInstanceRemovedCount; }
			bool isMemoryDirty()   const { return m_uiMemoryDataAddedCount || m_uiMemoryDataRemovedCount; }

			bool m_bTransformUpdated;

			unsigned m_uiInstanceAddedCount;
			unsigned m_uiInstanceRemovedCount;
			unsigned m_uiMemoryDataAddedCount;
			unsigned m_uiMemoryDataRemovedCount;

			std::vector<InstanceData*> m_InstanceData;
			std::vector<Shape3D*>	   m_pShapes;
			std::map<std::string, MemoryData*>   m_MemoryData;
		};

		template<typename T>
		static void fill(std::vector<unsigned int>& dst, unsigned count, const T* src)
		{
			dst.reserve(count);
			for (unsigned i = 0; i < count; i++)
			{
				dst.push_back(static_cast<unsigned int>(src[i]));
			}
		}

		static const unsigned MAX_BUFFER_MOMERY = 32 * 1024 * 1024;

//		template<PolygonMode Mode>
		class MeshCombineBatching :public MeshBatching //<Mode>
		{
		public:
			MeshCombineBatching() : MeshBatching/*<Mode>*/(),
				m_pVertexArray(nullptr),
				m_pArrayBuffer(nullptr),
				m_pElementBuffer(nullptr)
			{}

			void build() override
			{
				if (isInstanceDirty())
				{
					std::vector<BufferRange> removedbufferDescs;
					std::vector<BufferRange> addedbufferDescs;
					int iRemovedFirst = -1;
					unsigned uiElementCount = 0;
					for (int i = 0; i < m_pShapes.size(); i++)
					{
						if (m_InstanceData[i]->UpdateMethod == MeshUpdateMethod::REMOVE)
						{
							if (iRemovedFirst < 0)
							{
								iRemovedFirst = i;
							}
						}
						else if (m_InstanceData[i]->UpdateMethod == MeshUpdateMethod::ADD)
						{
							BufferRange range;

							auto it = m_MemoryData.find(m_pShapes[i]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							range.uiArraySize = pData->uiArraySize;
							range.uiElementSize = pData->uiElementSize;
							range.ElementType = pData->ElementType;
							range.uiIndex = i;

							addedbufferDescs.push_back(range);
							uiElementCount += pData->uiElementSize / MeasureDataTypeSize(pData->ElementType);
						}
						else if (iRemovedFirst >= 0 )
						{
							BufferRange range;
							BufferRange last;
							if (removedbufferDescs.size())
							{
								last = removedbufferDescs.back();
							}
							else if (iRemovedFirst)
							{
								last = m_BufferDescs[iRemovedFirst - 1];
							}

							range.uiArrayOffset = last.uiArrayOffset + last.uiArraySize;
							range.uiElementOffset = last.uiElementOffset + last.uiElementSize;

							auto it = m_MemoryData.find(m_pShapes[i]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							range.uiArraySize = pData->uiArraySize;
							range.uiElementSize = pData->uiElementSize;
							range.ElementType = pData->ElementType;
							range.uiIndex = i;

							removedbufferDescs.push_back(range);
							uiElementCount += pData->uiElementSize / MeasureDataTypeSize(pData->ElementType);
						}
					}
					
					unsigned uiArrayTotalBytes = 0;
					unsigned uiElementTotalBytes = 0;
					
					if (removedbufferDescs.size())
					{
						const BufferRange& back = removedbufferDescs.back();
						uiArrayTotalBytes = back.uiArrayOffset + back.uiArraySize;
						uiElementTotalBytes = back.uiElementOffset + back.uiElementSize;
					}

					for (int i = 0; i < addedbufferDescs.size(); i++)
					{
						addedbufferDescs[i].uiArrayOffset = uiArrayTotalBytes;
						uiArrayTotalBytes += addedbufferDescs[i].uiArraySize;

						addedbufferDescs[i].uiElementOffset = uiElementTotalBytes;
						uiElementTotalBytes += addedbufferDescs[i].uiElementSize;
					}

					// Fill the Array buffer data.
					if (uiArrayTotalBytes && (!m_pArrayBuffer || uiArrayTotalBytes > m_pArrayBuffer->getSize()))
					{
						if (!m_pArrayBuffer)
						{
							m_pArrayBuffer = new ArrayBufferGL<BufferUsage::STATIC_DRAW, GL_MAP_WRITE_BIT>;
						}

						m_pArrayBuffer->bind();
						// Allocate capacity
						m_pArrayBuffer->init(Numeric::min(uiArrayTotalBytes * 2u, MAX_BUFFER_MOMERY), nullptr);

						// Fill the old data
						for (int i = 0; i < iRemovedFirst; i++)
						{
							auto it = m_MemoryData.find(m_pShapes[i]->getUniqueName());
							const BufferRange& range = m_BufferDescs[i];
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							m_pArrayBuffer->update(range.uiArrayOffset, range.uiArraySize, pData->pArrayData);
						}

						// move the data
						for (int i = 0; i < removedbufferDescs.size(); i++)
						{
							const BufferRange& range = removedbufferDescs[i];
							auto it = m_MemoryData.find(m_pShapes[range.uiIndex]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							m_pArrayBuffer->update(range.uiArrayOffset, range.uiArraySize, pData->pArrayData);
						}

						// append the new shapes' buffer data
						for (int i = 0; i < addedbufferDescs.size(); i++)
						{
							const BufferRange& range = addedbufferDescs[i];
							auto it = m_MemoryData.find(m_pShapes[range.uiIndex]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							m_pArrayBuffer->update(range.uiArrayOffset, range.uiArraySize, pData->pArrayData);
						}
					}
					else
					{
						// There are enough room for the buffer data
						m_pArrayBuffer->bind();
						// move the data
						for (int i = 0; i < removedbufferDescs.size(); i++)
						{
							const BufferRange& range = removedbufferDescs[i];
							auto it = m_MemoryData.find(m_pShapes[range.uiIndex]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							m_pArrayBuffer->update(range.uiArrayOffset, range.uiArraySize, pData->pArrayData);
						}

						// append the new shapes' buffer data
						for (int i = 0; i < addedbufferDescs.size(); i++)
						{
							const BufferRange& range = addedbufferDescs[i];
							auto it = m_MemoryData.find(m_pShapes[range.uiIndex]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							m_pArrayBuffer->update(range.uiArrayOffset, range.uiArraySize, pData->pArrayData);
						}
					}

					// Fill the element buffer data.
					// measure the element data type.
					static const unsigned short MAX_SHORT = static_cast<unsigned short>(-1);
					bool bElementOverflow = (m_ElementType == DataType::UINT16 && uiElementCount > MAX_SHORT);
					if (bElementOverflow)
					{
						m_ElementType = DataType::UINT32;
						uiElementTotalBytes *= 2;
					}
					
					if (uiElementTotalBytes && (bElementOverflow || !m_pElementBuffer || uiElementTotalBytes > m_pElementBuffer->getSize()))
					{
						if (!m_pElementBuffer)
						{
							m_pElementBuffer = new ElementBufferGL<BufferUsage::STATIC_DRAW, GL_MAP_WRITE_BIT>;
						}

						m_pElementBuffer->bind();
						// Allocate capacity
						m_pElementBuffer->init(Numeric::min(uiElementTotalBytes * 2u, MAX_BUFFER_MOMERY), nullptr);

						std::vector<unsigned int> intArray;
						// Fill the old data
						for (int i = 0; i < iRemovedFirst; i++)
						{
							auto it = m_MemoryData.find(m_pShapes[i]->getUniqueName());
							const BufferRange& range = m_BufferDescs[i];
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							if (bElementOverflow)
							{
								intArray.clear();
								assert(pData->ElementType == DataType::UINT16);
								fill(intArray, pData->uiElementSize / 2, (unsigned short*)pData->pElementData);
								m_pElementBuffer->update(range.uiElementOffset, intArray.size() * sizeof(unsigned int), (GLubyte*)&intArray[0]);
							}
							else
							{
								m_pElementBuffer->update(range.uiElementOffset, range.uiElementSize, pData->pElementData);
							}
						}

						// move the data
						for (int i = 0; i < removedbufferDescs.size(); i++)
						{
							const BufferRange& range = removedbufferDescs[i];
							auto it = m_MemoryData.find(m_pShapes[range.uiIndex]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							if (bElementOverflow)
							{
								intArray.clear();
								assert(pData->ElementType == DataType::UINT16);
								fill(intArray, pData->uiElementSize / 2, (unsigned short*)pData->pElementData);
								m_pElementBuffer->update(range.uiElementOffset, intArray.size() * sizeof(unsigned int), (GLubyte*)&intArray[0]);
							}
							else
							{
								m_pElementBuffer->update(range.uiElementOffset, range.uiElementSize, pData->pElementData);
							}
							
						}

						// append the new shapes' buffer data
						for (int i = 0; i < addedbufferDescs.size(); i++)
						{
							const BufferRange& range = addedbufferDescs[i];
							auto it = m_MemoryData.find(m_pShapes[range.uiIndex]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							if (bElementOverflow)
							{
								intArray.clear();
								assert(pData->ElementType == DataType::UINT16);
								fill(intArray, pData->uiElementSize / 2, (unsigned short*)pData->pElementData);
								m_pElementBuffer->update(range.uiElementOffset, intArray.size() * sizeof(unsigned int), (GLubyte*)&intArray[0]);
							}
							else
							{
								m_pElementBuffer->update(range.uiElementOffset, range.uiElementSize, pData->pElementData);
							}
						}
					}
					else
					{
						m_pElementBuffer->bind();
						bElementOverflow = (m_ElementType == DataType::UINT32);
						std::vector<unsigned int> intArray;

						// move the data
						for (int i = 0; i < removedbufferDescs.size(); i++)
						{
							const BufferRange& range = removedbufferDescs[i];
							auto it = m_MemoryData.find(m_pShapes[range.uiIndex]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							if (bElementOverflow)
							{
								intArray.clear();
								assert(pData->ElementType == DataType::UINT16);
								fill(intArray, pData->uiElementSize / 2, (unsigned short*)pData->pElementData);
								m_pElementBuffer->update(range.uiElementOffset, intArray.size() * sizeof(unsigned int), (GLubyte*)&intArray[0]);
							}
							else
							{
								m_pElementBuffer->update(range.uiElementOffset, range.uiElementSize, pData->pElementData);
							}

						}

						// append the new shapes' buffer data
						for (int i = 0; i < addedbufferDescs.size(); i++)
						{
							const BufferRange& range = addedbufferDescs[i];
							auto it = m_MemoryData.find(m_pShapes[range.uiIndex]->getUniqueName());
							assert(it != m_MemoryData.end());
							MemoryData* pData = it->second;
							if (bElementOverflow)
							{
								intArray.clear();
								assert(pData->ElementType == DataType::UINT16);
								fill(intArray, pData->uiElementSize / 2, (unsigned short*)pData->pElementData);
								m_pElementBuffer->update(range.uiElementOffset, intArray.size() * sizeof(unsigned int), (GLubyte*)&intArray[0]);
							}
							else
							{
								m_pElementBuffer->update(range.uiElementOffset, range.uiElementSize, pData->pElementData);
							}
						}

						flushDirty();

						removedbufferDescs.insert(removedbufferDescs.end(), addedbufferDescs.begin(), addedbufferDescs.end());
						if (iRemovedFirst > 0)
						{
							m_BufferDescs.erase(m_BufferDescs.begin() + iRemovedFirst, m_BufferDescs.begin() + m_BufferDescs.size() - iRemovedFirst);
						}
						m_BufferDescs.insert(m_BufferDescs.end(), removedbufferDescs.begin(), removedbufferDescs.end());
					}
					
				}
			}
		private:
			void flushDirty()
			{
				Shape3D** deleteShapes = nullptr;
				InstanceData** deleteInstances = nullptr;
				if (m_uiInstanceRemovedCount)
				{
					deleteShapes = new Shape3D*[m_uiInstanceRemovedCount];
					deleteInstances = new InstanceData*[m_uiInstanceRemovedCount];
				}

				int index = 0;
				for (int i = 0; i < m_pShapes.size(); i++)
				{
					if (m_InstanceData[i]->UpdateMethod == MeshUpdateMethod::REMOVE)
					{
						deleteShapes[index] = m_pShapes[i];
						deleteInstances[index] = m_InstanceData[i];

						auto it = m_MemoryData.find(m_pShapes[i]->getUniqueName());
						assert(it != m_MemoryData.end());
						assert(Numeric::remove(it->second->pShapes, m_pShapes[i]));
						if (it->second->pShapes.empty())
						{
							delete it->second;
							m_MemoryData.erase(it);
						}
					}
					else if (m_InstanceData[i]->UpdateMethod == MeshUpdateMethod::ADD)
					{
						m_InstanceData[i]->UpdateMethod = MeshUpdateMethod::UPDATE_ATTRIB;
					}
				}

				if (m_uiInstanceRemovedCount)
				{
					for (int i = 0; i < m_uiInstanceRemovedCount; i++)
					{
						Numeric::remove(m_pShapes, deleteShapes[i]);
						Numeric::remove(m_InstanceData, deleteInstances[i]);

						delete deleteInstances[i];
					}

					delete[] deleteShapes;
					delete[] deleteInstances;
				}

				clearBufferTag();
			}

		private:
			VertexArrayGL* m_pVertexArray;
			ArrayBufferGL<BufferUsage::STATIC_DRAW, GL_MAP_WRITE_BIT>* m_pArrayBuffer;
			ElementBufferGL<BufferUsage::STATIC_DRAW, GL_MAP_WRITE_BIT>* m_pElementBuffer;

			DataType m_ElementType;

			std::vector<BufferRange> m_BufferDescs;
		};

#if 0
		template<PolygonMode Mode>
		class MeshCombineElementBatching : public MeshBatching<Mode>
		{

		};

		template<PolygonMode Mode>
		class MeshInstancing : public MeshBatching<Mode>
		{

		};
#endif
		class MeshManager
		{
		public:
			MeshManager()
			{

			}
			~MeshManager();
		};
	}
}

