#include "PostProcessingGhostImageSetup.h"
#include "ColorModulation.h"

namespace jet
{
	namespace util
	{
		PostProcessingGhostImageSetup::PostProcessingGhostImageSetup(uint32_t offset, uint32_t width, uint32_t height) :
			PPRenderPass(PP_GHOST_IMAGE_SETUP + offset, width, height)
		{
			set(3, 1);
		}

		static VertexShader* g_GhostImageSetupProgramVS = nullptr;
		static PixelShader* g_GhostImageSetupProgramPS = nullptr;

		static void initGhostImageSetupProgram()
		{
			if (g_GhostImageSetupProgramVS == nullptr)
			{
				g_GhostImageSetupProgramVS = new VertexShader;
				GLSLProgram::createShaderFromFile("GhostSetupVS.vert", g_GhostImageSetupProgramVS);

				g_GhostImageSetupProgramPS = new PixelShader;
				GLSLProgram::createShaderFromFile("GhostSetupPS.frag", g_GhostImageSetupProgramPS);
			}
		}

		static void bindTexture2D(Texture2D* pTex, int unit)
		{
			glActiveTexture(GL_TEXTURE0 + unit);
		}

		void PostProcessingGhostImageSetup::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			auto Type = parameters.getLightStreakerStyle();
			Texture2D* inputTexture0 = getInput(0);  // blur0
			Texture2D* inputTexture1 = getInput(1);  // blur1
			Texture2D* lensTexture = getInput(2);    // Lens Mask Texture
			std::unique_ptr<RenderTarget> tempTexture;
			RenderTarget* outputTexture = getOutputTexture(0);
			const ColorModulation& colorModulation = ColorModulation::get();

			RenderTargetPool::getInstance()->findFreeElement(outputTexture->getDesc(), tempTexture);

			initGhostImageSetupProgram();

			context->begin();
			{
				context->setShader(g_GhostImageSetupProgramVS, g_GhostImageSetupProgramPS);
				context->setUniform1i("g_Texture1", 0);
				context->setUniform1i("g_Texture2", 1);
				context->setUniform1i("g_Texture3", 2);
				context->setUniform1i("g_Texture4", 3);

				context->bindTexture(0, inputTexture0);
				context->bindTexture(1, inputTexture1);
				context->bindTexture(2, inputTexture1);
				context->bindTexture(3, lensTexture);

				context->setUniform4f("g_TexCoordScale", -4.0f, 3.0f, -2.0f, 0.3f);
				if (Type == GlareType::CAMERA)
				{
					context->setUniform4fv("g_colorCoeff", 4, colorModulation.camera_ghost_modulation1st);
				}
				else
				{
					context->setUniform4fv("g_colorCoeff", 4, colorModulation.filmic_ghost_modulation1st);
				}

				const RenderTarget* renderTargets[] = { tempTexture.get() };
				context->setRenderTargets(1, renderTargets, nullptr);
				context->drawQuad();
			}

			{
				context->bindTexture(0, tempTexture.get());
				context->bindTexture(1, tempTexture.get());
				context->bindTexture(2, inputTexture1);
				context->bindTexture(3, lensTexture);

				context->setUniform4f("g_TexCoordScale", 3.6f, 2.0f, 0.9f, -0.55f);
				if (Type == GlareType::CAMERA)
				{
					context->setUniform4fv("g_colorCoeff", 4, colorModulation.camera_ghost_modulation2nd);
				}
				else
				{
					context->setUniform4fv("g_colorCoeff", 4, colorModulation.filmic_ghost_modulation2nd);
				}

				const RenderTarget* renderTargets[] = {outputTexture };
				context->setRenderTargets(1, renderTargets, nullptr);
				context->drawQuad();
			}
			context->end();

			RenderTargetPool::getInstance()->freeUnusedResource(tempTexture);
		}

		PostProcessingGhostImageSetup::~PostProcessingGhostImageSetup()
		{
		}

		void PostProcessingGhostImageSetup::shutDown()
		{
			SAFE_DELETE(g_GhostImageSetupProgramVS);
			SAFE_DELETE(g_GhostImageSetupProgramPS);
		}
	}
}
