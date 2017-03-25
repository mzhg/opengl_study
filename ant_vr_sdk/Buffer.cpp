#include "Buffer.h"
#include <string.h>

namespace jet
{
	namespace util
	{
		BufferMemory::BufferMemory() : Buffer(), m_pData(nullptr){}

		BufferMemory::~BufferMemory()
		{
			if (m_pData) delete[] m_pData;
		}

		bool BufferMemory::init(uint32_t size, const uint8_t* pData)
		{
			if (m_uiDataSize != size)
			{
				m_uiDataSize = size;
				if (m_pData)
				{
					delete[] m_pData;
				}

				if (size)
				{
					m_pData = new uint8_t[size];
				}
				
			}
			
			if (size)
			{
				memcpy(m_pData, pData, size);
			}
			
			return m_pData != nullptr;
		}

		bool BufferMemory::update(uint32_t offset, uint32_t size, const uint8_t* pData)
		{
			assert(size <= m_uiDataSize);
			assert(offset + size <= m_uiDataSize);

			memcpy(m_pData + offset, pData, size);
			return true;
		}

		uint8_t* BufferMemory::map(uint32_t offset, uint32_t length, MappingBits)
		{
			assert(length <= m_uiDataSize);
			assert(offset + length <= m_uiDataSize);

			return m_pData + offset;
		}

		void BufferMemory::unmap(){}

		void BufferMemory::memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size)
		{
			assert(size <= m_uiDataSize);
			assert(size + dst_offset <= m_uiDataSize);
			assert(size + src_offset <= m_uiDataSize);

			memmove(m_pData + dst_offset, m_pData + src_offset, size);
		}

		BufferSubRange::BufferSubRange(Buffer* pBuffer, uint32_t offset) : m_pBuffer(pBuffer), m_uiOffset(offset){}
		bool BufferSubRange::init(uint32_t size, const uint8_t* pData)
		{
			m_uiDataSize = size;
			m_pBuffer->update(m_uiOffset, m_uiDataSize, pData);
			return true;
		}

		bool BufferSubRange::update(uint32_t offset, uint32_t size, const uint8_t* pData)
		{
			assert(size < m_uiDataSize);
			return m_pBuffer->update(m_uiOffset + offset, size, pData);
		}

		uint8_t* BufferSubRange::map(uint32_t offset, uint32_t length, MappingBits bits)
		{
			assert(length < m_uiDataSize);
			return m_pBuffer->map(m_uiOffset + offset, length, bits);
		}

		void BufferSubRange::unmap()
		{
			m_pBuffer->unmap();
		}

		void BufferSubRange::memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size)
		{
			assert(size < m_uiDataSize);
			m_pBuffer->memoryCpy(m_uiOffset + dst_offset, m_uiOffset + src_offset, size);
		}

		bool BufferSharedPool::init(uint32_t size, const uint8_t* pData)
		{
			m_uiDataSize = size;
			return m_pProxyBuffer->init(size, pData);
		}

		bool BufferSharedPool::update(uint32_t offset, uint32_t size, const uint8_t* pData)
		{
			assert(size < m_uiDataSize);
			return m_pProxyBuffer->update(offset, size, pData);
		}
		uint8_t* BufferSharedPool::map(uint32_t offset, uint32_t length, MappingBits bits)
		{
			assert(length < m_uiDataSize);
			return m_pProxyBuffer->map(offset, length, bits);
		}

		void BufferSharedPool::unmap()
		{
			m_pProxyBuffer->unmap();
		}

		void BufferSharedPool::memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size)
		{
			assert(size < m_uiDataSize);
			m_pProxyBuffer->memoryCpy(dst_offset, src_offset, size);
		}
	}
}