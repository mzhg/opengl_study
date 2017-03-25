

#include "BufferGL.h"

namespace jet
{
	namespace util
	{
		bool BufferGPU::update(uint32_t offset, uint32_t size, const uint8_t* pData)
		{
			bind();
			CHECK_GL(glBufferSubData(getGLTarget(), static_cast<GLintptr>(offset), static_cast<GLsizeiptr>(size), reinterpret_cast<const GLvoid*>(pData)));
			return true;
		}

		void BufferGPU::dispose()
		{
			SAFE_RELEASE_BUFFER(m_Buffer);
		}

		uint8_t* BufferGPU::map(uint32_t offset, uint32_t length, MappingBits bits)
		{
			if (m_MappedPointer && m_MappedOffset == offset && m_MappedSize == length)
			{
				return m_MappedPointer;
			}
			else if (m_MappedPointer)
			{
				unmap();
			}

			bind();
			GLbitfield _bits;

			switch (bits)
			{
			case MappingBits::READ:
				_bits = GL_MAP_READ_BIT;
				break;
			case MappingBits::WRITE:
				_bits = GL_MAP_WRITE_BIT;
				break;
			case MappingBits::READ_WRITE:
				_bits = (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
				break;
			default:
				assert(false);
				break;
			}
#if defined(_DEBUG)
			GLStates& state = GLStates::get();
			if (state.getBindingBuffer(getTarget()) != m_Buffer)
			{
				printf("No buffer binding.\n");
				assert(false);
			}
#endif
			CHECK_GL(GLvoid* p = glMapBufferRange(getGLTarget(), offset, length, _bits));
			
			m_MappedPointer = reinterpret_cast<uint8_t*>(p);
			m_MappedOffset = offset;
			m_MappedSize = length;
			return m_MappedPointer;
		}

		void BufferGPU::unmap()
		{
			if (m_MappedPointer == nullptr)
			{
				return;
			}

			bind();
#if defined(_DEBUG)
			GLStates& state = GLStates::get();
			if (state.getBindingBuffer(getTarget()) != m_Buffer)
			{
				printf("No buffer binding.\n");
				assert(false);
			}
#endif
			glUnmapBuffer(getGLTarget());
		}

		void BufferGPU::memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size)
		{
			if (m_Buffer == 0 || size == 0 || dst_offset == src_offset)
			{
				return;
			}

			const uint32_t strider = dst_offset < src_offset ? src_offset - dst_offset : dst_offset - src_offset;
			uint32_t count = (size + strider - 1) / strider;
			uint32_t copyed_size = 0;
			if (glCopyNamedBufferSubData)
			{
				for (uint32_t i = 0; i < count; i++)
				{
					uint32_t remain_size = (size - copyed_size);
					uint32_t copy_size = remain_size > strider ? strider : remain_size;
					CHECK_GL(glCopyNamedBufferSubData(m_Buffer, m_Buffer, src_offset + i * strider, dst_offset + i * strider, copyed_size));
				}
			}
			else
			{
				GLStates& state = GLStates::get();
				state.bindBuffer(m_Buffer, BufferTarget::COPY_READ);
				state.bindBuffer(m_Buffer, BufferTarget::COPY_WRITE);

				const GLenum readTarget = ConvertBufferTargetToGLenum(BufferTarget::COPY_READ);
				const GLenum writeTarget = ConvertBufferTargetToGLenum(BufferTarget::COPY_WRITE);

				for (uint32_t i = 0; i < count; i++)
				{
					uint32_t remain_size = (size - copyed_size);
					uint32_t copy_size = remain_size > strider ? strider : remain_size;
					CHECK_GL(glCopyBufferSubData(readTarget, writeTarget, src_offset + i * strider, dst_offset + i * strider, copyed_size));
				}
			}
		}

		void BufferGPU::bind()
		{
			if (!m_Buffer)
			{
				glGenBuffers(1, &m_Buffer);
			}

			GLStates::get().bindBuffer(m_Buffer, getTarget());
		}

		void BufferGPU::unbind()
		{
			GLStates::get().bindBuffer(0, getTarget());
		}

		void VertexArrayGL::bind()
		{
			if (!m_VAO)
			{
				glGenVertexArrays(1, &m_VAO);
			}

			GLStates& state = GLStates::get();
			state.bindVAO(m_VAO);
		}

