#pragma once

#include "GLUtil.h"
#include "GLStates.h"
#include "Buffer.h"

namespace jet
{
	namespace util
	{

		class BufferGPU : public Buffer, public Disposeable
		{
		public:
			BufferGPU() : m_Buffer(0), m_MappedPointer(nullptr), m_MappedOffset(0), m_MappedSize(0){}
			virtual ~BufferGPU() { dispose(); }

			virtual bool init(uint32_t size, const uint8_t* pData = nullptr) = 0;
			virtual bool update(uint32_t offset, uint32_t size, const uint8_t* pData) override;
			virtual uint8_t* map(uint32_t offset, uint32_t length, MappingBits bits = MappingBits::READ_WRITE) override;
			virtual void unmap() override;

			void dispose() override;

			virtual void memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size) override;

			virtual void bind();
			virtual void unbind();
			virtual BufferTarget getTarget() const = 0;
		protected:
			virtual GLenum getGLTarget() const = 0;

		protected:
			// The OpenGL buffer object ID
			GLuint m_Buffer;
			uint8_t* m_MappedPointer;
			uint32_t m_MappedOffset;
			uint32_t m_MappedSize;
		};

		class BufferGPUSubRange : public BufferGPU
		{
		public:
			BufferGPUSubRange(BufferGPU* pBuffer, uint32_t offset);
			bool init(uint32_t size, const uint8_t* pData = nullptr) override;
			bool update(uint32_t offset, uint32_t size, const uint8_t* pData) override;
			uint8_t* map(uint32_t offset, uint32_t length, MappingBits bits = MappingBits::READ_WRITE) override;

			void unmap() override;

			void memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size) override;

			void setOffset(uint32_t uiNewOffset) { m_BuffSubRange.setOffset(uiNewOffset); }
			uint32_t getDynamicOffset() override { return m_BuffSubRange.getDynamicOffset(); }

			void bind() override;
			void unbind() override;
			BufferTarget getTarget() const override;
		protected:
			GLenum getGLTarget() const override;

