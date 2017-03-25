#include "PostProcessingGlareCompose.h"

namespace jet
{
	namespace util
	{
		PostProcessingGlareCompose::PostProcessingGlareCompose(glm::vec4 mixCoeff, uint32_t offset, uint32_t width, uint32_t height):
			PPRenderPass(PP_GLARE_COMPOSE + offset, width, height), MixCoeff(mixCoeff)
		{
			set(4, 1);
		}

		static PixelShader* g_GlareComposePS = nullptr;

		static PixelShader* getGlareComposePS()
		{
			if (g_GlareComposePS == nullptr)
			{
				g_GlareComposePS = new PixelShader;
				GLSLProgram::createShaderFromFile("GlareCompose.frag", g_GlareComposePS);
			}

			return g_GlareComposePS;
		}

		void PostProcessingGlareCompose::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			Texture2D* inputTexture0 = getInput(0); 
			Texture2D* inputTexture1 = getInput(1); 
			Texture2D* inputTexture2 = getInput(2);
			Texture2D* inputTexture3 = getInput(3);

			const RenderTarget* outputTexture = getOutputTexture(0);

			context->begin();
			{
				context->setShader(getGlareComposePS());

				context->setUniform1i("g_Texture1", 0);
				context->setUniform1i("g_Texture2", 1);
				context->setUniform1i("g_Texture3", 2);
				context->setUniform1i("g_Texture4", 3);

				context->bindTexture(0, inputTexture0);
				context->bindTexture(1, inputTexture1);
				context->bindTexture(2, inputTexture2);
				context->bindTexture(3, inputTexture3);

				context->setUniform4f("g_TexCoordScale", MixCoeff.x, MixCoeff.y, MixCoeff.z, MixCoeff.w);
				context->setRenderTargets(1, &outputTexture, nullptr);

				context->drawQuad();
			}
			context->end();
		}

		void PostProcessingGlareCompose::shutDown()
		{
			SAFE_DELETE(g_GlareComposePS);
		}

		PostProcessingGlareCompose::~PostProcessingGlareCompose()
		{
		}
	}
}
