
#include "heightmap.h"

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

	glViewport(0, 0, width, height);
}

void HeightmapDemo::onRender()
{
	glClearColor(1, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

	/*
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

	glEnd();
	*/
}

void HeightmapDemo::onDispose(){
	printf("onDispose called!!!\n");
}

int main(int argc, char** argv)
{
	HeightmapDemo demo;
	demo.getConfig().IsOpenGLESContext = true;
	jet::util::BaseApp::Run(&demo, "Triangles");
	return 0;
}