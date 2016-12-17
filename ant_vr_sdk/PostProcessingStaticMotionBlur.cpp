#include "PostProcessingStaticMotionBlur.h"

namespace jet
{
	namespace util
	{
		static PixelShader* g_BloomSetupPS[2] = { NULL };

		static const int COMBINED_TEXTURE = 0;
		static const int SPERATE_TEXTURE = 1;

		static PixelShader* getStaticMotionBlurPS(int idx)
		{
			if (g_BloomSetupPS[idx] == NULL)
			{
				g_BloomSetupPS[idx] = new PixelShader();
				Macro macros = { "ENABLE_DEPTH_ALPHA", "1" };

				GLSLProgram::createShaderFromFile("StaticSceneMotionBlur.frag", g_BloomSetupPS[idx], false, (idx == COMBINED_TEXTURE) ? 1: 0, &macros);
			}

			return g_BloomSetupPS[idx];
		}

		void PostProcessingStaticMotionBlur::shutDown()
		{
			for (int i = 0; i < _countof(g_BloomSetupPS); i++)
			{
				SAFE_DISPOSE(g_BloomSetupPS[i]);
			}
		}

		void PostProcessingStaticMotionBlur::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			uint32_t inputCount = getPassDesc().InputCount;
			Texture2D* inputTexture0 = getInput(0);  // Color
			Texture2D* inputTexture1 = getInput(1);  // Depth

			RenderTarget* outputTexture = getOutputTexture(0);
			PixelShader* pShaderPixels = getStaticMotionBlurPS((inputCount == 1)? COMBINED_TEXTURE: SPERATE_TEXTURE);

			context->begin();
			{
				context->setShader(pShaderPixels);
				context->setUniform1i("g_SceneColor", 0);
				context->setUniform1i("g_SceneDepth", 1);
				CHECK_GL_ERROR
				{
					context->setUniformMatrix4( "g_ViewProjectionInverseMatrix", 1, false, (const float*)&parameters.getViewProjectionInverse());
					context->setUniformMatrix4("g_previousViewProjectionMatrix", 1, false, (const float*)&parameters.getPreviouseViewProjection());
					CHECK_GL_ERROR
						glActiveTexture(GL_TEXTURE0);
					glBindTexture(inputTexture0->getTarget(), inputTexture0->getTexture());
					if (inputCount > 1)
					{
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(inputTexture1->getTarget(), inputTexture1->getTexture());
					}
					CHECK_GL_ERROR
						const RenderTarget* renderTargets[] = { outputTexture };
					context->setRenderTargets(1, renderTargets, NULL);
					CHECK_GL_ERROR
						context->drawQuad();
					CHECK_GL_ERROR;

					if (inputCount > 1)
					{
						glActiveTexture(GL_TEXTURE1);
						glBindTexture(inputTexture1->getTarget(), 0);
					}
				}
			}
			context->end();
		}
	}
}
