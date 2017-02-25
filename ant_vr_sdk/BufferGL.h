#pragma once

#include "GLUtil.h"
#include "GLStates.h"
namespace jet
{
	namespace util
	{
		enum class MappingBits
		{
			READ = 1,
			WRITE = 2,
			READ_WRITE = READ | WRITE,
		};

		template<BufferTarget Target>
		class BufferGL : public Disposeable
		{
#define __TARGET ConvertBufferTargetToGLenum(Target)
		public:
			BufferGL() :
			m_Buffer(0),
			m_Size(0)
#if defined(_DEBUG)
			,
			m_Mapped(false)
#endif
			{}

			void bind()
			{
				if (!m_Buffer)
				{
					glGenBuffers(1, &m_Buffer);
				}

				GLStates::get().bindBuffer(m_Buffer, Target);
			}
			void load(GLsizei size, const GLubyte* pData = nullptr, BufferUsage usage = BufferUsage::STATIC_DRAW)
			{
				assert(size);
				m_Useage = usage;
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}
#endif
				CHECK_GL(glBufferData(__TARGET, size, pData, ConvertBufferUsageToGLenum(usage)));
			}

			void update(GLintptr offset, GLsizei size, const GLubyte* pData)
			{
				assert(size);
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}
#endif
				CHECK_GL(glBufferSubData(__TARGET, offset, size, pData));
			}

			template<typename T>
			T* map(GLintptr offset, GLsizei length, MappingBits bits = MappingBits::READ_WRITE)
			{
				GLbitfield bits;

				switch (bits)
				{
				case jet::util::READ:
					bits = GL_MAP_READ_BIT;
					break;
				case jet::util::WRITE:
					bits = GL_MAP_WRITE_BIT;
					break;
				case jet::util::READ_WRITE:
					bits = (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
					break;
				default:
					assert(false);
					break;
				}
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}

				m_Mapped = true;
#endif
				GLvoid* p = glMapBufferRange(__TARGET, offset, length, bits);
				CHECK_GL_ERROR;
				assert(p);
				return dynamic_cast<T*>(p);
			}

			void unmap(){ 
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}

				assert(m_Mapped);
				m_Mapped = false;
#endif
				glUnmapBuffer(__TARGET); 
			}

			void unbind()
			{
				GLStates& state = GLStates::get();
#if defined(_DEBUG)
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
				}
#endif
				state.resetBuffer(Target);
			}
			void dispose()
			{
				SAFE_RELEASE_BUFFER(m_Buffer);
			}

			virtual ~BufferGL()
			{
				dispose();
			}

			GLuint getSize() const 
			{
				return m_Size;
			}

			BufferTarget getTarget() const
			{
				return Target
			}

			BufferUsage getUsage() const { return m_Useage; }
		private:
			GLuint m_Buffer;
			GLuint m_Size;
			BufferUsage m_Useage;

#if defined(_DEBUG)
			bool  m_Mapped;
#endif

//			static const GLenum __TARGET = 0; // ConvertBufferTargetToGLenum(Target);
#undef __TARGET
		};

		
		typedef BufferGL<BufferTarget::ARRAY> ArrayBufferGL;
		typedef BufferGL<BufferTarget::ELEMENT> ElementBufferGL;
		typedef BufferGL<BufferTarget::UNIFORM> UniformBufferGL;

		typedef struct BufferData
		{
			ArrayBufferGL* ArrayBuffer;
			ElementBufferGL* ElementBuffer;
			AttribDesc* AttribDescs;
			unsigned AttribCount;

			BufferData() :
				ArrayBuffer(nullptr),
				AttribDescs(nullptr),
				AttribCount(0),
				ElementBuffer(nullptr)
			{}

			BufferData(ArrayBufferGL* pArray, unsigned count, AttribDesc* pDesc, ElementBufferGL* pElement) :
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
	}
}

