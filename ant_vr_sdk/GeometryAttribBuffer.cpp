#include "GeometryAttribBuffer.h"
#include "Numeric.h"

namespace jet
{
	namespace util
	{

		template <BufferTarget Target>
		class BufferCreatorImpl : public BufferCreator
		{
		public:
			Buffer* create() override 
			{
				const GLenum storageBits = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
				const GLenum mappingBits = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_RANGE_BIT | GL_MAP_UNSYNCHRONIZED_BIT;
				if (glBufferStorage)
				{
					return new BufferGLStorage < Target, storageBits, storageBits >;
				}
				else
				{
					return new BufferGL<Target, BufferUsage::STATIC_DRAW, mappingBits>
				}
			}
			
			void release(Buffer* buf) override
			{
				delete buf;
			}
		};

		static BufferCreatorImpl<BufferTarget::ARRAY> g_ArrayBufferGLCreator;
		static BufferCreatorImpl<BufferTarget::ELEMENT> g_ElementBufferGLCreator;
		static const uint32_t INIT_BUFFER_CAPACITY = 128 * 1024; // 128k

		GeometryAttribBuffer::GeometryAttribBuffer(bool hasIndices, uint32_t count, AttribDesc* attribs) :m_bHaveIndices(hasIndices)
		{
			if (count)
			{
				m_AttribDescs.resize(count);
				m_AttribBuffersPool.resize(count);
				for (uint32_t i = 0; i < count; i++)
				{
					m_AttribDescs[i] = attribs[i];
					m_AttribBuffersPool[i] = new BufferGPUSharedNonUniformPool(&g_ArrayBufferGLCreator, INIT_BUFFER_CAPACITY);
				}
			}

			if (hasIndices)
			{
				m_ElementBufferPool = new BufferGPUSharedNonUniformPool(&g_ElementBufferGLCreator, INIT_BUFFER_CAPACITY);
			}
		}


		GeometryAttribBuffer::~GeometryAttribBuffer()
		{
			for (size_t i = 0; i < m_AttribBuffersPool.size(); i++)
			{
				delete m_AttribBuffersPool[i];
			}

			if (m_bHaveIndices)
			{
				delete m_ElementBufferPool;
			}
		}

		bool GeometryAttribBuffer::addAttribDesc(const AttribDesc& attrib)
		{
			if (m_bHaveIndices)
			{
				return false;
			}
			else
			{
				if (attrib.Index >= GLStates::getMaxVertexAttribs())
				{
					return false;
				}

				for (size_t i = 0; i < m_AttribDescs.size(); i++)
				{
					if (m_AttribDescs[i].Index == attrib.Index)
					{
						return false;
					}
				}
				m_AttribDescs.push_back(attrib);
				m_AttribBuffersPool.push_back(new BufferGPUSharedNonUniformPool(&g_ArrayBufferGLCreator, INIT_BUFFER_CAPACITY));
				return true;
			}
		}
		bool GeometryAttribBuffer::removeAttribDesc(const AttribDesc& attrib)
		{
			return Numeric::removeRef(m_AttribDescs, attrib);
		}

		bool GeometryAttribBuffer::removeAttribDesc(uint32_t index)
		{
			if (index >= m_AttribDescs.size())
			{
				return false;
			}
			else
			{
				m_AttribDescs.erase(m_AttribDescs.begin() + index);
				BufferGPUSharedNonUniformPool* pPool = m_AttribBuffersPool[index];
				delete pPool;
				m_AttribBuffersPool.erase(m_AttribBuffersPool.begin() + index);
				return true;
			}
		}

		bool GeometryAttribBuffer::contain(int32_t key) const
		{
			return Numeric::indexOf(m_unusedKeys, key) >= 0 || m_AttribOffsets.find(key) != m_AttribOffsets.end();
		}

		int32_t GeometryAttribBuffer::generateKey()
		{
			static int32_t g_AttribKey = 0;

			int32_t returnedKey = g_AttribKey;
			g_AttribKey++;
			m_unusedKeys.push_back(returnedKey);
			return returnedKey;
		}

		bool GeometryAttribBuffer::add(int32_t key, const GeometryAttribData& data)
		{
			int slot = -1;
			if ((slot = Numeric::indexOf(m_unusedKeys, key)) >= 0)
			{
				if (data.AttribCount != m_AttribBuffersPool.size() || bool(data.ElementSize) != m_bHaveIndices)
				{
					return false;
				}

				GeometryAttribOffset offset;
				offset.AttribOffset = new uint32_t[data.AttribCount];
				offset.AttribBeans = new BufferBean[data.AttribCount];
				for (uint32_t i = 0; i < data.AttribCount; i++)
				{
					if (data.AttribSize[i])
					{
						BufferBean bean = m_AttribBuffersPool[i]->add(data.AttribSize[i], data.AttribData[i]);
						offset.AttribOffset[i] = m_AttribBuffersPool[i]->getOffset(bean);
						offset.AttribBeans[i] = bean;
					}
					else
					{
						offset.AttribOffset[i] = INVALID_OFFSET;
						offset.AttribBeans[i] = nullptr;
					}
				}
				
				if (data.ElementSize)
				{
					// TODO Note the element data type.
					BufferBean bean = m_ElementBufferPool->add(data.ElementSize, data.ElementData);
					offset.ElementOffset = m_ElementBufferPool->getOffset(bean);
					offset.ElementBean = bean;
				}
				else
				{
					offset.ElementOffset = INVALID_OFFSET;
					offset.ElementBean = nullptr;
				}

				m_unusedKeys.erase(m_unusedKeys.begin() + slot);
				
				auto value = std::pair<int32_t, GeometryAttribOffset>(key, offset);
				m_AttribOffsets.insert(value);
			}
		}

		bool GeometryAttribBuffer::remove(int32_t key)
		{
			if (Numeric::remove(m_unusedKeys, key))
			{
				return true;
			}
			else
			{
				auto it = m_AttribOffsets.find(key);
				if (it == m_AttribOffsets.end())
				{
					return false;
				}
				else
				{
					const GeometryAttribOffset& offset = it->second;
					for (uint32_t i = 0; i < offset.AttribCount; i++)
					{
						m_AttribBuffersPool[i]->remove(offset.AttribBeans[i]);
					}

					if (offset.ElementBean)
					{
						m_ElementBufferPool->remove(offset.ElementBean);
					}

					m_AttribOffsets.erase(it);
					return true;
				}
			}
		}

		void GeometryAttribBuffer::setIndicesState(bool flag)
		{
			m_bHaveIndices = flag;
		}

		static GeometryAttribOffset g_InvalidOffset = GeometryAttribOffset();

		const GeometryAttribOffset& GeometryAttribBuffer::getAttribOffset(int32_t key) const
		{
			auto it = m_AttribOffsets.find(key);
			if (it == m_AttribOffsets.end())
			{
				return g_InvalidOffset;
			}
			else
			{
				return it->second;
			}
		}
	}
}