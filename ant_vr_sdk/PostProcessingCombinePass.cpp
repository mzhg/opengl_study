#include "PostProcessingCombinePass.h"


namespace jet
{
	namespace util
	{
		PostProcessingCombinePass::PostProcessingCombinePass(uint32_t offset) :
			PPRenderPass(COMBINE + offset)
		{
			set(2, 1);
		}

		PostProcessingCombinePass::~PostProcessingCombinePass()
		{
		}

		static PixelShader* g_DefualtCombineShader = NULL;

		static PixelShader* getDefualtCombineShader()
		{
			if (g_DefualtCombineShader == NULL)
			{
				g_DefualtCombineShader = new PixelShader();
				GLSLProgram::createShaderFromFile("DefaultCombinePS.frag", g_DefualtCombineShader);
			}
			return g_DefualtCombineShader;
		}

		void PostProcessingCombinePass::shutDown()
		{
			SAFE_DISPOSE(g_DefualtCombineShader);
		}

		void PostProcessingCombinePass::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			Texture2D* inputTexture0 = getInput(0);
			Texture2D* inputTexture1 = getInput(1);
			RenderTarget* outputTexture = getOutputTexture(0);
			PixelShader* pShaderPixels = getDefualtCombineShader();

			context->begin();
			{
				context->setShader(pShaderPixels);
				context->setUniform1i("g_Texture0", 0);
				context->setUniform1i("g_Texture1", 1);
				context->setUniform2f("g_f2Intensity", 1.0f, parameters.getBloomIntensity());
				CHECK_GL_ERROR
				{
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(inputTexture0->getTarget(), inputTexture0->getTexture());
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(inputTexture1->getTarget(), inputTexture1->getTexture());
					CHECK_GL_ERROR
					const RenderTarget* renderTargets[] = { outputTexture };
					context->setRenderTargets(1, renderTargets, NULL);
					CHECK_GL_ERROR
					context->drawQuad();
					CHECK_GL_ERROR

					glBindTexture(inputTexture1->getTarget(), 0);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(inputTexture0->getTarget(), 0);
				}
			}
			context->end();
		}
	}
}
