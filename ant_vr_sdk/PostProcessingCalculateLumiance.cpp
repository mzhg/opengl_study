#include "PostProcessingCalculateLumiance.h"
#include <GLCapabilities.h>

namespace jet
{
	namespace util
	{

#ifdef __cplusplus
#define xxx 1
#endif

		static PixelShader* g_CalculateLuminanceProgramPS = nullptr;
		static PixelShader* g_CalculateAdaptedLumPS = nullptr;

		static ComputeShader* g_CalculateLuminanceProgramCS = nullptr;
		static ComputeShader* g_CalculateAdaptedLumCS = nullptr;

		static void initCalculateShaders()
		{
			const GLCapabilities* pCap = GLCapabilities::get();

			if (pCap->OpenGL43)
			{
				if (g_CalculateLuminanceProgramCS == nullptr)
				{
					g_CalculateLuminanceProgramCS = new ComputeShader;
					g_CalculateAdaptedLumCS = new ComputeShader();

					GLSLProgram::createShaderFromFile("cs_calculateLuminance.glcs", g_CalculateLuminanceProgramCS);
					GLSLProgram::createShaderFromFile("cs_calculateAdaptedLum.glcs", g_CalculateAdaptedLumCS);
				}
			}
			else
			{
				if (g_CalculateLuminanceProgramPS == nullptr)
				{
					g_CalculateLuminanceProgramPS = new PixelShader;
					g_CalculateAdaptedLumPS = new PixelShader();

					GLSLProgram::createShaderFromFile("CalculateLuminance.frag", g_CalculateLuminanceProgramPS);
					GLSLProgram::createShaderFromFile("CalculateAdaptedLum.frag", g_CalculateAdaptedLumPS);
				}
			}
		}



		void PostProcessingCalculateLumiance::shutDown()
		{
			SAFE_DELETE(g_CalculateLuminanceProgramCS);
			SAFE_DELETE(g_CalculateAdaptedLumCS);
			SAFE_DELETE(g_CalculateLuminanceProgramPS);
			SAFE_DELETE(g_CalculateAdaptedLumPS);
		}

		PostProcessingCalculateLumiance::PostProcessingCalculateLumiance(uint32_t offset): 
			PPRenderPass(CALCULATE_LUMIANCE + offset), m_pLumTex(nullptr)
		{
			set(1, 1);
		}


		PostProcessingCalculateLumiance::~PostProcessingCalculateLumiance()
		{
			if (m_pLumTex)
			{
				delete m_pLumTex;
			}
		}

		void PostProcessingCalculateLumiance::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			GLStates& states = GLStates::get();
			const GLCapabilities* pCap = GLCapabilities::get();
			Texture2D* inputTexture = getInput(0);
			RenderTarget* outputTexture = getOutputTexture(0);

			std::unique_ptr<RenderTarget> tempTexture;
			RenderTargetPool::getInstance()->findFreeElement(outputTexture->getDesc(), tempTexture);

			if (m_pLumTex == nullptr)
			{
				m_pLumTex = new Texture2D;
				Texture2DDesc desc;
				computeOutDesc(0, desc);

				TextureUtil::createTexture2D(&desc, nullptr, m_pLumTex);
			}

			if (!pCap->OpenGL43)
			{
				context->begin();
			}
			
			// first pass: calculate the averige ilummiance.
			{
				if (pCap->OpenGL43)
				{
					context->setCSShader(g_CalculateLuminanceProgramCS);
					glBindImageTexture(0, inputTexture->getTexture(), 0, false, 0, GL_READ_ONLY, inputTexture->getFormat());
					glBindImageTexture(1, tempTexture->getTexture(), 0, false, 0, GL_WRITE_ONLY, tempTexture->getFormat());
					glDispatchCompute(1, 1, 1);
				}
				else
				{
					context->setShader(g_CalculateLuminanceProgramPS);
					context->setUniform1i("inputImage", 0);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(inputTexture->getTarget(), inputTexture->getTexture());
					const RenderTarget* renderTargets[] = { tempTexture.get() };
					context->setRenderTargets(1, renderTargets, NULL);
					CHECK_GL_ERROR
					context->drawQuad();
					CHECK_GL_ERROR
				}
			}

			// second pass: calculate the adapted lumiance.
			{
				if (pCap->OpenGL43)
				{
					context->setCSShader(g_CalculateAdaptedLumCS);
					glBindImageTexture(0, tempTexture->getTexture(), 0, false, 0, GL_READ_ONLY, tempTexture->getFormat());
					glBindImageTexture(1, m_pLumTex->getTexture(), 0, false, 0, GL_WRITE_ONLY, m_pLumTex->getFormat());
					glBindImageTexture(1, outputTexture->getTexture(), 0, false, 0, GL_WRITE_ONLY, outputTexture->getFormat());
					glDispatchCompute(1, 1, 1);

					glCopyImageSubData(outputTexture->getTexture(), outputTexture->getTarget(), 0, 0, 0, 0,
										m_pLumTex->getTexture(), m_pLumTex->getTarget(), 0, 0, 0, 0,
										1, 1, 1);
				}
				else
				{
					context->setShader(g_CalculateAdaptedLumPS);
					context->setUniform1i("currentImage", 0);
					context->setUniform1i("image0", 1);
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(tempTexture->getTarget(), tempTexture->getTexture());
					glActiveTexture(GL_TEXTURE1);
					glBindTexture(m_pLumTex->getTarget(), m_pLumTex->getTexture());

					const RenderTarget* renderTargets[] = { outputTexture };
					context->setRenderTargets(1, renderTargets, NULL);
					CHECK_GL_ERROR
					context->drawQuad();
					CHECK_GL_ERROR
				}
			}

			if (!pCap->OpenGL43)
			{
				context->end();
			}

			// free the temp texture.
			RenderTargetPool::getInstance()->freeUnusedResource(tempTexture);
		}

		
	}
}
