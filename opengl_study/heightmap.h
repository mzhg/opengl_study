
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
	void onDispose();

private:
	int mWidth;
	int mHeight;

	PostProcessing* m_PostProcessing;
};