
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

			void performancePostProcessing(const FrameAttribs& frameAttribs);

			void shutDown();

		private:
			PostProcessing(PostProcessing&) = delete;
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
			bool isFXAAEnabled()  const { return m_bEffectBits[static_cast<int>(PostProcessingEffect::FXAA)]; }
			bool isStaticMotionBlurEnabled() const { return m_bEffectBits[static_cast<int>(PostProcessingEffect::STATIC_MOTION_BLUR)]; }

			class PostProcessGaussBlur* createGaussionBlurPass();
			class PostProcessingBloomSetup* createBloomSetupPass();
			class PostProcessingCombinePass* createCombinePass();
			class PostProcessingDownsample* createDownsamplePass(DownsampleMethod method);
			class PostProcessingFXAA* createFXAAPass(uint32_t quality);
			class PostProcessingStaticMotionBlur* createStaticMotionBlurPass(uint32_t inputCount);

			void reset()
			{
				m_iDownsamplePassCount = 0;
				m_iGaussionBlurPassCount = 0;
				m_iBloomSetupPassCount = 0;
				m_iCombinePassCount = 0;
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

			uint32_t m_iDownsamplePassCount;
			uint32_t m_iGaussionBlurPassCount;
			uint32_t m_iBloomSetupPassCount;
			uint32_t m_iCombinePassCount;
		};
	}
}

#endif