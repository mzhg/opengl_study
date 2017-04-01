#pragma once

#include "BufferGL.h"
#include "Shape3D.h"
#include <unordered_map>

namespace jet
{
	namespace util
	{
		class GeometryAttribBuffer;

		typedef struct GeometryAttribOffset
		{
			uint32_t AttribCount;
			uint32_t* AttribOffset;
		}GeometryAttribOffset;

		typedef struct GeometryBufferOffset
		{
			// The count of the array buffer.
			uint32_t BufferCount;
			// The attrib offsets for each array buffer.
			GeometryAttribOffset* BufferOffsets;
			BufferBean* BufferBeans;
			uint32_t ElementOffset;
			BufferBean ElementBean;

			GeometryBufferOffset(uint32_t bufferCount = 0, GeometryAttribOffset* bufferOffsets = nullptr, BufferBean* bufferBeans = nullptr, 
				uint32_t elementOffset = 0xffffffff,
				BufferBean elementBean = nullptr) :
				BufferCount(bufferCount), BufferOffsets(bufferOffsets), BufferBeans(bufferBeans), ElementOffset(elementOffset), ElementBean(elementBean)
			{}
#if 0
			GeometryBufferOffset(const GeometryAttribOffset& o)
			{
				BufferCount = o.BufferCount;
				ElementOffset = o.ElementOffset;
				ElementBean = o.ElementBean;

				if (BufferCount)
				{
					BufferOffsets = new GeometryAttribOffset[BufferCount];
					assert(BufferOffsets);
					memcpy(BufferOffsets, o.BufferOffsets, BufferCount * sizeof(GeometryAttribOffset));

					BufferBeans = new BufferBean[BufferCount];
					memcpy(BufferBeans, o.BufferBeans, BufferCount * sizeof(BufferBean));
				}
			}

			GeometryAttribOffset& operator=(const GeometryAttribOffset& o)
			{
				AttribCount = o.AttribCount;
				ElementOffset = o.ElementOffset;
				ElementBean = o.ElementBean;

				if (AttribCount)
				{
					AttribOffset = new uint32_t[AttribCount];
					assert(AttribOffset);
					memcpy(AttribOffset, o.AttribOffset, AttribCount * sizeof(uint32_t));

					AttribBeans = new BufferBean[AttribCount];
					memcpy(AttribBeans, o.AttribBeans, AttribCount * sizeof(BufferBean));
				}
				return *this;
			}

			~GeometryBufferOffset()
			{
				if (AttribOffset)
				{
					delete[] AttribOffset;
					delete[] AttribBeans;
				}
			}
#endif
		}GeometryBufferOffset;

		class GeometryAttribBuffer
		{
		public:

			static const uint32_t INVALID_OFFSET = 0xffffffff;

			// hasIndices: wheather have indices in this buffer.
			// count: The number of the attribs, attrib means POSTION2, POSITION3 or P3TN2.
			GeometryAttribBuffer(bool hasIndices, uint32_t count, GeometryBufferDesc* attribs);

			bool addAttribDesc(const GeometryBufferDesc& attrib);
			bool removeAttribDesc(const GeometryBufferDesc& attrib);
			bool removeAttribDesc(uint32_t index);

			uint32_t getAttribCount() const { return static_cast<uint32_t>(m_AttribDescs.size()); }
			const GeometryBufferDesc& getAttribDesc(uint32_t index) const { return m_AttribDescs[index];  /*TODO: Not Safe! */ }

			bool contain(int32_t key) const;
			int32_t generateKey();

			bool add(int32_t key, const GeometryAttribData& data);
			bool remove(int32_t key);
			const GeometryBufferOffset& getBufferOffset(int32_t key) const;

			bool hasIndices() const { return m_bHaveIndices; }
			void enableIndices()  { setIndicesState(true); }
			void disableIndices() { setIndicesState(false); }

			void setMode(Shape3D::Mode mode) { m_Mode = mode; }
			Shape3D::Mode getMode() const { return m_Mode; }
			BufferGPUSharedNonUniformPool* getAttribBuffer(uint32_t index)
			{
				if (index >= m_AttribBuffersPool.size())
				{
					return nullptr;
				}
				else
				{
					return m_AttribBuffersPool[index];
				}
			}

			BufferGPUSharedNonUniformPool* getElementBuffer() { return m_ElementBufferPool; }

			virtual ~GeometryAttribBuffer();

		private:
			template<typename T>
			T* allocate(uint32_t size)
			{
				void* pData = malloc(size);
				m_AllocatedData.push_back(pData);
				return reinterpret_cast<T*>(pData);
			}

			void setIndicesState(bool flag);

		private:
			bool m_bHaveIndices;
			std::vector<GeometryBufferDesc> m_AttribDescs;
			std::vector<BufferGPUSharedNonUniformPool*> m_AttribBuffersPool;
			std::vector<void*> m_AllocatedData;
			BufferGPUSharedNonUniformPool* m_ElementBufferPool;

			std::unordered_map<int32_t, GeometryBufferOffset> m_AttribOffsets;
			std::vector<int32_t> m_unusedKeys;

			Shape3D::Mode m_Mode;
		};
	}
}
