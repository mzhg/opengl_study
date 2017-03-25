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
			PPDownsamplePS(DownsampleMethod Method, uint32_t SampleCount = 1u)
			{
				Macro methodDeclar[] = 
				{
					{ "METHOD", Numeric::toString(static_cast<int>(Method)) },
					{ "TEXEL_FACTOR", Numeric::toString(static_cast<int>(SampleCount)) }
				};

				GLSLProgram::createShaderFromFile<GL_FRAGMENT_SHADER>("DownsamplePS.frag", this, false, 2, methodDeclar);
			}
		};

		class PostProcessingDownsample : public PPRenderPass
		{
		public:
			PostProcessingDownsample(DownsampleMethod method = DownsampleMethod::NORMAL, uint32_t sampleCount = 1u, uint32_t offset = 0, uint32_t width = 0, uint32_t height = 0);
			~PostProcessingDownsample();

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters);

			virtual void drawQuad(){}

			virtual void computeOutDesc(int index, Texture2DDesc& out)
			{
				Texture2D* input = getInput(0);
				out = input->getDesc();

				int downsampleScale = (1 << SampleCount);
				out.Width = Numeric::divideAndRoundUp(out.Width, downsampleScale);
				out.Height = Numeric::divideAndRoundUp(out.Height, downsampleScale);

				if (Width && Height)
				{
					out.Width = Width;
					out.Height = Height;
				}
			}

			const DownsampleMethod Method;
			const uint32_t SampleCount;
			static void shutDown();
		};
	}
}


