#include "PostProcessingDownsample.h"

namespace jet
{
	namespace util
	{

#if 0
		//		FASTEST = 0,
		//			NORMAL,
		//			COMBINED_DEPTH,
#define GEN_PS_VARIABLE(x) static PPDownsamplePS<DownsampleMethod::x>* g_DS_##x##_PS = NULL;
#define GEN_PS_CASE(x)  case DownsampleMethod::x:\
						if (g_DS_##x##_PS == NULL)  \
												{\
							g_DS_##x##_PS = new PPDownsamplePS<DownsampleMethod::x>();\
												}\
						return g_DS_##x##_PS;

		GEN_PS_VARIABLE(FASTEST)
			GEN_PS_VARIABLE(NORMAL)
			GEN_PS_VARIABLE(COMBINED_DEPTH)

			template<DownsampleMethod Method>
		static PPDownsamplePS<Method>* getDownsamplePS()
		{
			switch (Method)
			{
				GEN_PS_CASE(FASTEST)
					GEN_PS_CASE(NORMAL)
					GEN_PS_CASE(COMBINED_DEPTH)
			default:
				break;
			}
		}
#else

		static PPDownsamplePS* g_DwonsamplePS[static_cast<int>(DownsampleMethod::COUNT)] = {NULL};
		
		PPDownsamplePS* getDownsamplePS(DownsampleMethod method)
		{
			int index = static_cast<int>(method);
			PPDownsamplePS* result = g_DwonsamplePS[index];
			if (result == NULL)
			{
				result = new PPDownsamplePS(method);
			}

			return result;
		}

		void PostProcessingDownsample::shutDown()
		{
			for (int i = 0; i < static_cast<int>(DownsampleMethod::COUNT); i++)
			{
				if (g_DwonsamplePS[i])
				{
					delete g_DwonsamplePS[i];
					g_DwonsamplePS[i] = NULL;
				}
			}
		}
#endif

		PostProcessingDownsample::PostProcessingDownsample(DownsampleMethod method, uint32_t offset):
			PPRenderPass(DOWNSAMPLE + offset), Method(method){
			set(1, 1);
		}


		void PostProcessingDownsample::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			Texture2D* inputTexture = getInput(0);
			RenderTarget* outputTexture = getOutputTexture(0);
			PPDownsamplePS* pShaderPixels = getDownsamplePS(Method);

			context->begin();
			{
				context->setShader(pShaderPixels);
				context->setUniform1i("g_Texture", 0);
				checkGLError();
				{  
					context->setUniform2f("g_TexelSize", 1.0f / inputTexture->getWidth(), 1.0f/inputTexture->getHeight());
					checkGLError();
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(inputTexture->getTarget(), inputTexture->getTexture());
					checkGLError();
					const RenderTarget* renderTargets[] = { outputTexture };
					context->setRenderTargets(1, renderTargets, NULL);
					checkGLError();
					context->drawQuad();
					checkGLError();
				}
			}
			context->end();
		}

		PostProcessingDownsample::~PostProcessingDownsample()
		{
		}
	}
}

