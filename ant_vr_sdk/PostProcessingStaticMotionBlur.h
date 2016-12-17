#pragma once
#include "PostProcessingFramework.h"

namespace jet
{
	namespace util
	{
		class PostProcessingStaticMotionBlur : public PPRenderPass
		{
		public:
			// intputCount = 1: Color and Depth in a combined texture;
			// intputCount = 2: Color and Depth in sperate textures.
			PostProcessingStaticMotionBlur(uint32_t intputCount) : PPRenderPass(PP_STATIC_MOTION_BLUR)
			{
				set(intputCount, 1);
			}
			~PostProcessingStaticMotionBlur() {}

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters);

			virtual void drawQuad(){}

			virtual void computeOutDesc(int index, Texture2DDesc& out)
			{
				Texture2D* input = getInput(0);
				out = input->getDesc();
			}

			static void shutDown();
		};
	}
}

