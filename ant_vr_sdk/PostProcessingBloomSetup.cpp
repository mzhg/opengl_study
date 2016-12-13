#include "PostProcessingBloomSetup.h"

namespace jet
{
	namespace util
	{
		static PixelShader* g_BloomSetupPS = NULL;

		static PixelShader* getBloomSetupPS()
		{
			if (g_BloomSetupPS == NULL)
			{
				g_BloomSetupPS = new PixelShader();
				GLSLProgram::createShaderFromFile("BloomSetupPS.frag", g_BloomSetupPS);
			}

			return g_BloomSetupPS;
		}

		void PostProcessingBloomSetup::shutDown()
		{
			if (g_BloomSetupPS != NULL)
			{
				delete g_BloomSetupPS;
				g_BloomSetupPS = NULL;
			}
		}

		PostProcessingBloomSetup::PostProcessingBloomSetup(uint32_t offset) : 
			PPRenderPass(BLOOM_SETUP + offset)
		{
			set(1, 1);
		}

		void PostProcessingBloomSetup::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			Texture2D* inputTexture = getInput(0);
			RenderTarget* outputTexture = getOutputTexture(0);
			PixelShader* pShaderPixels = getBloomSetupPS();

			context->begin();
			{
				context->setShader(pShaderPixels);
				context->setUniform1i("g_Texture", 0);
				CHECK_GL_ERROR
				{
					context->setUniform2f("f2BloomThreshold", parameters.getBloomThreshold(), parameters.getExposureScale());
					CHECK_GL_ERROR
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(inputTexture->getTarget(), inputTexture->getTexture());
					CHECK_GL_ERROR
					const RenderTarget* renderTargets[] = { outputTexture };
					context->setRenderTargets(1, renderTargets, NULL);
					CHECK_GL_ERROR
					context->drawQuad();
					CHECK_GL_ERROR
				}
			}
			context->end();
		}

		PostProcessingBloomSetup::~PostProcessingBloomSetup()
		{
		}
	}
}
