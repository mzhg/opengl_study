#include "Buffer.h"
#include "geometry2d.h"

namespace jet
{
	namespace util
	{

//		uint32_t    m_numSubRanges;
//		uint32_t    m_subRangeSize;
//		uint32_t    m_index;
//		uint32_t    m_numFrames;
//		uint32_t    m_bufferSize;
		BufferSharedUniformPool::BufferSharedUniformPool(BufferCreator* pBufCreator):
			BufferSharedPool(pBufCreator), m_numSubRanges(0), 
			m_subRangeSize(0), m_index(0), m_numFrames(0), m_bufferSize(0)
		{}
		BufferSharedUniformPool::BufferSharedUniformPool(Buffer* pBuf):
			BufferSharedPool(pBuf), m_numSubRanges(0),
			m_subRangeSize(0), m_index(0), m_numFrames(0), m_bufferSize(0)
		{}

		bool BufferSharedUniformPool::init(uint32_t dataSize, uint32_t numSubRanges, uint32_t numFrames)
		{
			// Use dataSize as the size of a single sub-range...
			m_subRangeSize = dataSize;

			// Account for the number of sub-ranges...
			m_numSubRanges = numSubRanges;

			// Round the buffer size up to multiples of 4 bytes
			m_uiDataSize = ((m_numSubRanges * m_subRangeSize) + 3) & 0xFFFFFFFC;

			// Account for the number of frames in the ring-buffer...
			m_numFrames = numFrames;
			m_bufferSize = m_uiDataSize * m_numFrames;

			return initBuffers();
		}

		void BufferSharedUniformPool::setNumSubRanges(uint32_t numSubRanges)
		{
			// After changing these values, we'll have to create a new buffer, so
			// ensure we're not in the middle of using any portion of the VBO first
//			Finish();

			m_numSubRanges = numSubRanges;
			// Round the buffer size up to multiples of 4 bytes
			m_uiDataSize = ((m_numSubRanges * m_subRangeSize) + 3) & 0xFFFFFFFC;
			m_bufferSize = m_uiDataSize * m_numFrames;

			// Recreate the VBO with the new settings
			initBuffers();
		}

		void BufferSharedUniformPool::setSubRangeSize(uint32_t subRangeSize)
		{
			// After changing these values, we'll have to create a new buffer, so
			// ensure we're not in the middle of using any portion of the VBO first
//			Finish();

			m_subRangeSize = subRangeSize;
			// Round the buffer size up to multiples of 4 bytes
			m_uiDataSize = ((m_numSubRanges * m_subRangeSize) + 3) & 0xFFFFFFFC;
			m_bufferSize = m_uiDataSize * m_numFrames;

			// Recreate the VBO with the new settings
			initBuffers();
		}

		void BufferSharedUniformPool::setNumFrames(uint32_t numFrames)
		{
			// After changing these values, we'll have to create a new buffer, so
			// ensure we're not in the middle of using any portion of the VBO first
//			Finish();

			m_numFrames = numFrames;
			m_bufferSize = m_uiDataSize * m_numFrames;

			// Recreate the VBO with the new settings
			initBuffers();
		}

		uint8_t* BufferSharedUniformPool::map(uint32_t subRangeIndex, uint32_t subIndex)
		{
			uint8_t* pData = m_pProxyBuffer->map((subIndex * m_uiDataSize) + (subRangeIndex * m_subRangeSize), m_uiDataSize, MappingBits::READ_WRITE);
			return pData;
		}

		uint32_t BufferSharedUniformPool::getDynamicOffset(int subRangeIndex, uint32_t subIndex)
		{
			return Buffer::getDynamicOffset() + (subRangeIndex * m_subRangeSize) + (subIndex * m_uiDataSize);
		}

		bool BufferSharedUniformPool::initBuffers()
		{
			if (m_bufferSize == 0)
			{
				return false;
			}

			return m_pProxyBuffer->init(m_bufferSize);
		}
#if 0
		enum class  BatchAddingBehavior
		{
			APPEND,
			APPEND_COMPACT,
			/* Unspported so far.
			INSERT,
			INSERT_COMPACT,
			*/
			DEFAULT = APPEND
		};

