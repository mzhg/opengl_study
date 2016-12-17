
#pragma once
#include "BaseApp.h"
#include "PostProcessing.h"
#include "SampleColorTriangle.h"
#include "SampleTunnel.h"

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
	int mWidth;
	int mHeight;

	PostProcessing* m_PostProcessing;
	SampleApp* m_Sample;
};