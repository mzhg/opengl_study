#pragma once

#include "BufferGL.h"
#include "Shape3D.h"
#include <unordered_map>

namespace jet
{
	namespace util
	{
		typedef struct GeometryAttribData
		{
			uint32_t AttribCount;
			uint32_t* AttribSize;
			uint8_t** AttribData;
			DataType ElementDataType;
			uint32_t ElementSize;
			uint8_t* ElementData;
		}GeometryAttribData;


		class GeometryAttribBuffer;
		typedef struct GeometryAttribOffset
		{
			uint32_t AttribCount;
			uint32_t* AttribOffset;
			BufferBean* AttribBeans;
			uint32_t ElementOffset;
			BufferBean ElementBean;

			GeometryAttribOffset(uint32_t attribCount = 0, uint32_t* attribOffset = nullptr, BufferBean* attribBeans = nullptr, uint32_t elementOffset = GeometryAttribBuffer::INVALID_OFFSET, BufferBean elementBean = nullptr):
				AttribCount(attribCount), AttribOffset(attribOffset), AttribBeans(attribBeans), ElementOffset(elementOffset), ElementBean(elementBean)
			{}
		
			GeometryAttribOffset(const GeometryAttribOffset& o)
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

			~GeometryAttribOffset()
			{
				if (AttribOffset)
				{
					delete[] AttribOffset;
					delete[] AttribBeans;
				}
			}
		}GeometryAttribOffset;

		class GeometryAttribBuffer
		{
		public:

			static const uint32_t INVALID_OFFSET = 0xffffffff;

			// hasIndices: wheather have indices in this buffer.
			// count: The number of the attribs, attrib means POSTION2, POSITION3 or P3TN2.
			GeometryAttribBuffer(bool hasIndices, uint32_t count, AttribDesc* attribs);

			bool addAttribDesc(const AttribDesc& attrib);
			bool removeAttribDesc(const AttribDesc& attrib);
			bool removeAttribDesc(uint32_t index);

			uint32_t getAttribCount() const { return static_cast<uint32_t>(m_AttribDescs.size()); }
			const AttribDesc& getAttribDesc(uint32_t index) const { return m_AttribDescs[index];  /*TODO: No Safe! */}

			bool contain(int32_t key) const;
			int32_t generateKey();

			bool add(int32_t key, const GeometryAttribData& data);
			bool remove(int32_t key);
			const GeometryAttribOffset& getAttribOffset(int32_t key) const;

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
			void setIndicesState(bool flag);

		private:
			bool m_bHaveIndices;
			std::vector<AttribDesc> m_AttribDescs;
			std::vector<BufferGPUSharedNonUniformPool*> m_AttribBuffersPool;
			BufferGPUSharedNonUniformPool* m_ElementBufferPool;

			std::unordered_map<int32_t, GeometryAttribOffset> m_AttribOffsets;
			std::vector<int32_t> m_unusedKeys;

			Shape3D::Mode m_Mode;
		};
	}
}
