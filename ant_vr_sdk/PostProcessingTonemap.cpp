#include "PostProcessingTonemap.h"

namespace jet
{
	namespace util
	{
		PostProcessingTonemap::PostProcessingTonemap(GLuint width, GLuint height):
			PPRenderPass(PP_TONEMAP, width, height)
		{
			set(3, 1);
		}

		static PixelShader* g_TonemapProgramPS = nullptr;

		static PixelShader* getTonemapProgramPS()
		{
			if (g_TonemapProgramPS == nullptr)
			{
				g_TonemapProgramPS = new PixelShader;
				GLSLProgram::createShaderFromFile("Tonemap.frag", g_TonemapProgramPS);
			}

			return g_TonemapProgramPS;
		}

		void PostProcessingTonemap::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			Texture2D* sceneTexture = getInput(0);   // scene Texture
			Texture2D* glareTexture = getInput(1);   // glare texture
			Texture2D* lumTexture = getInput(2);   // eye adapation texture(lumiance texture)

			RenderTarget* output = getOutputTexture(0);

			context->begin();
			{
				context->setShader(getTonemapProgramPS());
				context->setUniform1i("sceneTex", 0);
				context->setUniform1i("blurTex", 1);
				context->setUniform1i("lumTex", 2);

				context->bindTexture(0, sceneTexture);
				context->bindTexture(1, glareTexture);
				context->bindTexture(2, lumTexture);

				context->setUniform4f("g_ToneParams", parameters.getHDRBlendAmount(), parameters.getHDRExplosure(), parameters.getHDRGamma(), 0.0f);

				const RenderTarget* renderTargets[] = { output };
				context->setRenderTargets(1, renderTargets, nullptr);
				context->drawQuad();
			}
			context->end();
		}

		void PostProcessingTonemap::shutDown()
		{
			SAFE_DELETE(g_TonemapProgramPS);
		}

		PostProcessingTonemap::~PostProcessingTonemap()
		{
			SAFE_DELETE(g_TonemapProgramPS);
		}
	}
}
