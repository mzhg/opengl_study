#pragma once
#include "Node.h"
#include "SampleApp.h"
#include "GLSLProgram.h"

namespace jet
{
	namespace util
	{
		class LegacyApplication : public SampleApp
		{
		public:

			void start(uint32_t width = 1280, uint32_t height = 720);
			LegacyApplication();
			virtual ~LegacyApplication();

		protected:
			// Called When sample created 
			void OnCreate() override;
			// Called When the viewport changed!
			void OnResize(int x, int y, int width, int height) override;
			// Render Loop...
			void OnRender(float elpsedTime = 0.0) override;

			virtual void initialize() = 0;
			virtual void update(float elpsedTime = 0.0) = 0;

		private:
			void renderNode(Node* pNode);

		protected:
			Node* m_pRoot;

			GLSLProgram* m_pProgram;
			GLuint m_ArrayBuffer;
			GLuint m_ElementBuffer;
		};
	}
}
