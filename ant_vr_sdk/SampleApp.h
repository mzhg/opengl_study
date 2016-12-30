#pragma once

#include "Texture.h"
#include <glm.hpp>
namespace jet
{
	namespace util
	{
		struct SampleOutputDesc
		{
			unsigned int ColorCount;
			int ColorFormats[8];

			int DepthStencilFormat;
			bool StencilOnly;
			bool EnableSRGB;
		};

		struct RenderInfo
		{
			glm::mat4 ViewMat;
			glm::mat4 ProjMat;
		};
		class SampleApp
		{
		public:
			SampleApp();
			virtual ~SampleApp();

		public:
			void Create();

			void Resize(int x, int y, int width, int height);
			void Render(bool renderToFBO = true, float elpsedTime = 0.0);

			SampleOutputDesc getOutputDesc() const { return mOutputDesc; }
			const Texture2D* getColorTexture(unsigned idx) const 
			{
				if (idx >= mOutputDesc.ColorCount)
				{
					return nullptr;
				}
				else
				{
					return m_ColorTextures[idx];
				}
			}

			const Texture2D* getDepthlTexture() const { return m_DepthTexture; }
			const Texture2D* getStencilTexture() const { return m_StencilTexture; }
			virtual void getRenderInfo(RenderInfo& info)
			{
				info.ViewMat = glm::mat4();
				info.ProjMat = glm::mat4();
			}
		protected:

			// Called When sample created 
			virtual void OnCreate() = 0;
			// Called When the viewport changed!
			virtual void OnResize(int x, int y, int width, int height) = 0;
			// Render Loop...
			virtual void OnRender(float elpsedTime = 0.0) = 0;

			virtual void InitConfig(SampleOutputDesc& desc);
		protected:
			int X;
			int Y;
			int Width;
			int Height;

			SampleOutputDesc mOutputDesc;

		private:
			// internal variables
			unsigned int m_Framebuffer;
			Texture2D* m_ColorTextures[8];
			Texture2D* m_DepthTexture;
			Texture2D* m_StencilTexture;
		};
	}
}

