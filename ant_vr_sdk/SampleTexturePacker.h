#pragma once
#include "SampleApp.h"
#include "TexturePacker.h"
#include "GLSLProgram.h"
#include "BufferGL.h"

namespace jet
{
	namespace util
	{
		class SampleTexturePacker : public SampleApp
		{
		public:
			SampleTexturePacker();
			~SampleTexturePacker();

		protected:
			// Called When sample created 
			void OnCreate() override;
			// Called When the viewport changed!
			void OnResize(int x, int y, int width, int height) override;
			// Render Loop...
			void OnRender(float elpsedTime = 0.0) override;
		private:
			ArrayBufferGL* m_ScreenQuadBuffer;
			VertexArrayGL* m_ScreenQuadVAO;

			GLSLProgram* m_FullScreenQuad;
			std::map<std::string, Texture2D*> m_TestTextureList;
			TextureAtlas* m_TextureAltas;
		};
	}
}

