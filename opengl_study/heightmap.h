
#pragma once
#include "BaseApp.h"

class HeightmapDemo : public jet::util::BaseApp
{
public:


	void onCreate();
	void onResize(GLuint width, GLuint height);
	void onRender();
	void onDispose();

private:
	int mWidth;
	int mHeight;
};