		void VertexArrayGL::load(const BufferData* pArray)
		{
#if defined(_DEBUG)
			GLStates& state = GLStates::get();
			assert(state.getBindingVAO() == m_VAO && m_VAO != 0);
#endif
			if (pArray)
			{
				pArray->ArrayBuffer->bind();
				unsigned index = 0;
				AttribDesc* pDesc = pArray->AttribDescs;
				while (index < pArray->AttribCount)
				{
					const AttribDesc& desc = pArray->AttribDescs[index];
					glEnableVertexAttribArray(desc.Index);
					glVertexAttribPointer(desc.Index, desc.Size, ConvertDataTypeToGLenum(desc.Type), desc.Normalized, desc.Stride, desc.Pointer);
					if (glVertexAttribDivisor)
					{
						glVertexAttribDivisor(desc.Index, desc.Divisor);
					}
					index++;
				}

				if (pArray->ElementBuffer)
				{
					pArray->ElementBuffer->bind();
				}
				
			}
		}
#if 0
		void VertexArrayGL::draw(unsigned numInstance)
		{
#if defined(_DEBUG)
			GLStates& state = GLStates::get();
			assert(state.getBindingVAO() == m_VAO);
#endif
			const GLenum mode = ConvertPrimitivesToGLenum(m_ElementData.Primitive);
			if (m_ElementData.ElementBuffer)
			{
				const GLenum type = ConvertDataTypeToGLenum(m_ElementData.ElementType);
				if (numInstance <= 1)
				{
					glDrawElements(mode, m_ElementData.Count, type, (const GLvoid*)0);
				}
				else
				{
					glDrawElementsInstanced(mode, m_ElementData.Count, type, (const GLvoid*)0, numInstance);
				}
			}
			else
			{
				if (numInstance <= 1)
				{
					glDrawArrays(mode, 0, m_ElementData.Count);
				}
				else
				{
					glDrawArraysInstanced(mode, 0, m_ElementData.Count, numInstance);
				}
			}
		}
#endif
		void VertexArrayGL::unbind()
		{
			GLStates& state = GLStates::get();
			state.resetVAO();
		}

		void VertexArrayGL::dispose()
		{
			SAFE_RELEASE_VERTEX_ARRAY(m_VAO);
		}

		VertexArrayGL::~VertexArrayGL()
		{
			dispose();
		}

		bool BufferGPUSharedPool::init(uint32_t size, const uint8_t* pData)
		{
			m_Pool->init(size, pData);
		}

		bool BufferGPUSharedPool::update(uint32_t offset, uint32_t size, const uint8_t* pData)
		{
			m_Pool->update(offset, size, pData);
		}

		uint8_t* BufferGPUSharedPool::map(uint32_t offset, uint32_t length, MappingBits bits)
		{
			return m_Pool->map(offset, length, bits);
		}

		void BufferGPUSharedPool::unmap()
		{
			m_Pool->unmap();
		}

		void BufferGPUSharedPool::memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size)
		{
			m_Pool->memoryCpy(dst_offset, src_offset, size);
		}

		void BufferGPUSharedPool::bind()
		{
			m_pProxyBuffer->bind();
		}

		void BufferGPUSharedPool::unbind()
		{
			m_pProxyBuffer->unbind();
		}

		BufferTarget BufferGPUSharedPool::getTarget() const
		{
			return m_pProxyBuffer->getTarget();
		}

		GLenum BufferGPUSharedPool::getGLTarget() const
		{
			return ConvertBufferTargetToGLenum(m_pProxyBuffer->getTarget());
		}

		BufferSharedPool* BufferGPUSharedPool::createPool(BufferCreator* pBufCreator, uint32_t) const
		{
			return new BufferSharedPool(pBufCreator);
		}

		BufferSharedPool* BufferGPUSharedPool::createPool(BufferGPU* pBuf) const
		{
			return new BufferSharedPool(pBuf);
		}

		BufferGPUSharedUniformPool::BufferGPUSharedUniformPool(BufferCreator* pBufCreator) :BufferGPUSharedPool(pBufCreator)
		{
			m_SharedUniformPool = dynamic_cast<BufferSharedUniformPool*>(m_Pool);
			assert(m_SharedUniformPool);
		}

		BufferGPUSharedUniformPool::BufferGPUSharedUniformPool(BufferGPU* pBuf) :BufferGPUSharedPool(pBuf)
		{
			m_SharedUniformPool = dynamic_cast<BufferSharedUniformPool*>(m_Pool);
			assert(m_SharedUniformPool);
		}

		bool BufferGPUSharedUniformPool::init(uint32_t dataSize, uint32_t numSubRanges, uint32_t numFrames)
		{
			return m_SharedUniformPool->init(dataSize, numSubRanges, numFrames);
		}

		void BufferGPUSharedUniformPool::setNumSubRanges(uint32_t numSubRanges)
		{
			m_SharedUniformPool->setNumSubRanges(numSubRanges);
		}

		void BufferGPUSharedUniformPool::setSubRangeSize(uint32_t subRangeSize)
		{
			m_SharedUniformPool->setSubRangeSize(subRangeSize);
		}

