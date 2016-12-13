#include "PostProcessingFXAA.h"


namespace jet
{
	namespace util
	{
		static VertexShader* g_FXAA_VS = NULL;
		static PixelShader*  g_FXAA_PS[6] = { NULL };
		
		static VertexShader* getFXAAVertexShahder()
		{
			if (g_FXAA_VS == NULL)
			{
				g_FXAA_VS = new VertexShader();
				GLSLProgram::createShaderFromFile("fullscreenFXAA.vert", g_FXAA_VS);
			}

			return g_FXAA_VS;
		}

		static PixelShader* getFXAAPixelShader(uint32_t quality)
		{
			if (g_FXAA_PS[quality] == NULL)
			{
				g_FXAA_PS[quality] = new PixelShader();
				Macro macro = { "FXAA_PRESET", Numeric::toString(quality) };
				GLSLProgram::createShaderFromFile("fullscreenFXAA.frag", g_FXAA_PS[quality], false, 1, &macro);
			}

			return g_FXAA_PS[quality];
		}

		void PostProcessingFXAA::shutDown()
		{
			SAFE_DISPOSE(g_FXAA_VS);
			for (int i = 0; i < _countof(g_FXAA_PS); i++)
			{
				SAFE_DISPOSE(g_FXAA_PS[i]);
			}
		}

		void PostProcessingFXAA::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			Texture2D* inputTexture = getInput(0);
			RenderTarget* outputTexture = getOutputTexture(0);
			VertexShader* pShaderVertex = getFXAAVertexShahder();
			PixelShader* pShaderPixels = getFXAAPixelShader(m_Quality);

			context->begin();
			{
				context->setShader(pShaderPixels, pShaderVertex);
				context->setUniform1i("g_Texture", 0);
				CHECK_GL_ERROR
				{
					context->setUniform2f("g_f2FrameRcpFrame", 1.0f / outputTexture->getWidth(), 1.0f / outputTexture->getHeight());
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
	}
}
