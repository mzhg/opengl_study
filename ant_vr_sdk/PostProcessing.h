
#ifndef _JET_UTIL_POSTPROCESSING_H
#define _JET_UTIL_POSTPROCESSING_H

#include "Numeric.h"
#include "PostProcessingFramework.h"
#include <set>

#include "PostProcessGaussBlur.h"
namespace jet
{
	namespace util
	{
		struct PPRectangle
		{
			GLint X, Y;
			GLint Width, Height;

			PPRectangle() : X(0), Y(0), Width(0), Height(0){}
			PPRectangle(GLint x, GLint y, GLint w, GLint h):
				X(x), Y(y), Width(w), Height(h)
			{}

			bool isValid() { return Width > 0 && Height > 0; }
		};

		struct FrameAttribs
		{
			PPRectangle Viewport;
			PPRectangle ClipRect;
			Texture2D* SceneColorBuffer;
			Texture2D* SceneDepthBuffer;
			
			// The Output destination, if NULL, will output to the default framebuffer.
			Texture2D* OutputBuffer;
		};

		class PostProcessing
		{

		public:
			PostProcessing() :m_RenderContext(NULL){}

			void addGaussBlur(int kernal)
			{
				m_CurrentEffects.insert(GAUSS_BLUR);
				kernal = Numeric::max(1, kernal);
				if (kernal % 2 == 0)
				{
					kernal++;
				}
				m_Parameters.GaussBlur_Kernal = kernal;
			}

			void performancePostProcessing(const FrameAttribs& frameAttribs)
			{
				prepare(frameAttribs);

				m_RenderContext->performancePostProcessing(/* TODO: don't forget the parameters */);

				// Clear effects for next loop
				m_CurrentEffects.clear();

				if (!m_AddedRenderPasses.empty())
				{
					Texture2D* src = m_AddedRenderPasses.back().get()->getOutput(0);
					m_RenderContext->renderTo(src, frameAttribs.OutputBuffer);
				}
			}

			void shutDown()
			{
				if (m_RenderContext != NULL)
				{
					m_RenderContext->shutDown();
					m_RenderContext = NULL;
				}

				m_AddedRenderPasses.clear();
				m_CurrentEffects.clear();

				RenderTargetPool::shutDown();
				PostProcessGaussBlur::shutDown();
			}

		private:
			void prepare(const FrameAttribs& frameAttribs)
			{
				if (m_RenderContext == NULL)
				{
					m_RenderContext = new PPRenderContext();
					m_RenderContext->initlizeGL();
				}

				if (m_CurrentEffects.empty())
				{
					return;
				}


				if (m_CurrentEffects != m_PrevEffects)
				{
					m_AddedRenderPasses.clear();
					// add the color and depth buffer first.
					std::shared_ptr<PPRenderPass> sceneColorPass = std::shared_ptr<PPRenderPass>(new PPRenderPassInput(frameAttribs.SceneColorBuffer));
					std::shared_ptr<PPRenderPass> sceneDepthPass = std::shared_ptr<PPRenderPass>(new PPRenderPassInput(frameAttribs.SceneDepthBuffer, static_cast<uint32_t>(INPUT_DEPTH - INPUT_COLOR0)));

					m_AddedRenderPasses.push_back(sceneColorPass);
					m_AddedRenderPasses.push_back(sceneDepthPass);

					bool haveGaussBlur = m_CurrentEffects.find(GAUSS_BLUR) != m_CurrentEffects.end();

					std::shared_ptr<PPRenderPass> guassBlurPass = sceneColorPass;
					if (haveGaussBlur)
					{
						PPRenderPass* pGaussBlurPass = new PostProcessGaussBlur();
						guassBlurPass = std::shared_ptr<PPRenderPass>(pGaussBlurPass);
						m_AddedRenderPasses.push_back(guassBlurPass);
						guassBlurPass->setDependency(0, sceneColorPass.get()->getName(), 0);
					}

					std::map<PassName, int> passDenpendencyCount;
					const size_t count = m_AddedRenderPasses.size();
					for (int i = static_cast<int>(count - 1); i > 0; i--)
					{
						const std::shared_ptr<PPRenderPass>& pass = m_AddedRenderPasses[i];
						PPRenderPassDesc desc = pass->getPassDesc();
						for (uint32_t j = 0; j < desc.InputCount; j++)
						{
							if (desc.InputDescs[j].DependencyPass != INPUT_NONE)
							{
								passDenpendencyCount[desc.InputDescs[j].DependencyPass] ++;  // TODO, I am not sure....
							}
						}
					}

					for (size_t i = 0; i < count; i++)
					{
						PassName name = m_AddedRenderPasses[i]->getName();
						m_AddedRenderPasses[i]->setDependencies(passDenpendencyCount[name]);
					}

					m_RenderContext->setRenderPasses(m_AddedRenderPasses);

					m_PrevEffects.clear();
					m_PrevEffects.insert(m_CurrentEffects.begin(), m_CurrentEffects.end());
				}
			}

		private:
			std::set<PassName> m_CurrentEffects;
			std::set<PassName> m_PrevEffects;

			std::vector<std::shared_ptr<PPRenderPass>> m_AddedRenderPasses;
			PostProcessingParameters m_Parameters;

			PPRenderContext* m_RenderContext;
		};
	}
}

#endif