		enum class  BatchRemoveBehavior
		{
			REMOVE,
			REMOVE_COMPACT,
			REMOVE_FILL,
			DEFAULT = REMOVE
		};

		
#endif

		
		BufferSharedNonUniformPool::BufferSharedNonUniformPool(BufferCreator* pBufCreator, uint32_t capacity) :BufferSharedPool(pBufCreator),
			m_pHeader(nullptr), m_uiRegionCount(0), m_uiConsumedSize(0)
		{
			if (capacity)
			{
				assert(m_pProxyBuffer->init(capacity));

				m_pTail = new Region;
				m_pTail->Interval = { 0, capacity };
				m_pTail->iIndex++;
			}
			else
			{
				m_pTail = nullptr;
			}
		}
		BufferSharedNonUniformPool::BufferSharedNonUniformPool(Buffer* pBuf) : BufferSharedPool(pBuf),
			m_pHeader(nullptr), m_uiRegionCount(0), m_uiConsumedSize(0)
		{
			if (pBuf->getSize())
			{
//				assert(m_pProxyBuffer->init(capacity));

				m_pTail = new Region;
				m_pTail->Interval = { 0, pBuf->getSize() };
				m_pTail->iIndex++;
			}
			else
			{
				m_pTail = nullptr;
			}
		}

		bool BufferSharedNonUniformPool::isDirty(BufferBean bean) const
		{
			return reinterpret_cast<BufferBeanImpl*>(bean)->pData->bDirty;
		}

		uint32_t BufferSharedNonUniformPool::getOffset(BufferBean bean) const
		{
			return reinterpret_cast<BufferBeanImpl*>(bean)->pData->Interval.Min;
		}

		void BufferSharedNonUniformPool::makeRegionsDirty()
		{
			Region* pNext = m_pHeader;
			while (pNext && pNext->pBean)
			{
				pNext->bDirty = true;
				pNext = pNext->pRight;
			}
		}

		BufferBean BufferSharedNonUniformPool::add(uint32_t size, const uint8_t* pData)
		{
			if (size == 0)
			{
				return nullptr;
			}

			if (size && pData == nullptr)
			{
//				assert(m_pTail);
//				assert(m_pProxyBuffer->getSize() > m_uiConsumedSize);

				const uint32_t uiRemainCapacity = m_pProxyBuffer->getSize() - m_uiConsumedSize;
				bool bReallocated = false;
				if (uiRemainCapacity < size)
				{
//					m_pBuffer->bind();

					uint32_t bufferSize = m_pProxyBuffer->getSize();
					uint32_t deserdSize = m_uiConsumedSize + size;
					while (bufferSize < deserdSize)
						bufferSize *= 2;

					if (m_pBufferCreator)
					{
						if (m_pTail == nullptr || m_pHeader == nullptr)
						{
							m_pProxyBuffer->init(bufferSize, nullptr/*, m_pBuffer->getUsage()*/);
						}
						else
						{
							Buffer* newBuf = m_pBufferCreator->create();
							assert(newBuf->init(bufferSize));
							uint8_t* pSrcData = m_pProxyBuffer->map(0, m_pProxyBuffer->getSize(), MappingBits::READ);
							if (pSrcData)
							{
								uint8_t* pDstData = newBuf->map(0, m_pProxyBuffer->getSize(), MappingBits::WRITE);
								memcpy(pDstData, pSrcData, m_pProxyBuffer->getSize());
								m_pProxyBuffer->unmap();
								newBuf->unmap();
							}
							else
							{
								m_pProxyBuffer->unmap();
								makeRegionsDirty();
							}
						}
					}

					if (m_pTail == nullptr)
					{
						m_pTail = new Region;
						m_pTail->Interval = { 0, bufferSize };
						m_pTail->iIndex++;
					}

					bReallocated = true;
				}
				
#if 0
				if (!bReallocated && (behavior == BatchAddingBehavior::APPEND_COMPACT /*|| behavior == BatchAddingBehavior::INSERT_COMPACT*/))
				{
//					bReallocated = true;
				}
#endif			
				if (bReallocated /*|| m_pTail->Interval.length() < pBean->uiBufferSize*/)
				{
//					result = BatchAddingBehavior::APPEND_COMPACT;
//					flushRegions();  // TODO Do not neeed this
				}

//				assert(m_pTail->pBean == nullptr && m_pTail->Interval.length() >= pBean->uiBufferSize);
				m_pProxyBuffer->update(m_pTail->Interval.Min, size, pData);  // TODO bad performance.  Can be optimise in buffer.
				m_uiConsumedSize += size;

				BufferBeanImpl* pBean = new BufferBeanImpl(size, pData);
				addBufferBeanToTail(pBean);

				// TODO 目前未实现的功能：
				// 1，中间空白区域的插入功能
				// 2，Element BaseVertex的功能 
				return pBean;
			}
		}