		protected:
			BufferSubRange m_BuffSubRange;
			BufferGPU* m_pBuffer;
		};

		class BufferGPUSharedPool : public BufferGPU
		{
		public:
			BufferGPUSharedPool(BufferCreator* pBufCreator, uint32_t capacity = 0) : m_Pool(createPool(pBufCreator, capacity)){
				m_pProxyBuffer = dynamic_cast<BufferGPU*>(pBufCreator->create());
				assert(m_pProxyBuffer);
			}
			BufferGPUSharedPool(BufferGPU* pBuf) : m_Pool(createPool(pBuf)), m_pProxyBuffer(pBuf){ assert(m_pProxyBuffer); }

			virtual bool init(uint32_t size, const uint8_t* pData = nullptr) override;
			virtual bool update(uint32_t offset, uint32_t size, const uint8_t* pData) override;
			virtual uint8_t* map(uint32_t offset, uint32_t length, MappingBits bits = MappingBits::READ_WRITE) override;
			virtual void unmap() override;
			virtual void memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size) override;

			virtual void bind();
			virtual void unbind();
			BufferTarget getTarget() const override;

			virtual ~BufferGPUSharedPool()
			{
				if (m_Pool)
				{
					delete m_Pool;
				}
			}
		protected:
			GLenum getGLTarget() const override;

			virtual BufferSharedPool* createPool(BufferCreator* pBufCreator, uint32_t) const;
			virtual BufferSharedPool* createPool(BufferGPU* pBuf) const;

		protected:
			BufferSharedPool* m_Pool;
			BufferGPU*        m_pProxyBuffer;
		};

		class BufferGPUSharedUniformPool : public BufferGPUSharedPool
		{
		public:

			virtual ~BufferGPUSharedUniformPool()
			{
				BufferGPUSharedPool::~BufferGPUSharedPool();
			}

			BufferGPUSharedUniformPool(BufferCreator* pBufCreator);
			BufferGPUSharedUniformPool(BufferGPU* pBuf);

			/// Initializes the large, shared VBO wrapper and sets its size
			/// \param dataSize Size, in bytes, of each sub-range that will be
			///                 contained withing the buffer object
			/// \param numSubRanges Number of sub-ranges that will be contained 
			///                     within the shared VBO
			/// \param numFrames Number of frames worth of data contained in the VBO, which will
			///                  act as a ring buffer, using the next set of sub-ranges for each frame
			/// \param bPersistent Flag to indicate whether the VBO should be persistently mapped
			/// \return True if the buffer was initialized, false if an error occurred
			bool init(uint32_t dataSize, uint32_t numSubRanges, uint32_t numFrames);

			/// Sets the current number of sub-ranges to contain in the shared VBO
			/// \param numSubRanges Number of sub-ranges that will be held within the VBO
			/// \note Must not be called between calls to BeginUpdate()/EndUpdate().
			void setNumSubRanges(uint32_t numSubRanges);

			/// Sets the size of each sub-range contained in the shared VBO
			/// \param subRangeSize Size, in bytes, of each sub-range that will be held within the VBO
			/// \note Must not be called between calls to BeginUpdate()/EndUpdate().
			void setSubRangeSize(uint32_t subRangeSize);

			/// Sets the number of frames worth of data contained in the shared VBO
			/// \param numFrames number of frames worth of data that will be held within the VBO
			/// \note Must not be called between calls to BeginUpdate()/EndUpdate().
			void setNumFrames(uint32_t numFrames);

			/// Returns the pointer to the area within the active buffer in 
			/// the shared VBO that corresponds to the given sub-range
			/// \param subRangeIndex Index of the sub-range for which to return a pointer
			/// \return Pointer to the current writeable range of the vertex buffer
			uint8_t* map(uint32_t subRangeIndex, uint32_t subIndex);

			/// Returns the offset into the shared VBO of the sub-range with the 
			/// provided index
			/// \param subRangeIndex Index of the sub-range for which to retrieve the offset
			/// \return Offset, in bytes, from the start of the vertex buffer at which the
			///         sub-range with the given index begins.
			uint32_t getDynamicOffset(int subRangeIndex, uint32_t subIndex);

		protected:
			BufferSharedPool* createPool(BufferCreator* pBufCreator, uint32_t) const override;
			BufferSharedPool* createPool(BufferGPU* pBuf) const override;

		private:
			bool initBuffers();

		private:
			BufferSharedUniformPool* m_SharedUniformPool;
		};

		class BufferGPUSharedNonUniformPool : public BufferGPUSharedPool
		{
		public:
			BufferGPUSharedNonUniformPool(BufferCreator* pBufCreator, uint32_t);
			BufferGPUSharedNonUniformPool(BufferGPU* pBuf);
			virtual ~BufferGPUSharedNonUniformPool()
			{
				BufferGPUSharedPool::~BufferGPUSharedPool();
			}

			BufferBean add(uint32_t size, const uint8_t* pData);
			bool update(BufferBean bean, uint32_t offset, uint32_t size, const uint8_t* pData);
			void remove(BufferBean bean);
			uint32_t getOffset(BufferBean bean) const;
			uint8_t* map(BufferBean bean, MappingBits bits = MappingBits::READ_WRITE);
			bool isDirty(BufferBean bean) const;
		protected:
			BufferSharedPool* createPool(BufferCreator* pBufCreator, uint32_t) const override;
			BufferSharedPool* createPool(BufferGPU* pBuf) const override;

		private:
			BufferSharedNonUniformPool* m_SharedNonUniformPool;
		};

		template<BufferTarget Target, BufferUsage Usage, GLenum MapBits>
		class BufferGL : public BufferGPU
		{
#define __TARGET ConvertBufferTargetToGLenum(Target)
#define __USAGE  ConvertBufferUsageToGLenum(Usage)
#define __PERSISTENT (MapBits & GL_MAP_PERSISTENT_BIT)
		public:
			BufferGL() : BufferGPU(){}

//			void load(GLsizei size, const GLubyte* pData = nullptr, BufferUsage usage = BufferUsage::STATIC_DRAW)
			virtual bool init(uint32_t size, const uint8_t* pData = nullptr) override
			{
				assert(size);
				bind();
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}
#endif
				CHECK_GL(glBufferData(__TARGET, size, pData, __USAGE));
				m_uiDataSize = size;
				return true;
			}

			uint8_t* map(uint32_t offset, uint32_t length, MappingBits bits = MappingBits::READ_WRITE) override
			{
#if __PERSISTENT
				if(m_MappedPointer == nullptr)
				{
					CHECK_GL(m_MappedPointer = glMapBufferRange(__TARGET, 0, m_uiDataSize, bits));
					if (m_MappedPointer)
					{
						m_MappedOffset = 0;
						m_MappedSize = m_uiDataSize;
						return m_MappedPointer + offset;
					}
				}
#else
				if (m_MappedPointer && m_MappedOffset == offset && m_MappedSize == length)
				{
					return m_MappedPointer;
				}
				else if (m_MappedPointer)
				{
					unmap();
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
				CHECK_GL(GLvoid* p = glMapBufferRange(getGLTarget(), offset, length, MapBits));

				m_MappedPointer = reinterpret_cast<uint8_t*>(p);
				m_MappedOffset = offset;
				m_MappedSize = length;
				return m_MappedPointer;
#endif
			}

			void unmap() override
			{
#if __PERSISTENT
				// Nothing need to do.
#else
				BufferGPU::unmap();
#endif
			}

			BufferTarget getTarget() const override
			{
				return Target;
			}

			GLenum getGLTarget() const override
			{
				return __TARGET;
			}

			BufferUsage getUsage() const { return Usage; }

//			static const GLenum __TARGET = 0; // ConvertBufferTargetToGLenum(Target);
#undef __TARGET
#undef __USAGE
#undef __PERSISTENT

			private:
//				const bool m_bPersistent;
		};

		template<BufferTarget Target, GLenum StorageBits, GLenum MapBits>
		class BufferGLStorage : public BufferGL<Target, BufferUsage::STATIC_DRAW, MapBits>
		{
		public:
			bool init(uint32_t size, const uint8_t* pData = nullptr) override
			{
				assert(size);
				bind();
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}
#endif
				CHECK_GL(glBufferStorage(getGLTarget(), size, pData, flags));
				m_uiDataSize = size;

				return true;
			}
		};

		
		template<BufferUsage Usage, GLenum MapBits>
		class ArrayBufferGL : public BufferGL < BufferTarget::ARRAY, Usage, MapBits > {};

		template<BufferUsage Usage, GLenum MapBits>
		class ElementBufferGL : public BufferGL < BufferTarget::ELEMENT, Usage, MapBits > {};

		template<BufferUsage Usage, GLenum MapBits>
		class UniformBufferGL : public BufferGL < BufferTarget::UNIFORM, Usage, MapBits > {};
		
