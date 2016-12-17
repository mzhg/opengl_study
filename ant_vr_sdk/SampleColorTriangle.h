#pragma once

#include "SampleApp.h"
#include "GLSLProgram.h"

namespace jet
{
	namespace util
	{
		class SampleColorTriangle : public SampleApp
		{
		public:
			SampleColorTriangle();
			~SampleColorTriangle();

		protected:
			// Called When sample created 
			void OnCreate() override;
			// Called When the viewport changed!
			void OnResize(int x, int y, int width, int height) override;
			// Render Loop...
			void OnRender(float elpsedTime = 0.0) override;
		private:

			void DrawAnimatedTriangles(double deltaSeconds, bool messWithTheGraph);
			GLSLProgram* m_AnimatedTrianglesProgram;
			GLuint m_ATVAO;
			GLuint m_ATVAOVertices;

			GLSLProgram* m_FullScreenQuad;
			Texture2D* mpTestTexture;
		};
	}
}