		bool BufferSharedNonUniformPool::update(BufferBean bean, uint32_t offset, uint32_t size, const uint8_t* pData)
		{
			BufferBeanImpl* pBean = reinterpret_cast<BufferBeanImpl*>(bean);
			if (pBean)
			{
				Region* pRegion = pBean->pData;
				uint8_t* pDst = m_pProxyBuffer->map(pRegion->Interval.Min + offset, size, MappingBits::WRITE);
				if (pDst)
				{
					memcpy(pDst, pData, size);
					m_pProxyBuffer->unmap();
					pRegion->bDirty = false;
					return true;
				}
				else
				{
					pRegion->bDirty = false;
					return m_pProxyBuffer->update(pRegion->Interval.Min + offset, size, pData);
				}
			}
			return false;
		}

		void BufferSharedNonUniformPool::remove(BufferBean bean)
		{
//			assert(m_pBuffer);
			BufferBeanImpl* pBean = reinterpret_cast<BufferBeanImpl*>(bean);
			if (pBean && pBean->pData)
			{
				Region* const pRegion = pBean->pData;
				delete pRegion->pBean;
				pRegion->pBean = nullptr;

				uint32_t length = pRegion->Interval.length();
				m_uiConsumedSize -= length;

				// merge the sapce
				Region* pLeft = pRegion->pLeft;
				Region* const pRight = pRegion->pRight;

				if (pRight == nullptr)
				{
					assert(false);  // The tail must be non-null
				}

				
				if (pLeft == nullptr)
				{ // This is the head.
					m_pHeader = pRight;
					pRight->pLeft = nullptr;
				}
				else
				{
					pLeft->pRight = pRight;
					pRight->pLeft = pLeft;
				}

				Region* pNext = pRight;
				while (pNext)
				{
					pNext->Interval -= length;
					pNext = pNext->pRight;
				}

				// move the buffer data.
				m_pProxyBuffer->memoryCpy(pRegion->Interval.Min, pRight->Interval.Min, length);
				delete pRegion;
			}
		}

		uint8_t* BufferSharedNonUniformPool::map(BufferBean bean, MappingBits bits)
		{
			BufferBeanImpl* pBean = reinterpret_cast<BufferBeanImpl*>(bean);
			return m_pProxyBuffer->map(pBean->pData->Interval.Min, pBean->uiBufferSize, bits);
		}

		void BufferSharedNonUniformPool::addBufferBeanToTail(BufferBeanImpl* pBean)
		{
			Region* pRegion = new Region;
			pRegion->Interval = { m_pTail->Interval.Min, m_pTail->Interval.Min + pBean->uiBufferSize };
			pRegion->pBean = pBean;
			pBean->pData = pRegion;

			const Rangeui oldValue = m_pTail->Interval;
			Region* pLeft = m_pTail->pLeft;

			//				m_pTail = new Region;
			m_pTail->Interval.Min = pRegion->Interval.Max;
			m_pTail->Interval.Max = oldValue.Max;
			m_pTail->iIndex++;

			pRegion->pRight = m_pTail;
			pRegion->pLeft = pLeft;
			m_pTail->pLeft = pRegion;

			if (pLeft == nullptr)
			{
				assert(m_pHeader == nullptr);
				m_pHeader = pRegion;
				pRegion->iIndex = 0;
			}
			else
			{
				pLeft->pRight = pRegion;
				pRegion->iIndex = pLeft->iIndex + 1;
				assert(pRegion->iIndex == m_pTail->iIndex - 1);
			}
		}

		void BufferSharedNonUniformPool::deleteRegions()
		{
			if (m_pHeader)
			{
				Region* pNext = m_pHeader;
				while (pNext)
				{
					Region* pDel = pNext;
					pNext = pNext->pRight;

					delete pDel;
				}

				m_pHeader = nullptr;
			}
		}

