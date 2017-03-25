#include "PostProcessingLightStreaker.h"
#include "ColorModulation.h"

namespace jet
{
	namespace util
	{
		PostProcessingLightStreaker::PostProcessingLightStreaker(uint32_t offset, uint32_t width, uint32_t height) :
			PPRenderPass(PP_LIGHT_STREAKER + offset, Width, Height)
		{
			set(1, 1);
		}

		static VertexShader* g_StarStreakerVS = nullptr;
		static PixelShader*  g_StarStreakerPS = nullptr;
		static PixelShader*  g_StreakerComposePS = nullptr;

		static void initStarStreakerShaders()
		{
			if (g_StarStreakerVS == nullptr)
			{
				g_StarStreakerVS = new VertexShader();
				GLSLProgram::createShaderFromFile("StarStreakerVS.vert", g_StarStreakerVS);

				g_StarStreakerPS = new PixelShader();
				GLSLProgram::createShaderFromFile("StarStreakerPS.frag", g_StarStreakerPS);

				g_StreakerComposePS = new PixelShader();
				GLSLProgram::createShaderFromFile("StarSteakerCompose.frag", g_StreakerComposePS);
			}
		}

		void PostProcessingLightStreaker::genHorizontalGlare(PPRenderContext* context, RenderTarget* output, int dir)
		{
			Texture2D* inputTexture = getInput(0);
			std::unique_ptr<RenderTarget> tempTexture;

			const float delta = 0.9f;
			int n, s, w, h;
			float step0, step1;
			float stride = 1.0f;
			w = output->getWidth();
			h = output->getHeight();

			const ColorModulation& colorModulation = ColorModulation::get();
			RenderTargetPool::getInstance()->findFreeElement(inputTexture->getDesc(), tempTexture);

			if (dir == 0) {
				step0 = (delta) / w;
			}
			else {
				step0 = -(delta) / w;
			}
			step1 = 0;

			{
				// inputTexture --> tempTexture
				const RenderTarget* renderTargets[] = { tempTexture.get() };
				context->setRenderTargets(1, renderTargets, nullptr);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(inputTexture->getTarget(), inputTexture->getTexture());
				context->setUniform4f("g_StepAndStride", step0, step1, stride, 0.0f);
				context->setUniform4fv("g_colorCoeff", 4, colorModulation.hori_passes[ColorModulation::HORI0]);
				context->drawQuad();
			}

			{
				// tempTexture --> output
				stride = 4.0f;
				const RenderTarget* renderTargets[] = { output };
				context->setRenderTargets(1, renderTargets, nullptr);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(tempTexture->getTarget(), tempTexture->getTexture());
				context->setUniform4f("g_StepAndStride", step0, step1, stride, 0.0f);
				context->setUniform4fv("g_colorCoeff", 4, colorModulation.hori_passes[ColorModulation::HORI1]);
				context->drawQuad();
			}

			{
				// output --> tempTexture
				stride = 16.0f;
				const RenderTarget* renderTargets[] = { tempTexture.get() };
				context->setRenderTargets(1, renderTargets, nullptr);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(output->getTarget(), output->getTexture());
				context->setUniform4f("g_StepAndStride", step0, step1, stride, 0.0f);
				context->setUniform4fv("g_colorCoeff", 4, colorModulation.hori_passes[ColorModulation::HORI2]);
				context->drawQuad();
			}

			{
				// tempTexture --> output
				stride = 64.0f;
				const RenderTarget* renderTargets[] = { output };
				context->setRenderTargets(1, renderTargets, nullptr);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(tempTexture->getTarget(), tempTexture->getTexture());
				context->setUniform4f("g_StepAndStride", step0, step1, stride, 0.0f);
				context->setUniform4fv("g_colorCoeff", 4, colorModulation.hori_passes[ColorModulation::HORI3]);
				context->drawQuad();
			}

			RenderTargetPool::getInstance()->freeUnusedResource(tempTexture);
		}

