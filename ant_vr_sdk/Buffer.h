
#include <stdint.h>
#include <assert.h>
#include "geometry2d.h"

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

		class Buffer
		{
		public:
			Buffer() :m_uiDataSize(0){}
			virtual ~Buffer(){}
			virtual bool init(uint32_t size, const uint8_t* pData = nullptr) = 0;
			virtual bool update(uint32_t offset, uint32_t size, const uint8_t* pData) = 0;
			virtual uint32_t getSize() const { return m_uiDataSize; }
			virtual uint8_t* map(uint32_t offset, uint32_t length, MappingBits bits = MappingBits::READ_WRITE) = 0;
			virtual uint32_t getDynamicOffset() { return 0; }
			virtual void unmap() = 0;

			virtual void memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size) = 0;

		protected:
			uint32_t m_uiDataSize;
		};

		class BufferSubRange : public Buffer
		{
		public:
			BufferSubRange(Buffer* pBuffer, uint32_t offset);
			bool init(uint32_t size, const uint8_t* pData = nullptr) override;
			bool update(uint32_t offset, uint32_t size, const uint8_t* pData) override;
			uint8_t* map(uint32_t offset, uint32_t length, MappingBits bits = MappingBits::READ_WRITE) override;
			
			void unmap() override;

			void memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size) override;

			void setOffset(uint32_t uiNewOffset) { m_uiOffset = uiNewOffset; }
			uint32_t getDynamicOffset() override { return m_uiOffset; }
		protected:
			Buffer* m_pBuffer;
			uint32_t m_uiOffset;
		};

		class BufferMemory : public Buffer
		{
		public:
			BufferMemory();
			virtual ~BufferMemory();
			virtual bool init(uint32_t size, const uint8_t* pData = nullptr) override;
			virtual bool update(uint32_t offset, uint32_t size, const uint8_t* pData) override;
			virtual uint8_t* map(uint32_t offset, uint32_t length, MappingBits bits = MappingBits::READ_WRITE) override;
			virtual void unmap() override;
			virtual void memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size) override;

		protected:
			uint8_t* m_pData;
		};

		class BufferCreator
		{
		public:
			virtual Buffer* create() = 0;
			virtual void release(Buffer*) = 0;
		};

		class BufferSharedPool : public Buffer
		{
		public:
			BufferSharedPool(BufferCreator* pBufCreator) : Buffer(), m_pBufferCreator(pBufCreator){
				m_pProxyBuffer = pBufCreator->create();
				assert(m_pProxyBuffer);
			}
			BufferSharedPool(Buffer* pBuf) : m_pProxyBuffer(pBuf), m_pBufferCreator(nullptr){ assert(m_pProxyBuffer); }

			virtual bool init(uint32_t size, const uint8_t* pData = nullptr) override;
			virtual bool update(uint32_t offset, uint32_t size, const uint8_t* pData) override;
			virtual uint8_t* map(uint32_t offset, uint32_t length, MappingBits bits = MappingBits::READ_WRITE) override;
			virtual void unmap() override;
			virtual void memoryCpy(uint32_t dst_offset, uint32_t src_offset, uint32_t size) override;

			Buffer* getProxyBuffer() { return m_pProxyBuffer; }

			virtual ~BufferSharedPool()
			{
				if (m_pBufferCreator)
				{
					m_pBufferCreator->release(m_pProxyBuffer);
				}
			}

		protected:
			BufferCreator* m_pBufferCreator;
			Buffer*        m_pProxyBuffer;
		};

		class BufferSharedUniformPool : public BufferSharedPool
		{
		public:
			
			virtual ~BufferSharedUniformPool()
			{
				BufferSharedPool::~BufferSharedPool();
			}

			BufferSharedUniformPool(BufferCreator* pBufCreator);
			BufferSharedUniformPool(Buffer* pBuf);

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

		private:
			bool initBuffers();

		private:
			uint32_t    m_numSubRanges;
			uint32_t    m_subRangeSize;
			uint32_t    m_index;
			uint32_t    m_numFrames;
			uint32_t    m_bufferSize;
		};

		typedef void* BufferBean;
		class BufferSharedNonUniformPool : public BufferSharedPool
		{
		public:
			BufferSharedNonUniformPool(BufferCreator* pBufCreator, uint32_t);
			BufferSharedNonUniformPool(Buffer* pBuf);
			virtual ~BufferSharedNonUniformPool()
			{
				BufferSharedPool::~BufferSharedPool();
				deleteRegions();
			}

			BufferBean add(uint32_t size, const uint8_t* pData);
			bool update(BufferBean bean, uint32_t offset, uint32_t size, const uint8_t* pData);
			void remove(BufferBean bean);
			uint32_t getOffset(BufferBean bean) const;
			uint8_t* map(BufferBean bean, MappingBits bits = MappingBits::READ_WRITE);
			bool isDirty(BufferBean bean) const;

		private:
			struct BufferBeanImpl;
			struct Region
			{
				Rangeui Interval;
				Region* pLeft;
				Region* pRight;
				int32_t iIndex;
				bool bDirty;
				BufferBeanImpl* pBean;

				Region() :
					Interval(),
					pLeft(nullptr),
					pRight(nullptr),
					pBean(false),
					bDirty(false),
					iIndex(-1)
				{}
			};

			struct BufferBeanImpl
			{
			public:
				uint32_t uiBufferSize;
				const unsigned char* pBufferData;

				BufferBeanImpl() :
					pData(nullptr){}

				BufferBeanImpl(uint32_t bufferSize, const unsigned char* pData) :
					uiBufferSize(bufferSize), pBufferData(pData), pData(nullptr){}
				Region* pData;
			};

		private:
			void addBufferBeanToTail(BufferBeanImpl* bean);
			void deleteRegions();
			void removeRegionSpaces();
			void mergeSapce(Region* pRegion, int32_t& iIndexShift);
			void flushRegions();
			void makeRegionsDirty();

		private:
			Region* m_pHeader;
			Region* m_pTail;
//			Region* m_FirstEmpty;

			uint32_t m_uiRegionCount;
//			uint32_t m_uiEmptyRegionCount;
			uint32_t m_uiConsumedSize;
		};
	}
}