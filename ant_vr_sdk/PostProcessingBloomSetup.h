#pragma once

#include "PostProcessingFramework.h"
namespace jet
{
	namespace util
	{

		class PostProcessingBloomSetup : public PPRenderPass
		{
		public:
			PostProcessingBloomSetup(uint32_t offset = 0);
			~PostProcessingBloomSetup();

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters) override;

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

