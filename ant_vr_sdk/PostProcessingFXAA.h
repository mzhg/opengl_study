#pragma once
#include "PostProcessingFramework.h"
namespace jet
{
	namespace util
	{
		class PostProcessingFXAA : public PPRenderPass
		{
		public:
			PostProcessingFXAA(uint32_t quality, uint32_t width = 0, uint32_t height = 0) :PPRenderPass(PP_FXAA, width, height), m_Quality(quality){ set(1, 1); }
			virtual ~PostProcessingFXAA() {}

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters);

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

			static void shutDown();

			const uint32_t m_Quality;
		};
	}
}

