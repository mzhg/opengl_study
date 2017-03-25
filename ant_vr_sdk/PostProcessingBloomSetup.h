#pragma once

#include "PostProcessingFramework.h"
namespace jet
{
	namespace util
	{
		class PostProcessingBloomSetup : public PPRenderPass
		{
		public:
			PostProcessingBloomSetup(uint32_t offset = 0, bool useLight = false, uint32_t width = 0, uint32_t height = 0);
			~PostProcessingBloomSetup();

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters) override;

			virtual void drawQuad(){}

			virtual void computeOutDesc(int index, Texture2DDesc& out)
			{
				Texture2D* input = getInput(0);
				out = input->getDesc();

				if (Width && Height)
				{
					out.Width = Width;
					out.Height = Height;
				}
			}

			const bool UseLightParams;
			static void shutDown();
		};
	}
}

