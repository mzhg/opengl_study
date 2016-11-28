
#include "heightmap.h"

typedef struct Rect
{
	int left, bottom;
	int right, top;
}Rect;


void HeightmapDemo::onCreate()
{
	
	const GLubyte* render = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("Render: %s\n", render);
	printf("Version: %s\n", version);
	printf("GLSLVersion: %s\n", glslVersion);
	
}

void HeightmapDemo::onResize(GLuint width, GLuint height)
{
	if (width == 0 || height == 0)
		return;

	mWidth = width;
	mHeight = height;
	glViewport(0, 0, width, height);
}

void renderTriangle()
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_TRIANGLES);
	{
		glColor3f(1.0, 0.0, 0.0);
		glVertex2f(-1.0, -1.0);

		glColor3f(0.0, 1.0, 0.0);
		glVertex2f(1.0, -1.0);

		glColor3f(0.0, 0.0, 1.0);
		glVertex2f(0.0, 1.0);
	}

	glGetIntegerv(GL_VIEWPORT, nullptr);
	glEnd();
}

void HeightmapDemo::onRender()
{
	glClearColor(1, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	GLint viewport_[4];
	Rect viewport;
	glGetIntegerv(GL_VIEWPORT, &viewport_[0]);

//	printf("--- Graphics::RenderFrame:: Viewport(%d, %d, %d, %d)\n", viewport_[0], viewport_[1], viewport_[2], viewport_[3]);

	viewport.left = viewport_[0];
	viewport.right = viewport_[0] + viewport_[2] / 2;
	viewport.bottom = viewport_[1];
	viewport.top = viewport_[1] + viewport_[3];

	glViewport(viewport.left, viewport.bottom,
		viewport.right - viewport.left,
		viewport.top - viewport.bottom);
//	printf("--- Graphics::RenderFrame:: Left Eyey Viewport(%d, %d, %d, %d)\n", viewport.left, viewport.bottom, viewport.right, viewport.top);
	renderTriangle();
	
	viewport.left = viewport_[0] + viewport_[2] / 2;
	viewport.right = viewport_[2];
	viewport.bottom = viewport_[1];
	viewport.top = viewport_[1] + viewport_[3];

	glViewport(viewport.left, viewport.bottom,
		viewport.right - viewport.left,
		viewport.top - viewport.bottom);
//	printf("--- Graphics::RenderFrame:: Right Eyey Viewport(%d, %d, %d, %d)\n", viewport.left, viewport.bottom, viewport.right, viewport.top);

	renderTriangle();

	glViewport(viewport_[0], viewport_[1], viewport_[2], viewport_[3]);
}

void HeightmapDemo::onDispose(){
	printf("onDispose called!!!\n");
}

int main(int argc, char** argv)
{
	HeightmapDemo demo;
	demo.getConfig().IsOpenGLESContext = false;
	jet::util::BaseApp::Run(&demo, "Triangles");
	return 0;
}