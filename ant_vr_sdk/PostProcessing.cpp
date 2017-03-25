
#include "PostProcessing.h"
#include "PostProcessingGaussBlur.h"
#include "PostProcessingDownsample.h"
#include "PostProcessingBloomSetup.h"
#include "PostProcessingCombinePass.h"
#include "PostProcessingFXAA.h"
#include "PostProcessingStaticMotionBlur.h"
#include "PostProcessingCalculateLumiance.h"
#include "PostProcessingGhostImageSetup.h"
#include "PostProcessingGlareCompose.h"
#include "PostProcessingLightStreaker.h"
#include "PostProcessingTonemap.h"

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

		void PostProcessing::addLensFlare(Texture2D* pFlareTexture)
		{
			m_Parameters.pLensFlareTexture = pFlareTexture;
			m_CurrentEffects.insert(EffectDesc{ PostProcessingEffect::LENS_FLARE, 0, 0 });
		}

		void PostProcessing::addLightStreaker(float lumThreshold , int startGenLevel)
		{
			// TODO params.
			m_CurrentEffects.insert(EffectDesc{ PostProcessingEffect::LIGHT_STREAKER, 0, 0 });
		}

		void PostProcessing::addHDR(bool autoExpose, float explosure, float gamma)
		{
			// TODO params.
			m_CurrentEffects.insert(EffectDesc{ PostProcessingEffect::HDR, 0, 0 });
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
			m_RenderContext->performancePostProcessing(frameAttribs.Viewport);
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

		PostProcessingCalculateLumiance* PostProcessing::createCalculateLumiancePass()
		{
			assert(m_bEyeAdapterEnabled == false);
			m_bEyeAdapterEnabled = true;
			return new PostProcessingCalculateLumiance();
		}

		PostProcessGaussBlur* PostProcessing::createGaussionBlurPass(uint32_t width, uint32_t height)
		{
			assert(m_iGaussionBlurPassCount < 100);
			return new PostProcessGaussBlur(m_iGaussionBlurPassCount++, width, height);
		}

		PostProcessingBloomSetup* PostProcessing::createBloomSetupPass(bool useLight, uint32_t width, uint32_t height)
		{
			assert(m_iBloomSetupPassCount < 100);
			return new PostProcessingBloomSetup(m_iBloomSetupPassCount++, useLight, width, height);
		}

		PostProcessingCombinePass* PostProcessing::createCombinePass(uint32_t width, uint32_t height)
		{
			assert(m_iCombinePassCount < 100);
			return new PostProcessingCombinePass(m_iCombinePassCount++, width, height);
		}
		
		PostProcessingDownsample* PostProcessing::createDownsamplePass(DownsampleMethod method, uint32_t sampleCount, uint32_t width, uint32_t height)
		{
			assert(m_iDownsamplePassCount < 100);
			return new PostProcessingDownsample(method, m_iDownsamplePassCount++, width, height);
		}

		PostProcessingFXAA* PostProcessing::createFXAAPass(uint32_t quality, uint32_t width, uint32_t height)
		{
			assert(m_bFXAAEnabled == false);
			m_bFXAAEnabled = true;
			return new PostProcessingFXAA(quality, width, height);
		}

		PostProcessingGhostImageSetup* PostProcessing::createGhostImageSetupPass(uint32_t width, uint32_t height)
		{
			assert(m_iGhostImageCount < 100);
			return new PostProcessingGhostImageSetup(m_iGhostImageCount++, width, height);
		}

		PostProcessingGlareCompose*    PostProcessing::createGlareComposePass(glm::vec4 mixCoeff, uint32_t width, uint32_t height)
		{
			assert(m_iGlareComposeCount < 100);
			return new PostProcessingGlareCompose(mixCoeff, m_iGlareComposeCount++, width, height);
		}

		PostProcessingLightStreaker*   PostProcessing::createLightStreakerPass(uint32_t width, uint32_t height)
		{
			assert(m_iLightStreakerCount < 100);
			return new PostProcessingLightStreaker(m_iLightStreakerCount++, width, height);
		}

		PostProcessingTonemap*         PostProcessing::createTonemapPass(GLuint width, GLuint height)
		{
			assert(!m_bTonemapEnabled);
			m_bTonemapEnabled = true;
			return new PostProcessingTonemap(width, height);
		}

		PostProcessingStaticMotionBlur* PostProcessing::createStaticMotionBlurPass(uint32_t inputCount)
		{
			assert(m_bStaticMotionEnabled == false);
			m_bStaticMotionEnabled = true;
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
			PostProcessingCalculateLumiance::shutDown();
			PostProcessingGhostImageSetup::shutDown();
			PostProcessingGlareCompose::shutDown();
			PostProcessingLightStreaker::shutDown();
			PostProcessingTonemap::shutDown();
		}

		PostProcessing::RenderPass PostProcessing::addBloomPass(RenderPass last_color_buffer)
		{
			const bool haveBloomEffect = isBloomEnabled();

			if (haveBloomEffect)
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

				last_color_buffer =  combinePass;
			}

			return last_color_buffer;
		}

		PostProcessing::RenderPass PostProcessing::addLightEffectPass(RenderPass last_color_buffer, uint32_t inputWidth, uint32_t inputHeight, RenderPass& eyeAdaption)
		{
			const bool haveLenFlare = isLensFlareEnabed();
			const bool haveLightStreaker = isLightStreakerEnabed();
			const bool haveHDR = isHDREnabed();

			const uint32_t postProcessingWidth = 1024U;
			const uint32_t postProcessingHeight = 1024U;

			if (haveLenFlare || haveLightStreaker)
			{
				// first the Add the downsample Pass.
				RenderPass downsamplePass4x = RenderPass(createDownsamplePass(DownsampleMethod::NORMAL, 2, postProcessingWidth / 4, postProcessingHeight/4));
				downsamplePass4x->setDependency(0, last_color_buffer->getName(), 0);
				m_AddedRenderPasses.push_back(downsamplePass4x);

				// calculate the averige lumiance of the scene color texture.
				if (haveHDR && m_Parameters.AutoExposure)
				{
					RenderPass lumianceInput = downsamplePass4x;

					uint32_t width = postProcessingWidth/4;
					uint32_t height = postProcessingHeight/4;

					while (width > 16 && height > 16)
					{
						RenderPass downsample4x = RenderPass(createDownsamplePass(DownsampleMethod::NORMAL, 2));
						downsample4x->setDependency(0, lumianceInput->getName(), 0);
						m_AddedRenderPasses.push_back(downsample4x);

						lumianceInput = downsample4x;
						
						width /= 4;
						height /= 4;
					}

					eyeAdaption = RenderPass(createCalculateLumiancePass());
					eyeAdaption->setDependency(0, lumianceInput->getName(), 0);
					m_AddedRenderPasses.push_back(eyeAdaption);
				}

				//Extract high light area for further processing.
				RenderPass bloomSetupPass = RenderPass(createBloomSetupPass(true));  // TODO still use the bloom threshold.
				bloomSetupPass->setDependency(0, downsamplePass4x->getName(), 0);
				m_AddedRenderPasses.push_back(bloomSetupPass);

				RenderPass blurPass0 = RenderPass(createGaussionBlurPass()); // TODO need specify the kernel size.
				blurPass0->setDependency(0, bloomSetupPass->getName(), 0);
				m_AddedRenderPasses.push_back(blurPass0);

				const int BLUR_COUNT = 4;
				RenderPass composePasses[BLUR_COUNT];
				RenderPass blurPasses[BLUR_COUNT];

				composePasses[0] = bloomSetupPass;
				blurPasses[0] = blurPass0;
				for (int i = 1; i < BLUR_COUNT; i++)
				{
					composePasses[i] = RenderPass(createDownsamplePass(DownsampleMethod::FASTEST));
					composePasses[i]->setDependency(0, composePasses[i-1]->getName(), 0);
					m_AddedRenderPasses.push_back(composePasses[i]);

					blurPasses[i] = RenderPass(createGaussionBlurPass());
					blurPasses[i]->setDependency(0, composePasses[i]->getName(), 0);
					m_AddedRenderPasses.push_back(blurPasses[i]);
				}

				RenderPass lightStreakerPass;
				if (haveLightStreaker)
				{
					lightStreakerPass = RenderPass(createLightStreakerPass(postProcessingWidth / 4, postProcessingHeight/4));
					lightStreakerPass->setDependency(0, composePasses[0]->getName(), 0);
					m_AddedRenderPasses.push_back(lightStreakerPass);
				}

				RenderPass ghostImagePass;
				if (haveLenFlare)
				{
					// TODO generate the ghost images for lens flare.
					ghostImagePass = RenderPass(createGhostImageSetupPass(postProcessingWidth / 2, postProcessingHeight / 2));
					ghostImagePass->setDependency(0, blurPasses[0]->getName(), 0);
					ghostImagePass->setDependency(1, blurPasses[1]->getName(), 0);

					RenderPass lensTexture = RenderPass(new PPRenderPassInput(m_Parameters.pLensFlareTexture, 1));
					ghostImagePass->setDependency(2, lensTexture->getName(), 0);
					m_AddedRenderPasses.push_back(ghostImagePass);
				}

				// TODO Compose the blur buffer.
				RenderPass blurComposePass = RenderPass(createGlareComposePass(glm::vec4(0.3f, 0.3f, 0.25f, 0.20f), postProcessingWidth / 4, postProcessingHeight / 4));
				blurComposePass->setDependency(0, blurPasses[0]->getName(), 0);
				blurComposePass->setDependency(1, blurPasses[1]->getName(), 0);
				blurComposePass->setDependency(2, blurPasses[2]->getName(), 0);
				blurComposePass->setDependency(3, blurPasses[3]->getName(), 0);
				m_AddedRenderPasses.push_back(blurComposePass);


				const glm::vec4 cameraMixCoeff = { 1.2f, 0.8f, 0.1f, 0.0f };
				const glm::vec4 filmicMixCoeff = { 0.6f, 0.55f, 0.08f, 0.0f };
				// Galre buffer
				const glm::vec4& mixCoeff = (m_Parameters.getLightStreakerStyle() == GlareType::CAMERA) ? cameraMixCoeff : filmicMixCoeff;
				RenderPass glareComposePass = RenderPass(createGlareComposePass(mixCoeff, postProcessingWidth / 2, postProcessingHeight / 2));
				glareComposePass->setDependency(0, blurComposePass->getName(), 0);
				glareComposePass->setDependency(1, lightStreakerPass.get() ? lightStreakerPass->getName() : INPUT_NONE, 0);
				glareComposePass->setDependency(2, ghostImagePass.get() ? ghostImagePass->getName() : INPUT_NONE, 0);
				glareComposePass->setDependency(3, INPUT_NONE, 0);
				m_AddedRenderPasses.push_back(glareComposePass);

				last_color_buffer = glareComposePass;
			}

			return last_color_buffer;
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

				last_color_buffer = addBloomPass(last_color_buffer);

				RenderPass eyeAdaptionPass;
				last_color_buffer = addLightEffectPass(last_color_buffer, frameAttribs.SceneColorBuffer->getWidth(), 
					frameAttribs.SceneColorBuffer->getHeight(),eyeAdaptionPass);

				// Tone map
				if (isHDREnabed())
				{
					RenderPass tonemapPass = RenderPass(createTonemapPass(frameAttribs.SceneColorBuffer->getWidth(), frameAttribs.SceneColorBuffer->getHeight()));
					tonemapPass->setDependency(0, sceneColorPass->getName(), 0);
					tonemapPass->setDependency(1, last_color_buffer->getName(), 0);
					tonemapPass->setDependency(2, eyeAdaptionPass.get() ? eyeAdaptionPass->getName():0, 0);
					m_AddedRenderPasses.push_back(tonemapPass);
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
