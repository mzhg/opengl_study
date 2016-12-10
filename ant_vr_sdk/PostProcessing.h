
#ifndef _JET_UTIL_POSTPROCESSING_H
#define _JET_UTIL_POSTPROCESSING_H

#include "Numeric.h"
#include "PostProcessingFramework.h"
#include <set>

#include "PostProcessingGaussBlur.h"
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

		class PostProcessing
		{

		public:
			PostProcessing() :m_RenderContext(NULL){}
			~PostProcessing();

			void addGaussBlur(int kernal);

			void performancePostProcessing(const FrameAttribs& frameAttribs);

			void shutDown();

		private:
			void prepare(const FrameAttribs& frameAttribs);

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