//		typedef BufferGL<BufferTarget::ELEMENT> ElementBufferGL;
//		typedef BufferGL<BufferTarget::UNIFORM> UniformBufferGL;

		typedef struct BufferData
		{
			BufferGPU* ArrayBuffer;
			BufferGPU* ElementBuffer;
			AttribDesc* AttribDescs;
			unsigned AttribCount;

			BufferData() :
				ArrayBuffer(nullptr),
				AttribDescs(nullptr),
				AttribCount(0),
				ElementBuffer(nullptr)
			{}

			BufferData(BufferGPU* pArray, unsigned count, AttribDesc* pDesc, BufferGPU* pElement) :
				ArrayBuffer(pArray), AttribDescs(pDesc), AttribCount(count), ElementBuffer(pElement){}

		}BufferData;

		class VertexArrayGL : public Disposeable
		{
		public:
			VertexArrayGL() :m_VAO(0),
				m_BufferData()
			{} 

			void bind();

			void load(const BufferData* pArray);

//			void draw(unsigned count, GLint first = 0, unsigned numInstance = 1);

			void unbind();
			void dispose() override;
			~VertexArrayGL();

		private:
			GLuint m_VAO;

			BufferData m_BufferData;
		};

#if 0
		typedef struct BufferBean
		{
		public:
			uint32_t uiBufferSize;
			const unsigned char* pBufferData;

			BufferBean() :
				pData(nullptr){}

			BufferBean(uint32_t bufferSize, const unsigned char* pData) :
				uiBufferSize(bufferSize), pBufferData(pData), pData(nullptr){}
		private:
			void* pData;

			friend class BufferBatching;
		}BufferBean;

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

		class BufferBatching
		{
		private:
			struct Region
			{
				Rangeui Interval;
				Region* pLeft;
				Region* pRight;
				int32_t iIndex;
				const BufferBean* pBean;

				Region() :
					Interval(),
					pLeft(nullptr),
					pRight(nullptr),
					pBean(false),
					iIndex(-1)
				{}
			};
		public:
			BufferBatching(ArrayBufferGL* pArray) :
				m_pBuffer(pArray), m_pHeader(nullptr), m_pTail(nullptr),  m_uiConsumedSize(0),
				m_uiRegionCount(0), m_uiEmptyRegionCount(0)
			{
				if (pArray)
				{
					m_pTail = new Region;
					m_pTail->Interval = { 0, pArray->getSize() };
					m_pTail->iIndex++;
				}
			
			}

			BatchAddingBehavior add(BufferBean* pBean, BatchAddingBehavior behavior = BatchAddingBehavior::DEFAULT, uint32_t slot = 0)
			{
				assert(m_pBuffer);

				BatchAddingBehavior result = BatchAddingBehavior::APPEND;

				if (pBean && pBean->pData == nullptr)
				{
					assert(m_pTail);
					assert(m_pBuffer->getSize() > m_uiConsumedSize);

					const uint32_t uiRemainCapacity = m_pBuffer->getSize() - m_uiConsumedSize;
					bool bReallocated = false;
					if (uiRemainCapacity < pBean->uiBufferSize)
					{
						m_pBuffer->bind();

						uint32_t bufferSize = m_pBuffer->getSize();
						uint32_t deserdSize = m_uiConsumedSize + pBean->uiBufferSize;
						while (bufferSize < deserdSize)
							bufferSize *= 2;

						m_pBuffer->load(bufferSize, nullptr, m_pBuffer->getUsage());
						bReallocated = true;
					}

					if (!bReallocated && (behavior == BatchAddingBehavior::APPEND_COMPACT /*|| behavior == BatchAddingBehavior::INSERT_COMPACT*/))
					{
						bReallocated = true;
					}

					if (bReallocated || m_pTail->Interval.length() < pBean->uiBufferSize)
					{
						result = BatchAddingBehavior::APPEND_COMPACT;
						flushRegions();  // TODO Can be optimise.
					}

					assert(m_pTail->pBean == nullptr && m_pTail->Interval.length() >= pBean->uiBufferSize);
					m_pBuffer->update(m_pTail->Interval.Min, pBean->uiBufferSize, pBean->pBufferData);
					m_uiConsumedSize += pBean->uiBufferSize;
					addBufferBeanToTail(pBean);

					// TODO 目前未实现的功能：
					// 1，中间空白区域的插入功能
					// 2，Element BaseVertex的功能 
				}

				return result;
			}

			void remove(BufferBean* pBean, BatchRemoveBehavior behavior = BatchRemoveBehavior::DEFAULT)
			{
				assert(m_pBuffer);
				if (pBean && pBean->pData)
				{
					Region* pRegion = reinterpret_cast<Region*>(pBean->pData);
					pRegion->pBean = nullptr;
					m_uiConsumedSize -= pRegion->Interval.length();

					// merge the sapce
					m_uiEmptyRegionCount++;

					// TODO 目前未实现的功能：
					// 1，删除后空白区间的合并功能，当添加空白区域的插入功能时需要实现
					// 2，Element/Instance Attrib删除后，要么移除空白区间，要么填充预先设置的无效数据。
					// 3，如果没有Element数据,则只能移除空白区间
				}
			}

		private:

			void addBufferBeanToTail(BufferBean* pBean)
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

			void deleteRegions()
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

			void removeRegionSpaces()
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

			void mergeSapce(Region* pRegion, int32_t& iIndexShift)
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

			void flushRegions()
			{
				uint32_t uiLastOffset = 0;
				int32_t iIndexShift = 0;
				Region* pCurrent = m_pHeader;
				Region* new_pHeader = nullptr;

				if (m_uiConsumedSize == 0 || m_uiEmptyRegionCount == 0)
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
						m_pBuffer->update(uiLastOffset, pCurrent->pBean->uiBufferSize, (GLubyte*)pCurrent->pBean->pBufferData);
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

				m_uiEmptyRegionCount = 0;
				m_pTail->Interval.Max = m_pBuffer->getSize();
			}
			
		private:
			ArrayBufferGL* m_pBuffer;

			Region* m_pHeader;
			Region* m_pTail;
			Region* m_FirstEmpty;

			uint32_t m_uiRegionCount;
			uint32_t m_uiEmptyRegionCount;
			uint32_t m_uiConsumedSize;
		};
#endif
	}
}

