#pragma once
#include "PostProcessingFramework.h"
namespace jet
{
	namespace util
	{

		
//		template<DownsampleMethod Method>
		class PPDownsamplePS : public PixelShader
		{
		public:
			PPDownsamplePS(DownsampleMethod Method)
			{
				Macro methodDeclar = { "METHOD", Numeric::toString(static_cast<int>(Method)) };
				GLSLProgram::createShaderFromFile<GL_FRAGMENT_SHADER>("DownsamplePS.frag", this, false, 1, &methodDeclar);
			}
		};

		class PostProcessingDownsample : public PPRenderPass
		{
		public:
			PostProcessingDownsample(DownsampleMethod method, uint32_t offset = 0);
			~PostProcessingDownsample();

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters);

			virtual void drawQuad(){}

			virtual void computeOutDesc(int index, Texture2DDesc& out)
			{
				Texture2D* input = getInput(0);
				out = input->getDesc();
				out.Width = Numeric::divideAndRoundUp(out.Width, 2);
				out.Height = Numeric::divideAndRoundUp(out.Height, 2);
			}

			const DownsampleMethod Method;
			static void shutDown();
		};
	}
}