		void BufferSharedNonUniformPool::removeRegionSpaces()
		{
			uint32_t uiTotalShift = 0;
			int32_t iIndexShift = 0;
			if (!m_uiConsumedSize)
			{
				return;
			}

			while (m_pHeader)
			{
				if (m_pHeader->pBean == nullptr)
				{
					uiTotalShift += m_pHeader->Interval.length();
					iIndexShift++;
					Region* pNext = m_pHeader->pRight;
					if (pNext)
					{
						pNext->pLeft = nullptr;
					}

					delete m_pHeader;
					m_pHeader = pNext;
				}
				else
				{
					break;
				}
			}

			if (!m_pHeader)
			{
				assert(false);  // Internal Error.
			}

			m_pHeader->Interval.Max -= uiTotalShift;
			m_pHeader->Interval.Min -= uiTotalShift;
			assert(m_pHeader->iIndex == iIndexShift);
			assert(m_pHeader->Interval.Min == 0);

			m_pHeader->iIndex = 0;
			Region* pCurrent = m_pHeader;

			while (pCurrent)
			{
				m_pTail = pCurrent;
				if (pCurrent->pBean)
				{
					pCurrent->Interval.Max -= uiTotalShift;
					pCurrent->Interval.Min -= uiTotalShift;
					pCurrent->iIndex -= iIndexShift;
					pCurrent = pCurrent->pRight;
				}
				else
				{
					uiTotalShift += pCurrent->Interval.length();
					// delete this node
					Region* pLeft = pCurrent->pLeft;
					Region* pRight = pCurrent->pRight;

					pLeft->pRight = pRight;
					if (pRight)
					{
						pRight->pLeft = pLeft;
					}

					delete pCurrent;
					pCurrent = pRight;
				}
			}
		}

		void BufferSharedNonUniformPool::mergeSapce(Region* pRegion, int32_t& iIndexShift)
		{
			Region* pStart = pRegion;
			if (pStart == nullptr || pStart->pBean)
				return;
			pRegion = pStart->pRight;
			while (pRegion)
			{
				if (pRegion->pBean == nullptr)
				{
					assert(pStart->Interval.Max == pRegion->Interval.Min);
					// combine the interval
					pStart->Interval.Max = pRegion->Interval.Max;
					Region* pNext = pRegion->pRight;
					pStart->pRight = pNext;
					if (pNext)
					{
						pNext->pLeft = pStart;
					}

					delete pRegion;
					pRegion = pNext;
					iIndexShift++;
				}
				else
				{
					break;
				}
			}
		}

		void BufferSharedNonUniformPool::flushRegions()
		{
			uint32_t uiLastOffset = 0;
			int32_t iIndexShift = 0;
			Region* pCurrent = m_pHeader;
			Region* new_pHeader = nullptr;

			if (m_uiConsumedSize == 0 /*|| m_uiEmptyRegionCount == 0*/)
			{
				// no data
				return;
			}

			// TODO: 没有利用到高性能的glCopyBufferSubData函数

			while (pCurrent)
			{
				pCurrent->iIndex -= iIndexShift;
				m_pTail = pCurrent;

				if (pCurrent->pBean)
				{
					m_pProxyBuffer->update(uiLastOffset, pCurrent->pBean->uiBufferSize, (uint8_t*)pCurrent->pBean->pBufferData);
					uiLastOffset += pCurrent->pBean->uiBufferSize;
					pCurrent = pCurrent->pRight;

					if (new_pHeader == nullptr)
					{
						new_pHeader = pCurrent;
					}
				}
				else
				{
					// removed the empty regions
					Region* pLeft = pCurrent->pLeft;
					// merge the spaces
					mergeSapce(pCurrent, iIndexShift);

					// switch the current and right
					Region* pRight = pCurrent->pRight;
					if (pRight == nullptr)
					{
						break;
					}

					Region* pRR = pRight->pRight;
					pCurrent->pRight = pRR;
					if (pRR)
					{
						pRR->pLeft = pCurrent;
					}

					pRight->pRight = pCurrent;
					pRight->pLeft = pLeft;
					if (pLeft)
					{
						pLeft->pRight = pRight;
					}

					pCurrent->pLeft = pRight;

					// shift the intervals. 
					uint32_t uiCurrentLength = pCurrent->Interval.length();
					uint32_t uiRightLength = pRight->Interval.length();
					pCurrent->Interval.Min += uiRightLength;
					pCurrent->Interval.Max += uiRightLength;
					pRight->Interval.Min -= uiCurrentLength;
					pRight->Interval.Max -= uiCurrentLength;

					// index
					int32_t iOldIndex = pCurrent->iIndex;
					pCurrent->iIndex = pRight->iIndex;
					pRight->iIndex = iOldIndex;

					pCurrent = pRight;
				}
			}

			if (new_pHeader)
			{
				m_pHeader = new_pHeader;
			}

//			m_uiEmptyRegionCount = 0;
			m_pTail->Interval.Max = m_pProxyBuffer->getSize();
		}
	}
}