		void PostProcessingLightStreaker::genStarStreak(PPRenderContext* context, RenderTarget* output, int dir, float ratio)
		{
			Texture2D* inputTexture = getInput(0);
			std::unique_ptr<RenderTarget> tempTexture;
			const float delta = 0.9f;
			int n, s, w, h;
			//    	float[] step = new float[2];
			float step0 = 0, step1 = 0;
			float stride = 1.0f;
			w = output->getWidth();
			h = output->getHeight();
			float m_aspectRatio = ratio;
			const ColorModulation& colorModulation = ColorModulation::get();

			RenderTargetPool::getInstance()->findFreeElement(inputTexture->getDesc(), tempTexture);

			switch (dir) {
			case 0:
				step1 = (delta) / w*m_aspectRatio;
				step0 = (delta) / w;
				break;
			case 1:
				step1 = (delta) / w*m_aspectRatio;
				step0 = -(delta) / w;
				break;
			case 2:
				step1 = -(delta) / w*m_aspectRatio;
				step0 = (delta) / w;
				break;
			case 3:
				step1 = -(delta) / w*m_aspectRatio;
				step0 = -(delta) / w;
				break;
			default:
				break;
			}
			
			{
				// inputTexture --> output
				const RenderTarget* renderTargets[] = { output };
				context->setRenderTargets(1, renderTargets, nullptr);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(inputTexture->getTarget(), inputTexture->getTexture());
				context->setUniform4f("g_StepAndStride", step0, step1, stride, 0.0f);
				context->setUniform4fv("g_colorCoeff", 4, colorModulation.hori_passes[ColorModulation::STAR0]);
				context->drawQuad();
			}

			{
				// output --> tempTexture
				stride = 4.0f;
				const RenderTarget* renderTargets[] = { tempTexture.get() };
				context->setRenderTargets(1, renderTargets, nullptr);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(output->getTarget(), output->getTexture());
				context->setUniform4f("g_StepAndStride", step0, step1, stride, 0.0f);
				context->setUniform4fv("g_colorCoeff", 4, colorModulation.hori_passes[ColorModulation::STAR1]);
				context->drawQuad();
			}

			{
				// tempTexture --> output
				stride = 16.0f;
				const RenderTarget* renderTargets[] = { output };
				context->setRenderTargets(1, renderTargets, nullptr);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(tempTexture->getTarget(), tempTexture->getTexture());
				context->setUniform4f("g_StepAndStride", step0, step1, stride, 0.0f);
				context->setUniform4fv("g_colorCoeff", 4, colorModulation.hori_passes[ColorModulation::STAR2]);
				context->drawQuad();
			}

			RenderTargetPool::getInstance()->freeUnusedResource(tempTexture);
		}

		void PostProcessingLightStreaker::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			const auto Type = parameters.getLightStreakerStyle();
			const PPRectangle& size = context->getScreenSize();
			const float ratio = Numeric::ratio(size.Width, size.Height);

			Texture2DDesc desc;
			computeOutDesc(0, desc);
			std::unique_ptr<RenderTarget> tempTexture0;
			std::unique_ptr<RenderTarget> tempTexture1;
			std::unique_ptr<RenderTarget> tempTexture2;
			std::unique_ptr<RenderTarget> tempTexture3;

			RenderTargetPool::getInstance()->findFreeElement(desc, tempTexture0);
			RenderTargetPool::getInstance()->findFreeElement(desc, tempTexture1);

			initStarStreakerShaders();

			context->begin();
			{ // First pass: gen the star streakers
				
				context->setShader(g_StarStreakerVS, g_StarStreakerPS);
				context->setUniform1i("g_Texture0", 0);

				if (Type == GlareType::CAMERA)
				{
					RenderTargetPool::getInstance()->findFreeElement(desc, tempTexture2);
					RenderTargetPool::getInstance()->findFreeElement(desc, tempTexture3);

					genStarStreak(context, tempTexture0.get(), 0, ratio);
					genStarStreak(context, tempTexture1.get(), 1, ratio);
					genStarStreak(context, tempTexture2.get(), 2, ratio);
					genStarStreak(context, tempTexture3.get(), 3, ratio);
				}
				else  // Type == GlareType::FILM
				{
					genHorizontalGlare(context, tempTexture0.get(), 0);
					//    		GLError.checkError();
					genHorizontalGlare(context, tempTexture1.get(), 1);
				}
			}
			
			{ // Second pass: Combine the streakers
				context->setShader(g_StreakerComposePS);
				context->setUniform1i("g_Texture1", 0);
				context->setUniform1i("g_Texture2", 1);
				context->setUniform1i("g_Texture3", 2);
				context->setUniform1i("g_Texture4", 3);

				const RenderTarget* final_output = getOutputTexture(0);
				context->setRenderTargets(1, &final_output, nullptr);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(tempTexture0->getTarget(), tempTexture0->getTexture());
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(tempTexture1->getTarget(), tempTexture1->getTexture());

				if (Type == GlareType::CAMERA)
				{
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(tempTexture2->getTarget(), tempTexture2->getTexture());
					glActiveTexture(GL_TEXTURE3);
					glBindTexture(tempTexture3->getTarget(), tempTexture3->getTexture());
				}
				else
				{
					glActiveTexture(GL_TEXTURE2);
					glBindTexture(GL_TEXTURE_2D, 0);
					glActiveTexture(GL_TEXTURE3);
					glBindTexture(GL_TEXTURE_2D, 0);
				}

				context->drawQuad();

				// unbind the textures.
				glActiveTexture(GL_TEXTURE2);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE3);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
				glActiveTexture(GL_TEXTURE0);
			}
			context->end();

			RenderTargetPool::getInstance()->freeUnusedResource(tempTexture0);
			RenderTargetPool::getInstance()->freeUnusedResource(tempTexture1);
			if (Type == GlareType::CAMERA)
			{
				RenderTargetPool::getInstance()->freeUnusedResource(tempTexture2);
				RenderTargetPool::getInstance()->freeUnusedResource(tempTexture3);
			}
		}

		void PostProcessingLightStreaker::shutDown()
		{
			SAFE_DELETE(g_StarStreakerVS);
			SAFE_DELETE(g_StarStreakerPS);
			SAFE_DELETE(g_StreakerComposePS);
		}

		PostProcessingLightStreaker::~PostProcessingLightStreaker(){}
	}
}