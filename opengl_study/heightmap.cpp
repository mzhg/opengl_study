
#include "heightmap.h"
#pragma comment(lib,"ws2_32.lib")

void HeightmapDemo::onCreate()
{

}

void HeightmapDemo::onResize(GLuint width, GLuint height)
{
	if (width == 0 || height == 0)
		return;

	glViewport(0, 0, width, height);
}

void HeightmapDemo::onRender()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);

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
}

void HeightmapDemo::onDispose(){}

int main(int argc, char** argv)
{
	HeightmapDemo demo;
	jet::util::BaseApp::Run(&demo, "Triangles");
	return 0;
}