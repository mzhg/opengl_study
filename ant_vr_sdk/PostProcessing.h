
#ifndef _JET_UTIL_POSTPROCESSING_H
#define _JET_UTIL_POSTPROCESSING_H

#include "Numeric.h"
#include "PostProcessingFramework.h"
#include <set>

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

			FrameAttribs() : Viewport(), ClipRect(), SceneColorBuffer(NULL), SceneDepthBuffer(NULL), OutputBuffer(NULL){}
			FrameAttribs(PPRectangle viewport, PPRectangle clipRect, Texture2D* sceneColor, Texture2D* sceneDepth, Texture2D* output) :
				Viewport(viewport), ClipRect(clipRect), SceneColorBuffer(sceneColor), SceneDepthBuffer(sceneDepth), OutputBuffer(output)
			{}
		};

		enum class PostProcessingEffect
		{
			BLOOM,
			COUNT
		};

		class PostProcessing
		{

		public:
			PostProcessing() :m_RenderContext(NULL){}
			~PostProcessing();

			void addBloom(float bloomThreshold = 0.25f, float exposureScale = 1.02f, float bloomIntensity = 1.12f);
			void addGaussBlur(int kernal);

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

			class PostProcessGaussBlur* createGaussionBlurPass();
			class PostProcessingBloomSetup* createBloomSetupPass();
			class PostProcessingCombinePass* createCombinePass();
			class PostProcessingDownsample* createDownsamplePass(DownsampleMethod method);

			void reset()
			{
				m_iDownsamplePassCount = 0;
				m_iGaussionBlurPassCount = 0;
				m_iBloomSetupPassCount = 0;
				m_iCombinePassCount = 0;
			}

		private:
			struct EffectDesc
			{
				PostProcessingEffect name;
				uint32_t length;  // The length of data
				void* pData;

				~EffectDesc()
				{
					delete pData;
				}
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