
#include "PostProcessing.h"

namespace jet
{
	namespace util
	{
		PostProcessing::~PostProcessing() { shutDown(); }

		void PostProcessing::addGaussBlur(int kernal)
		{
			m_CurrentEffects.insert(GAUSS_BLUR);
			if (kernal < 1)
				// kernel less 1 we could igore the gauss blur.
				return;

			kernal = Numeric::max(3, kernal);
			int n = (kernal + 1) / 4;
			if (n >= 1 && (kernal + 1) % 4 != 0)
			{
				// rounded up
				n++;
			}

			kernal = 4 * n - 1;
			m_Parameters.GaussBlur_Kernal = kernal;
		}

		void PostProcessing::performancePostProcessing(const FrameAttribs& frameAttribs)
		{
			prepare(frameAttribs);

			checkGLError();
			m_RenderContext->performancePostProcessing(/* TODO: don't forget the parameters */);
			checkGLError();
			// Clear effects for next loop
			m_CurrentEffects.clear();

			if (!m_AddedRenderPasses.empty())
			{
				Texture2D* src = m_AddedRenderPasses.back().get()->getOutput(0);
				m_RenderContext->renderTo(src, frameAttribs.OutputBuffer);
				m_RenderContext->finalize();
			}
		}

		void PostProcessing::shutDown()
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

		void PostProcessing::prepare(const FrameAttribs& frameAttribs)
		{
			if (m_RenderContext == NULL)
			{
				m_RenderContext = new PPRenderContext();
				m_RenderContext->initlizeGL();
				m_RenderContext->setParameters(&m_Parameters);
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

				std::shared_ptr<PPRenderPass> guassBlurPass;
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

				m_AddedRenderPasses.back()->setDependencies(1);
				m_RenderContext->setRenderPasses(m_AddedRenderPasses);

				m_PrevEffects.clear();
				m_PrevEffects.insert(m_CurrentEffects.begin(), m_CurrentEffects.end());
			}
		}
	}
}
