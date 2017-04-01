#include "BatchBuffer.h"

namespace jet
{
	namespace util
	{
		class SingleGeometryGeometryAssembly : public GeometryAssembly
		{
		public:
			SingleGeometryGeometryAssembly(const GeometryAttribData& data, const GeometryAttribDesc* pDescs, GLuint vertexCount, GLenum primitives = GL_TRIANGLES) :m_pVAO(nullptr), m_pGeometry(nullptr),
				m_pElementBuffer(nullptr), m_IndicesType(0)
			{
				init(data, pDescs, vertexCount, primitives);
			}

			void init(const GeometryAttribData& data, const GeometryAttribDesc* pDescs, GLuint vertexCount, GLenum primitives = GL_TRIANGLES)
			{
				release();

				m_VertexCount = vertexCount;
				m_Primitve = primitives;
				for (uint32_t i = 0; i < data.AttribCount; i++)
				{
					ArrayBufferGL<BufferUsage::STATIC_DRAW, 0>* pArrayBuffer = new ArrayBufferGL < BufferUsage::STATIC_DRAW, 0 >;
					pArrayBuffer->init(data.AttribSize[i], data.AttribData[i]);
					m_pArrayBuffers.push_back(pArrayBuffer);

					//						attribDesc[i] = pDescs->BufferDescs[i];
				}

				if (data.ElementSize)
				{
					m_pElementBuffer = new ElementBufferGL< BufferUsage::STATIC_DRAW, 0 >;
					m_pElementBuffer->init(data.ElementSize, data.ElementData);
					m_IndicesType = ConvertDataTypeToGLenum(data.ElementDataType);
				}

				BufferData bufferData;
				bufferData.ArrayBufferCount = data.AttribCount;
				bufferData.ArrayBufferDescs = const_cast<GeometryAttribDesc*>(pDescs);
				bufferData.ArrayBuffers = &m_pArrayBuffers[0];
				bufferData.ElementBuffer = m_pElementBuffer;

				m_pVAO = new VertexArrayGL;
				m_pVAO->load(&bufferData);
			}

			~SingleGeometryGeometryAssembly()
			{
				release();
			}

			void release()
			{
				for (size_t i = 0; i < m_pArrayBuffers.size(); i++)
				{
					delete m_pArrayBuffers[i];
				}
				m_pArrayBuffers.clear();

				SAFE_DELETE(m_pElementBuffer);
				SAFE_DELETE(m_pVAO);

				m_IndicesType = 0;
			}

			bool contain(Geometry* pGeo) const override
			{
				return pGeo == m_pGeometry && pGeo != nullptr;
			}

			// Bind the geometries buffer data
			void bind() override
			{
				if (m_pGeometry && m_pVAO)
				{
					m_pVAO->bind();
				}
			}
			// Unbind the buffer data
			void unbind() override
			{
				if (m_pGeometry && m_pVAO)
				{
					m_pVAO->unbind();
				}
			}

			// Invoking the opengl draw command.
			void draw() override
			{
				if (m_pGeometry && m_pVAO)
				{
					if (m_IndicesType != 0)
					{
						glDrawElements(m_Primitve, m_VertexCount, m_IndicesType, reinterpret_cast<void*>(0));
					}
					else
					{
						glDrawArrays(m_Primitve, 0, m_VertexCount);
					}
				}
			}

			bool isEmpty() override
			{
				return m_pGeometry != nullptr;
			}

			VertexArrayGL* m_pVAO;
			std::vector<BufferGPU*> m_pArrayBuffers;
			BufferGPU* m_pElementBuffer;
			GLenum m_IndicesType;
			Geometry* m_pGeometry;
			GLenum m_Primitve;
			GLuint m_VertexCount;
		};
		
		class SingleGeometryBatchBuffer : public BatchBuffer
		{
		public:

			SingleGeometryBatchBuffer(SpatialManager* pManager) : BatchBuffer(pManager), 
				m_pGeometry(nullptr), m_pGeometryAssembly(nullptr){}

			bool addGeometry(Geometry* pGeo) override
			{
				if (m_pGeometry)
				{
					return false;
				}
				else
				{
					m_pGeometry = pGeo;
					return pGeo != nullptr;
				}
			}

			bool removeGeometry(Geometry* pGeo) override
			{
				if (pGeo && pGeo == m_pGeometry)
				{
					m_pGeometry = nullptr;
					return true;
				}
				else
				{
					return false;
				}
			}

			// Find the geometry attribute data by the specified Geometry.
			GeometryAssembly* getGeometryAttribData(Geometry* pGeo)
			{
				if (!pGeo || pGeo != m_pGeometry)
				{
					return nullptr;
				}
				else
				{
					return m_pGeometryAssembly;
				}
				
			}

			size_t getGeometryAssemblyCount() const
			{
				return 1;
			}

			static void initIndicesData(Shape3D* pShape, GeometryAttribData& attribData)
			{
				const bool indexed = pShape->getMode() != Shape3D::Mode::POINTS;
				if (indexed)
				{
					const GeometryMemoryData& indicesData = SpatialManager::getGeometryMemoryData(pShape, MeshAttrib::INDICES, indexed);
					attribData.ElementDataType == pShape->getIndiceType();
					attribData.ElementData = const_cast<uint8_t*>(indicesData.MemoryData->pData);
					attribData.ElementSize = indicesData.MemoryData->uiLength;
				}
				else
				{
					attribData.ElementDataType == DataType::UNKOWN;
					attribData.ElementData = nullptr;
					attribData.ElementSize = 0;
				}
			}

