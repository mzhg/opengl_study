
#include "PostProcessing.h"
#include "PostProcessingGaussBlur.h"
#include "PostProcessingDownsample.h"
#include "PostProcessingBloomSetup.h"
#include "PostProcessingCombinePass.h"

namespace jet
{
	namespace util
	{
		bool operator < (const PostProcessing::EffectDesc& a, const PostProcessing::EffectDesc& b)
		{
			GEN_LESS_COMPARE(a, b, name);
			GEN_LESS_COMPARE(a, b, length);
			if (a.length == 0)
				return false;

			int cmp = memcmp(a.pData, b.pData, a.length);
			return cmp < 0;
		}

		bool operator == (const PostProcessing::EffectDesc& a, const PostProcessing::EffectDesc& b)
		{
			if (a.name != b.name || a.length != b.length)
				return false;
			
			return memcmp(a.pData, b.pData, a.length) == 0;
		}

		bool operator != (const PostProcessing::EffectDesc& a, const PostProcessing::EffectDesc& b)
		{
			if (a.name != b.name || a.length != b.length)
				return true;

			return memcmp(a.pData, b.pData, a.length) != 0;
		}

		PostProcessing::~PostProcessing() { shutDown(); }

		void PostProcessing::addBloom(float bloomThreshold, float exposureScale, float bloomIntensity)
		{
			addGaussBlur(27);
			m_Parameters.BloomThreshold = Numeric::max(bloomThreshold, 0.25f);
			m_Parameters.ExposureScale = exposureScale;
			m_Parameters.BloomIntensity = Numeric::max(bloomIntensity, 0.01f);

			struct BloomDesc
			{
				float BloomThreshold;
				float ExposureScale;
				float BloomIntensity;
			};

			BloomDesc* desc = new BloomDesc{ m_Parameters.BloomThreshold, m_Parameters.ExposureScale, m_Parameters.BloomIntensity };
			m_CurrentEffects.insert(EffectDesc{ PostProcessingEffect::BLOOM, sizeof(BloomDesc), desc});
		}

		void PostProcessing::addGaussBlur(int kernal)
		{
//			m_CurrentEffects.insert(GAUSS_BLUR);
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

		PostProcessGaussBlur* PostProcessing::createGaussionBlurPass()
		{
			assert(m_iGaussionBlurPassCount < 100);
			return new PostProcessGaussBlur(m_iGaussionBlurPassCount++);
		}

		PostProcessingBloomSetup* PostProcessing::createBloomSetupPass()
		{
			assert(m_iBloomSetupPassCount < 100);
			return new PostProcessingBloomSetup(m_iBloomSetupPassCount++);
		}

		PostProcessingCombinePass* PostProcessing::createCombinePass()
		{
			assert(m_iCombinePassCount < 100);
			return new PostProcessingCombinePass(m_iCombinePassCount++);
		}
		
		PostProcessingDownsample* PostProcessing::createDownsamplePass(DownsampleMethod method)
		{
			assert(m_iDownsamplePassCount < 100);
			return new PostProcessingDownsample(method, m_iDownsamplePassCount++);
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
			PostProcessingDownsample::shutDown();
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


//			if (m_CurrentEffects != m_PrevEffects)
			if (!Numeric::isEqual(m_CurrentEffects, m_PrevEffects))
			{
				checkEffectBits();
				reset();
				m_AddedRenderPasses.clear();
				// add the color and depth buffer first.
				std::shared_ptr<PPRenderPass> sceneColorPass = std::shared_ptr<PPRenderPass>(new PPRenderPassInput(frameAttribs.SceneColorBuffer));
				std::shared_ptr<PPRenderPass> sceneDepthPass = std::shared_ptr<PPRenderPass>(new PPRenderPassInput(frameAttribs.SceneDepthBuffer, static_cast<uint32_t>(INPUT_DEPTH - INPUT_COLOR0)));

				m_AddedRenderPasses.push_back(sceneColorPass);
				m_AddedRenderPasses.push_back(sceneDepthPass);

				bool haveBloomEffect = isBloomEnabled();

				{// Add Bloom Effect
					// first the Add the downsample Pass.
					std::shared_ptr<PPRenderPass> downsamplePass = std::shared_ptr<PPRenderPass>(createDownsamplePass(DownsampleMethod::NORMAL));
					downsamplePass->setDependency(0, sceneColorPass->getName(), 0);
					m_AddedRenderPasses.push_back(downsamplePass);

					std::shared_ptr<PPRenderPass> bloomSetupPass = std::shared_ptr<PPRenderPass>(createBloomSetupPass());
					bloomSetupPass->setDependency(0, downsamplePass->getName(), 0);
					m_AddedRenderPasses.push_back(bloomSetupPass);

					std::shared_ptr<PPRenderPass> gaussionBlurPass = std::shared_ptr<PPRenderPass>(createGaussionBlurPass());
					gaussionBlurPass->setDependency(0, bloomSetupPass->getName(), 0);
					m_AddedRenderPasses.push_back(gaussionBlurPass);

					std::shared_ptr<PPRenderPass> combinePass = std::shared_ptr<PPRenderPass>(createCombinePass());
					combinePass->setDependency(0, sceneColorPass->getName(), 0);
					combinePass->setDependency(1, gaussionBlurPass->getName(), 0);
					m_AddedRenderPasses.push_back(combinePass);
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
				m_PrevEffects.swap(m_CurrentEffects);
			}
		}
	}
}
