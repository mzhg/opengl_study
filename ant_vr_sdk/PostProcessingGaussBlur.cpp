#include "PostProcessingGaussBlur.h"
#include "GaussianBlur.h"
#include "FileUtil.h"

namespace jet
{
	namespace util
	{
		
		PPGuassBlurPS::PPGuassBlurPS(int kernelSize)
		{
			GLVersion version = GetOpenGLVersion();
			std::string shaderCode;
			shaderCode += "#version ";
			shaderCode += version.toString();
			shaderCode += "\n";

			shaderCode += "precision highp float;\n";

			std::string gassCode = GenerateGaussFunctionCode(kernelSize, false);
			
			shaderCode += gassCode;

			int32_t len;
			char* pMainCode = AssetLoaderRead("GuassBlurPS_Template.frag", len);
			shaderCode += pMainCode;
			AssetLoaderFree(pMainCode);
			ShaderSourceItem sourceItem = ShaderSourceItem(shaderCode.c_str(), GL_FRAGMENT_SHADER);
//			printf("Guass Blur Shaders:\n %s\n", shaderCode.c_str());
			FileUtil::saveStringToFile("E:/shaders/guass_blurPS.frag", shaderCode.c_str());

			GLSLProgram::createShaderFromStrings(sourceItem, this);
#if 0
			m_HalfSizeIndex = getUniformLocation("g_HalfPixelSize", true);
			assert(m_HalfSizeIndex >= 0);
			GLint textureIndex = getUniformLocation("", true);
			enable();
			glUniform1i(textureIndex, 0);
			disable();
#endif
			checkGLError();
		}

#if 0
		void PPGuassBlurPS::setTextrueHalfSize(float texelX, float texelY)
		{
			glUniform2f(m_HalfSizeIndex, texelX, texelY);
		}
#endif

		PostProcessGaussBlur::~PostProcessGaussBlur()
		{
		}

		void PostProcessGaussBlur::process(PPRenderContext* context, const PostProcessingParameters& parameters)
		{
			Texture2D* inputTexture = getInput(0);
			RenderTarget* outputTexture = getOutputTexture(0);
			std::unique_ptr<RenderTarget> tempTexture;
			RenderTargetPool::getInstance()->findFreeElement(inputTexture->getDesc(), tempTexture);
			PPGuassBlurPS* pixelShader = getGuassBlurPS(parameters.getGaussBlurKernal());

			checkGLError();
			context->begin();
			{
				context->setShader(pixelShader);
				context->setUniform1i("g_Texture", 0);
				checkGLError();
				{  // first pass, draw the hori blur to the tempTexture.
					context->setUniform2f("g_HalfPixelSize", 1.0f / inputTexture->getWidth(), 0);
					checkGLError();
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(inputTexture->getTarget(), inputTexture->getTexture());
					checkGLError();
					const RenderTarget* renderTargets[] = { tempTexture.get() };
					context->setRenderTargets(1, renderTargets, NULL);
					checkGLError();
					context->drawQuad();
					checkGLError();
				}
				
				checkGLError();

				{
					context->setUniform2f("g_HalfPixelSize", 0, 1.0f / inputTexture->getHeight());

					glActiveTexture(GL_TEXTURE0);
					glBindTexture(tempTexture->getTarget(), tempTexture->getTexture());
					const RenderTarget* renderTargets[] = { outputTexture };
					context->setRenderTargets(1, renderTargets, NULL);
					context->drawQuad();
				}

				checkGLError();
				
			}
			context->end();

			// free the temp texture.
			RenderTargetPool::getInstance()->freeUnusedResource(tempTexture);
		}

		static std::map<int, PPGuassBlurPS*> g_GuassBlurPS;

		PPGuassBlurPS* PostProcessGaussBlur::getGuassBlurPS(int kernelSize)
		{
			auto it = g_GuassBlurPS.find(kernelSize);
			if (it == g_GuassBlurPS.end())
			{
				PPGuassBlurPS* result = new PPGuassBlurPS(kernelSize);
				g_GuassBlurPS.insert(std::pair<int, PPGuassBlurPS*>(kernelSize, result));
				return result;
			}
			else
			{
				return it->second;
			}
		}

		void PostProcessGaussBlur::shutDown()
		{
			for (auto it = g_GuassBlurPS.begin(); it != g_GuassBlurPS.end(); it++)
			{
				delete it->second;
			}
			g_GuassBlurPS.clear();
		}
	}
}
