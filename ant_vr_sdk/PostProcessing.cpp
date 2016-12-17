
#include "PostProcessing.h"
#include "PostProcessingGaussBlur.h"
#include "PostProcessingDownsample.h"
#include "PostProcessingBloomSetup.h"
#include "PostProcessingCombinePass.h"
#include "PostProcessingFXAA.h"
#include "PostProcessingStaticMotionBlur.h"

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
			
			return a.length == 0 || memcmp(a.pData, b.pData, a.length) == 0;
		}

		bool operator != (const PostProcessing::EffectDesc& a, const PostProcessing::EffectDesc& b)
		{
			if (a.name != b.name || a.length != b.length)
				return true;

			return a.length != 0 && memcmp(a.pData, b.pData, a.length) != 0;
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

//			BloomDesc* desc = new BloomDesc{ m_Parameters.BloomThreshold, m_Parameters.ExposureScale, m_Parameters.BloomIntensity };
//			m_CurrentEffects.insert(EffectDesc{ PostProcessingEffect::BLOOM, sizeof(BloomDesc), desc});
			m_CurrentEffects.insert(EffectDesc{ PostProcessingEffect::BLOOM, 0, 0 });
		}

		void PostProcessing::addFXAA(uint32_t quality)
		{
			m_Parameters.FXAA_Quality = Numeric::min(6U, quality);
			m_CurrentEffects.insert(EffectDesc{ PostProcessingEffect::FXAA, 0, 0 });
		}

		void PostProcessing::addStaticMotionBlur(glm::mat4 previouseViewProj, glm::mat4 viewProjInverse)
		{
			m_Parameters.PreviouseViewProjection = previouseViewProj;
			m_Parameters.ViewProjectionInverse = viewProjInverse;
			m_CurrentEffects.insert(EffectDesc{ PostProcessingEffect::STATIC_MOTION_BLUR, 0, 0 });
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

			if (!m_AddedRenderPasses.empty())
			{
				Texture2D* src = m_AddedRenderPasses.back().get()->getOutput(0);
//				printf("renderTo src Name: %d\n", src->getTexture());
				m_RenderContext->renderTo(src, frameAttribs.OutputBuffer, frameAttribs.Viewport);
				m_RenderContext->finalize();
			}
			else
			{
				m_RenderContext->renderTo(frameAttribs.SceneColorBuffer, frameAttribs.OutputBuffer, frameAttribs.Viewport);
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

		PostProcessingFXAA* PostProcessing::createFXAAPass(uint32_t quality)
		{
			return new PostProcessingFXAA(quality);
		}

		PostProcessingStaticMotionBlur* PostProcessing::createStaticMotionBlurPass(uint32_t inputCount)
		{
			return new PostProcessingStaticMotionBlur(inputCount);
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
			PostProcessingBloomSetup::shutDown();
			PostProcessingCombinePass::shutDown();
			PostProcessingFXAA::shutDown();
			PostProcessingStaticMotionBlur::shutDown();
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

			bool TestPassRepeatBuild = false;
//			if (m_CurrentEffects != m_PrevEffects)
			if (!Numeric::isEqual(m_CurrentEffects, m_PrevEffects) || TestPassRepeatBuild)
			{
				typedef std::shared_ptr<PPRenderPass> RenderPass;
				printf("Reconstruct the Pass List\n");
				checkEffectBits();
				reset();
				m_AddedRenderPasses.clear();

				RenderPass last_color_buffer;
				RenderPass sceneDepthPass;
				// add the color and depth buffer first.
				RenderPass sceneColorPass = RenderPass(new PPRenderPassInput(frameAttribs.SceneColorBuffer));
				m_AddedRenderPasses.push_back(sceneColorPass);
				last_color_buffer = sceneColorPass;

				const bool haveMotionBlur = isStaticMotionBlurEnabled();

				if (haveMotionBlur && frameAttribs.SceneDepthBuffer)
				{
					sceneDepthPass = RenderPass(new PPRenderPassInput(frameAttribs.SceneDepthBuffer, static_cast<uint32_t>(INPUT_DEPTH - INPUT_COLOR0)));
					m_AddedRenderPasses.push_back(sceneDepthPass);
				}
				
				if (haveMotionBlur && (frameAttribs.ColorDepthCombined || frameAttribs.SceneDepthBuffer != NULL))
				{
					RenderPass staticMotionBlurPass = RenderPass(createStaticMotionBlurPass(frameAttribs.ColorDepthCombined ? 1 : 2));
					staticMotionBlurPass->setDependency(0, last_color_buffer->getName(), 0);
					if (!frameAttribs.ColorDepthCombined)
					{
						staticMotionBlurPass->setDependency(1, sceneDepthPass->getName(), 0);
					}
					m_AddedRenderPasses.push_back(staticMotionBlurPass);

					last_color_buffer = staticMotionBlurPass;
				}

				const bool haveBloomEffect = isBloomEnabled();

				if(haveBloomEffect)
				{// Add Bloom Effect
					// first the Add the downsample Pass.
					RenderPass downsamplePass = RenderPass(createDownsamplePass(DownsampleMethod::NORMAL));
					downsamplePass->setDependency(0, last_color_buffer->getName(), 0);
					m_AddedRenderPasses.push_back(downsamplePass);
//					
					
					RenderPass bloomSetupPass = RenderPass(createBloomSetupPass());
					bloomSetupPass->setDependency(0, downsamplePass->getName(), 0);
					m_AddedRenderPasses.push_back(bloomSetupPass);
					
					
					RenderPass gaussionBlurPass = RenderPass(createGaussionBlurPass());
					gaussionBlurPass->setDependency(0, bloomSetupPass->getName(), 0);
					m_AddedRenderPasses.push_back(gaussionBlurPass);

					
					RenderPass combinePass = RenderPass(createCombinePass());
					combinePass->setDependency(0, last_color_buffer->getName(), 0);
					combinePass->setDependency(1, gaussionBlurPass->getName(), 0);
					m_AddedRenderPasses.push_back(combinePass);

					last_color_buffer = combinePass;
				}

				// Add FXAA Pass.
				if (isFXAAEnabled() && m_Parameters.FXAA_Quality)
				{
					RenderPass fxaaPass = RenderPass(createFXAAPass(m_Parameters.FXAA_Quality - 1));
					fxaaPass->setDependency(0, last_color_buffer->getName(), 0);
					m_AddedRenderPasses.push_back(fxaaPass);

					last_color_buffer = fxaaPass;
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
							passDenpendencyCount[desc.InputDescs[j].DependencyPass] ++;
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

				std::vector<EffectDesc> list = std::vector<EffectDesc>(m_PrevEffects.begin(), m_PrevEffects.end());
				for (auto it = list.begin(); it != list.end(); it++)
				{
					it->release();
				}
				m_PrevEffects.clear();
				m_PrevEffects.insert(m_CurrentEffects.begin(), m_CurrentEffects.end());
				m_CurrentEffects.clear();
			}
			else
			{
				// release the memory allocated from addXXXEffect
				std::vector<EffectDesc> list = std::vector<EffectDesc>(m_CurrentEffects.begin(), m_CurrentEffects.end());
				
				for (auto it = list.begin(); it != list.end(); it++)
				{
					it->release();
				}
				m_CurrentEffects.clear();
			}
		}
	}
}
