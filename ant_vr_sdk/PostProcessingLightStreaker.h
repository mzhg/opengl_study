#pragma once

#include "PostProcessingFramework.h"
namespace jet
{
	namespace util
	{
		class PostProcessingLightStreaker : public PPRenderPass
		{
		public:
			PostProcessingLightStreaker(uint32_t offset = 0, uint32_t width = 0, uint32_t height = 0);
			~PostProcessingLightStreaker();

			void process(PPRenderContext* context, const PostProcessingParameters& parameters) override;

			void computeOutDesc(int index, Texture2DDesc& out) override
			{
				Texture2D* input = getInput(index);
				if (!input)
					return;

				out = input->getDesc();

				if (Width && Height)
				{
					out.Width = Width;
					out.Height = Height;
				}
			}

			static void shutDown();

		private:
			void genHorizontalGlare(PPRenderContext* context, RenderTarget* output, int dir);
			void genStarStreak(PPRenderContext* context, RenderTarget* output, int dir, float ratio);
		};
	}
}