		void BufferGPUSharedUniformPool::setNumFrames(uint32_t numFrames)
		{
			m_SharedUniformPool->setNumFrames(numFrames);
		}

		uint8_t* BufferGPUSharedUniformPool::map(uint32_t subRangeIndex, uint32_t subIndex)
		{
			return m_SharedUniformPool->map(subRangeIndex, subIndex);
		}

		uint32_t BufferGPUSharedUniformPool::getDynamicOffset(int subRangeIndex, uint32_t subIndex)
		{
			return m_SharedUniformPool->getDynamicOffset(subRangeIndex, subIndex);
		}

		BufferSharedPool* BufferGPUSharedUniformPool::createPool(BufferCreator* pBufCreator, uint32_t) const
		{
			return new BufferSharedUniformPool(pBufCreator);
		}

		BufferSharedPool* BufferGPUSharedUniformPool::createPool(BufferGPU* pBuf) const
		{
			return new BufferSharedUniformPool(pBuf);
		}

		BufferGPUSubRange::BufferGPUSubRange(BufferGPU* pBuffer, uint32_t offset) : m_BuffSubRange(pBuffer, offset), m_pBuffer(pBuffer){}

		bool BufferGPUSubRange::init(uint32_t size, const uint8_t* pData)
		{
			return m_BuffSubRange.init(size, pData);
		}

		bool BufferGPUSubRange::update(uint32_t offset, uint32_t size, const uint8_t* pData)
		{
			return m_BuffSubRange.update(offset, size, pData);
		}

		uint8_t* BufferGPUSubRange::map(uint32_t offset, uint32_t length, MappingBits bits)
		{
			return m_BuffSubRange.map(offset, length, bits);
		}

		void BufferGPUSubRange::unmap()
		{
			m_BuffSubRange.unmap();
		}

		void BufferGPUSubRange::memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size)
		{
			m_BuffSubRange.memoryCpy(dst_offset, src_offset, size);
		}

		void BufferGPUSubRange::bind()
		{
			m_pBuffer->bind();
		}
		void BufferGPUSubRange::unbind()
		{
			m_pBuffer->unbind();
		}
		BufferTarget BufferGPUSubRange::getTarget() const
		{
			return m_pBuffer->getTarget();
		}

		GLenum BufferGPUSubRange::getGLTarget() const
		{
			return ConvertBufferTargetToGLenum(m_pBuffer->getTarget());
		}

		BufferGPUSharedNonUniformPool::BufferGPUSharedNonUniformPool(BufferCreator* pBufCreator, uint32_t capacity) : BufferGPUSharedPool(pBufCreator, capacity)
		{
			m_SharedNonUniformPool = dynamic_cast<BufferSharedNonUniformPool*>(m_Pool);
			assert(m_SharedNonUniformPool);
		}
		BufferGPUSharedNonUniformPool::BufferGPUSharedNonUniformPool(BufferGPU* pBuf) : BufferGPUSharedPool(pBuf)
		{
			m_SharedNonUniformPool = dynamic_cast<BufferSharedNonUniformPool*>(m_Pool);
			assert(m_SharedNonUniformPool);
		}

		BufferBean BufferGPUSharedNonUniformPool::add(uint32_t size, const uint8_t* pData)
		{
			return m_SharedNonUniformPool->add(size, pData);
		}
		bool BufferGPUSharedNonUniformPool::update(BufferBean bean, uint32_t offset, uint32_t size, const uint8_t* pData)
		{
			return m_SharedNonUniformPool->update(bean, offset, size, pData);
		}
		void BufferGPUSharedNonUniformPool::remove(BufferBean bean)
		{
			m_SharedNonUniformPool->remove(bean);
		}

		uint32_t BufferGPUSharedNonUniformPool::getOffset(BufferBean bean) const
		{
			return m_SharedNonUniformPool->getOffset(bean);
		}

		uint8_t* BufferGPUSharedNonUniformPool::map(BufferBean bean, MappingBits bits)
		{
			return m_SharedNonUniformPool->map(bean, bits);
		}
		bool BufferGPUSharedNonUniformPool::isDirty(BufferBean bean) const
		{
			return m_SharedNonUniformPool->isDirty(bean);
		}
		
		BufferSharedPool* BufferGPUSharedNonUniformPool::createPool(BufferCreator* pBufCreator, uint32_t capacity) const
		{
			return new BufferSharedNonUniformPool(pBufCreator, capacity);
		}
		BufferSharedPool* BufferGPUSharedNonUniformPool::createPool(BufferGPU* pBuf) const
		{
			return new BufferSharedNonUniformPool(pBuf);
		}
	}
}