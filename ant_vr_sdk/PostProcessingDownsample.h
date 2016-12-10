#pragma once
#include "PostProcessingFramework.h"
namespace jet
{
	namespace util
	{
		class PostProcessingDownsample : public PPRenderPass
		{
		public:
			PostProcessingDownsample(PassName name);
			~PostProcessingDownsample();
		};
	}
}


