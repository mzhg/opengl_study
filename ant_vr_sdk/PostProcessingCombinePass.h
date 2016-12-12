#pragma once
#include "PostProcessingFramework.h"

namespace jet
{
	namespace util
	{
		class PostProcessingCombinePass : public PPRenderPass
		{
		public:
			PostProcessingCombinePass(uint32_t offset = 0);
			~PostProcessingCombinePass();

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters);

			virtual void drawQuad(){}

			virtual void computeOutDesc(int index, Texture2DDesc& out)
			{
				Texture2D* input0 = getInput(0);
				Texture2D* input1 = getInput(1);
				out = input0->getDesc();
				out.Width = Numeric::max(out.Width, input1->getWidth());
				out.Height = Numeric::max(out.Height, input1->getHeight());
			}

			static void shutDown();
		};
	}
}


