
#ifndef _JET_UTIL_POSTPROCESSING_H
#define _JET_UTIL_POSTPROCESSING_H

#include "Numeric.h"
#include "PostProcessingFramework.h"
#include <set>

namespace jet
{
	namespace util
	{
		struct FrameAttribs
		{
			PPRectangle Viewport;
			PPRectangle ClipRect;
			Texture2D* SceneColorBuffer;
			Texture2D* SceneDepthBuffer;
			
			// The Output destination, if NULL, will output to the default framebuffer.
			Texture2D* OutputBuffer;
			bool ColorDepthCombined;

			FrameAttribs() : Viewport(), ClipRect(), SceneColorBuffer(NULL), SceneDepthBuffer(NULL), OutputBuffer(NULL), ColorDepthCombined(false){}
			FrameAttribs(PPRectangle viewport, PPRectangle clipRect, Texture2D* sceneColor, Texture2D* sceneDepth, Texture2D* output, bool colorDepthCombined) :
				Viewport(viewport), ClipRect(clipRect), SceneColorBuffer(sceneColor), SceneDepthBuffer(sceneDepth), OutputBuffer(output), ColorDepthCombined(colorDepthCombined)
			{}
		};

		enum class PostProcessingEffect
		{
			BLOOM,
			LENS_FLARE,
			LIGHT_STREAKER,
			HDR,
			FXAA,
			STATIC_MOTION_BLUR,
			COUNT
		};

		class PostProcessing
		{

		public:
			PostProcessing() :m_RenderContext(NULL){}
			~PostProcessing();

			void addBloom(float bloomThreshold = 0.25f, float exposureScale = 1.02f, float bloomIntensity = 1.12f);
			// Add the FXAA Antialised, quanlity ranged [0, 6], 0 means off, others are the quality.
			void addFXAA(uint32_t quality);

			void addStaticMotionBlur(glm::mat4 previouseViewProj, glm::mat4 viewProjInverse);
			void addLensFlare(Texture2D* pFlareTexture = nullptr);
			void addLightStreaker(float lumThreshold = 0.97f, int startGenLevel = 0);
			void addHDR(bool, float explosure = 1.4f, float gamma = 1.0f / 1.8f);

			void performancePostProcessing(const FrameAttribs& frameAttribs);

			void shutDown();

		private:
			PostProcessing(PostProcessing&) = delete;
			PostProcessing& operator=(PostProcessing&) = delete;

			void prepare(const FrameAttribs& frameAttribs);
			void checkEffectBits()
			{
				memset(m_bEffectBits, 0, sizeof(m_bEffectBits));
				int first = static_cast<int>(PostProcessingEffect::BLOOM);
				for (auto it = m_CurrentEffects.begin(); it != m_CurrentEffects.end(); it++)
				{
					int idx = static_cast<int>(it->name) -first;
					m_bEffectBits[idx] = true;
				}
			}

			bool isBloomEnabled() const { return m_bEffectBits[static_cast<int>(PostProcessingEffect::BLOOM)]; }
			bool isLensFlareEnabed() const { return m_bEffectBits[static_cast<int>(PostProcessingEffect::LENS_FLARE)]; }
			bool isLightStreakerEnabed() const { return m_bEffectBits[static_cast<int>(PostProcessingEffect::LIGHT_STREAKER)]; }
			bool isHDREnabed() const { return m_bEffectBits[static_cast<int>(PostProcessingEffect::HDR)]; }
			bool isFXAAEnabled()  const { return m_bEffectBits[static_cast<int>(PostProcessingEffect::FXAA)]; }
			bool isStaticMotionBlurEnabled() const { return m_bEffectBits[static_cast<int>(PostProcessingEffect::STATIC_MOTION_BLUR)]; }

			typedef std::shared_ptr<PPRenderPass> RenderPass;
			RenderPass addBloomPass(RenderPass inputColor);
			RenderPass addLightEffectPass(RenderPass inputColor, uint32_t inputWidth, uint32_t inputHeight, RenderPass& eyeAdaption);

			class PostProcessingCalculateLumiance* createCalculateLumiancePass();
			class PostProcessGaussBlur* createGaussionBlurPass(uint32_t width = 0, uint32_t height = 0);
			class PostProcessingBloomSetup* createBloomSetupPass(bool useLight = false, uint32_t width = 0, uint32_t height = 0);
			class PostProcessingCombinePass* createCombinePass(uint32_t width = 0, uint32_t height = 0);
			class PostProcessingDownsample* createDownsamplePass(DownsampleMethod method, uint32_t sampleCount = 1u, uint32_t width = 0, uint32_t height = 0);
			class PostProcessingFXAA* createFXAAPass(uint32_t quality, uint32_t width = 0, uint32_t height = 0);
			class PostProcessingStaticMotionBlur* createStaticMotionBlurPass(uint32_t inputCount);

			class PostProcessingGhostImageSetup* createGhostImageSetupPass(uint32_t width = 0, uint32_t height = 0);
			class PostProcessingGlareCompose*    createGlareComposePass(glm::vec4 mixCoeff, uint32_t width = 0, uint32_t height = 0);
			class PostProcessingLightStreaker*   createLightStreakerPass(uint32_t width = 0, uint32_t height = 0);
			class PostProcessingTonemap*         createTonemapPass(GLuint width = 0, GLuint height = 0);

			void reset()
			{
				m_iDownsamplePassCount = 0;
				m_iGaussionBlurPassCount = 0;
				m_iBloomSetupPassCount = 0;
				m_iCombinePassCount = 0;
				m_iGhostImageCount = 0;
				m_iGlareComposeCount = 0;
				m_iLightStreakerCount = 0;

				m_bFXAAEnabled = false;
				m_bStaticMotionEnabled = false;
				m_bEyeAdapterEnabled = false;
				m_bTonemapEnabled = false;
			}

			void addGaussBlur(int kernal);
		private:
			struct EffectDesc
			{
				PostProcessingEffect name;
				uint32_t length;  // The length of data
				void* pData;

				void release() { if (length > 0) delete pData; }
			};

			friend bool operator < (const EffectDesc& a, const EffectDesc& b);
			friend bool operator == (const EffectDesc& a, const EffectDesc& b);
			friend bool operator != (const EffectDesc& a, const EffectDesc& b);

		private:
			std::set<EffectDesc> m_CurrentEffects;
			std::set<EffectDesc> m_PrevEffects;

			std::vector<std::shared_ptr<PPRenderPass>> m_AddedRenderPasses;
			PostProcessingParameters m_Parameters;

			bool m_bEffectBits[static_cast<int>(PostProcessingEffect::COUNT)];
			PPRenderContext* m_RenderContext;

			uint8_t m_iDownsamplePassCount;
			uint8_t m_iGaussionBlurPassCount;
			uint8_t m_iBloomSetupPassCount;
			uint8_t m_iCombinePassCount;
			uint8_t m_iGhostImageCount;
			uint8_t m_iGlareComposeCount;
			uint8_t m_iLightStreakerCount;
			
			bool    m_bFXAAEnabled;
			bool    m_bStaticMotionEnabled;
			bool    m_bEyeAdapterEnabled;
			bool    m_bTonemapEnabled;
		};
	}
}

#endif