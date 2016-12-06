
#pragma once
#include "BaseApp.h"
#include "PostProcessing.h"

using namespace jet::util;

class HeightmapDemo : public BaseApp
{
public:


	void onCreate();
	void onResize(GLuint width, GLuint height);
	void onRender();

	void viewportTest();
	void onDispose();

private:
	void CreateAnimatedTrianglesGeometry();
	void DrawAnimatedTriangles(double deltaSeconds, bool messWithTheGraph);

private:
	int mWidth;
	int mHeight;

	PostProcessing* m_PostProcessing;

	GLSLProgram* m_AnimatedTrianglesProgram;
	GLuint m_ATVAO;
	GLuint m_ATVAOVertices;

	GLSLProgram* m_FullScreenQuad;
	Texture2D* mpTestTexture;

	GLuint	m_FBO;
	GLuint  m_ScreenColorTexture;
	Texture2D* m_SceneColorBuffer;
};