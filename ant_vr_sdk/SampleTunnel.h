#pragma once
#include "SampleApp.h"
#include "GLSLProgram.h"

namespace jet
{
	namespace util
	{
		class SampleTunnel : public SampleApp
		{
		public:
			SampleTunnel();
			~SampleTunnel();

		public:
			void getRenderInfo(RenderInfo& info) override
			{
				info.ViewMat = m_ViewMat;
				info.ProjMat = m_ProjMat;
			}
		protected:
			// Called When sample created 
			void OnCreate() override;
			// Called When the viewport changed!
			void OnResize(int x, int y, int width, int height) override;
			// Render Loop...
			void OnRender(float elpsedTime = 0.0) override;
		private:
			GLuint m_VAO;
			GLuint m_VBO;
			GLuint m_IBO;

			Texture2D* m_FloorTex;
			Texture2D* m_WallTex;
			Texture2D* m_CeilingTex;
			GLSLProgram* m_SceneProgram;

			GLuint m_TexSample;
			float m_Time;
			glm::mat4 m_ViewMat;
			glm::mat4 m_ProjMat;
		};
	}
}