			void initGeometryAssembly(Shape3D* pShape)
			{
				if (pShape == nullptr)
				{
					return;
				}

//				ShapeKey key = ShapeKey(pShape->getUniqueName(), (pShape->getMode() != Shape3D::Mode::POINTS));
				MeshAttrib combinedAttrib = pShape->getSupportCombinedAttrib();
				const bool indexed = pShape->getMode() != Shape3D::Mode::POINTS;
				
				std::vector<AttribDesc> attribDescs;

				if (combinedAttrib != MeshAttrib::NONE)
				{
					const GeometryMemoryData& memoryData = SpatialManager::getGeometryMemoryData(pShape, combinedAttrib, indexed);
					assert(memoryData.MemoryData.get());

					GeometryAttribData attribData;
					initIndicesData(pShape, attribData);

					attribData.AttribCount = 1;
					attribData.AttribData = const_cast<uint8_t**>(&memoryData.MemoryData->pData);
					attribData.AttribSize = &memoryData.MemoryData->uiLength;
					ParseMeshAttrib(combinedAttrib, attribDescs);

					GeometryAttribDesc bufferDesc = { attribDescs.size(), &attribDescs[0] };

					if (m_pGeometryAssembly == nullptr)
						m_pGeometryAssembly = new SingleGeometryGeometryAssembly(attribData, &bufferDesc, 
						indexed ? pShape->getIndiceCount(): pShape->getVertexCount(false), 
						Shape3D::convertModeToGLenum(pShape->getMode()));
					else
						m_pGeometryAssembly->init(attribData, &bufferDesc, 
						indexed ? pShape->getIndiceCount() : pShape->getVertexCount(false), 
						Shape3D::convertModeToGLenum(pShape->getMode()));
				}
				else
				{
					uint32_t attribCount = 0;
					const MeshAttrib* pMeshAttribs=pShape->getSupportAttribs(attribCount);
					assert(attribCount);
					std::vector<GeometryMemoryData> memoryData(attribCount);

					for (uint32_t i = 0; i < attribCount; i++)
					{
						memoryData[i] = SpatialManager::getGeometryMemoryData(pShape, pMeshAttribs[i], indexed);
					}

					std::vector<uint32_t> arrayBufferSize;
					std::vector<const uint8_t*> arrayBufferData;
					std::vector<GeometryAttribDesc> arrayBufferDesc;
					GeometryAttribData attribData;
					initIndicesData(pShape, attribData);

					for (uint32_t i = 0; i < attribCount; i++)
					{
						size_t start_loc = attribDescs.size();
						uint32_t count = ParseMeshAttrib(memoryData[i].Attrib, attribDescs);
						MeteData* pData = memoryData[i].MemoryData.get();

						arrayBufferSize.push_back(pData->uiLength);
						arrayBufferData.push_back(pData->pData);
						arrayBufferDesc.push_back(GeometryAttribDesc{ count, &attribDescs[start_loc] });
					}
					attribData.AttribCount = static_cast<uint32_t>(arrayBufferDesc.size());
					attribData.AttribData = const_cast<uint8_t**>(&arrayBufferData[0]);
					attribData.AttribSize = &arrayBufferSize[0];
					uint32_t vertexCount = indexed ? pShape->getIndiceCount() : pShape->getVertexCount(false);

					if (m_pGeometryAssembly == nullptr)
						m_pGeometryAssembly = new SingleGeometryGeometryAssembly(attribData, &arrayBufferDesc[0], vertexCount, Shape3D::convertModeToGLenum(pShape->getMode()));
					else
						m_pGeometryAssembly->init(attribData, &arrayBufferDesc[0], vertexCount, Shape3D::convertModeToGLenum(pShape->getMode()));
				}
				
			}

			GeometryAssembly* getGeometryAssembly(uint32_t index) override
			{
				if (index > 0 || !m_pGeometry || m_pGeometry->getMesh().get() == nullptr)
				{
					return nullptr;
				}
				else
				{
					if (m_pGeometryAssembly)
					{
						return m_pGeometryAssembly;
					}

					GeometryAssembly* pSharedGeometryAssembly = m_SpatialManager->getGeometryAttribData(m_pGeometry);
					if (pSharedGeometryAssembly)
					{
						return pSharedGeometryAssembly;
					}

					Shape3D* pShape = m_pGeometry->getMesh().get();
					if (!pShape)
					{
						return nullptr;
					}

					initGeometryAssembly(pShape);
					return m_pGeometryAssembly;
				}
			}

			/**
			* Called by {@link Geometry geom} to specify that its world transform
			* has been changed.
			*
			* @param geom The Geometry whose transform changed.
			*/
			void onTransformChange(Geometry* pGeom) override {/*no thing need todo.*/}

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
			void onMaterialChange(Geometry * pGeom) override {/*no thing need todo.*/};

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
			void onMeshChange(Geometry * pGeom) override
			{
				m_bMeshChanged = true;
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
			void onGeometryUnassociated(Geometry * pGeom)
			{
				m_pGeometry = nullptr;
				m_bMeshChanged = true;
			}

			void update(/*arguments */) override
			{
				if (m_bMeshChanged)
				{
					m_bMeshChanged = false;
					if ( m_pGeometryAssembly != nullptr)
					{
						if (m_pGeometry == nullptr || m_pGeometry->getMesh().get() == nullptr)
						{
							delete m_pGeometryAssembly;
							m_pGeometryAssembly = nullptr;
						}
						else
						{
							initGeometryAssembly(m_pGeometry->getMesh().get());
						}
					}
//					else if (m_pGeometry != nullptr && )
				}
			}

		private:
			Geometry* m_pGeometry;
			SingleGeometryGeometryAssembly* m_pGeometryAssembly;
			bool m_bMeshChanged;
		};

		BatchBuffer* CreateSingleBatchBuffer(SpatialManager* pManager)
		{
			return new SingleGeometryBatchBuffer(pManager);
		}
	}
}