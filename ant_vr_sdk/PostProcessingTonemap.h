#pragma once

#include "PostProcessingFramework.h"
namespace jet
{
	namespace util
	{
		class PostProcessingTonemap : public PPRenderPass
		{
		public:
			PostProcessingTonemap(GLuint width = 0, GLuint height = 0);
			~PostProcessingTonemap();

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
		};
	}
}

