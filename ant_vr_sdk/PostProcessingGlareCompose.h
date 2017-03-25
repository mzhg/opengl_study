#pragma once

#include "PostProcessingFramework.h"
namespace jet
{
	namespace util
	{
		class PostProcessingGlareCompose : public PPRenderPass
		{
		public:
			PostProcessingGlareCompose(glm::vec4 mixCoeff, uint32_t offset = 0, uint32_t width = 0, uint32_t height = 0);

			void process(PPRenderContext* context, const PostProcessingParameters& parameters) override;

			void computeOutDesc(int index, Texture2DDesc& out) override
			{
				Texture2D* input = getInput(0);
				out = input->getDesc();

				if (Width && Height)
				{
					out.Width = Width;
					out.Height = Height;
				}
			}

			const glm::vec4 MixCoeff;
			static void shutDown();

			~PostProcessingGlareCompose();
		};
	}
}

