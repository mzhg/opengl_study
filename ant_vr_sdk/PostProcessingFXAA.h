#pragma once
#include "PostProcessingFramework.h"
namespace jet
{
	namespace util
	{
		class PostProcessingFXAA : public PPRenderPass
		{
		public:
			PostProcessingFXAA(uint32_t quality) :PPRenderPass(PP_FXAA), m_Quality(quality){ set(1, 1);}
			virtual ~PostProcessingFXAA() {}

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters);

			virtual void drawQuad(){}

			virtual void computeOutDesc(int index, Texture2DDesc& out)
			{
				Texture2D* input = getInput(0);
				out = input->getDesc();
			}

			static void shutDown();

			const uint32_t m_Quality;
		};
	}
}

