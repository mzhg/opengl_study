
#ifndef _JET_UTIL_POSTPROCESSING_GUASS_BLUR_H
#define _JET_UTIL_POSTPROCESSING_GUASS_BLUR_H

#include "PostProcessingFramework.h"

namespace jet
{
	namespace util
	{
		class PPGuassBlurPS : public PixelShader
		{
		public:
			PPGuassBlurPS(int kernalSize);

#if 0
			void setTextrueHalfSize(float texelX, float texelY);

		private:
			GLint m_HalfSizeIndex;
#endif

		};

		class PostProcessGaussBlur : public PPRenderPass
		{
		public:
			PostProcessGaussBlur(uint32_t index = 0, uint32_t width = 0, uint32_t height = 0) : PPRenderPass(GAUSS_BLUR + index, width, height)
			{
				set(1, 1);
			}

			void process(PPRenderContext* context, const PostProcessingParameters& parameters) override;

			virtual void drawQuad(){}

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

			~PostProcessGaussBlur();

			static PPGuassBlurPS* getGuassBlurPS(int kernelSize);

			static void shutDown();

		private:
//			static std::map<int, PPGuassBlurPS*>  g_GuassBlurPS;
		};

	}
}
#endif

