#include "BatchBuffer.h"

namespace jet
{
	namespace util
	{
		extern BatchBuffer* CreateSingleBatchBuffer(SpatialManager* pManager);

		BatchBuffer* BatchBuffer::create(SpatialManager* pManager,BatchType type)
		{
			switch (type)
			{
			case BatchBuffer::SINGLE:
				return CreateSingleBatchBuffer(pManager);
				break;
			case jet::util::BatchBuffer::INSTANCE:
				break;
			case jet::util::BatchBuffer::DYNAMIC:
				break;
			case jet::util::BatchBuffer::STATIC:
				break;
			case jet::util::BatchBuffer::ELEMENT_COMBINE:
				break;
			case jet::util::BatchBuffer::MULTI_DRAW:
				break;
			default:
				break;
			}
		}
	}
}