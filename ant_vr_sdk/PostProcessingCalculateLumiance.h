#pragma once

#include "PostProcessingFramework.h"

namespace jet
{
	namespace util
	{
		class PostProcessingCalculateLumiance : public PPRenderPass
		{
		public:
			PostProcessingCalculateLumiance(uint32_t offset = 0);
			~PostProcessingCalculateLumiance();

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters) override;

			void computeOutDesc(int index, Texture2DDesc& out) override
			{
				Texture2D* input = getInput(0);
				out = input->getDesc();
				out.Width = 1;
				out.Height = 1;
			}

			static void shutDown();

		private:
			// Record the last frame lumiance.
			Texture2D* m_pLumTex;
		};
	}
}

