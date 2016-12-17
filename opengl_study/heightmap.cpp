
#include "heightmap.h"
#include "FileUtil.h"
#include <glm.hpp>

struct Rect
{
	int left, bottom;
	int right, top;
};

void HeightmapDemo::onCreate()
{

//	std::string filepath = jet::util::FileUtil::findFilePath("DrawSceneVS.vert");
//	printf("testimg_1280x720.png full path: %s\n", filepath.c_str());

	m_Sample = new SampleTunnel();
//	m_Sample = new SampleColorTriangle();
	m_Sample->Create();
	m_PostProcessing = new PostProcessing();
}

void HeightmapDemo::onResize(GLuint width, GLuint height)
{
	if (width == 0 || height == 0)
		return;

	if (width == mWidth && height == mHeight)
		return;

	mWidth = width;
	mHeight = height;
	m_Sample->Resize(0, 0, width, height);
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

void HeightmapDemo::viewportTest()
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

static glm::mat4 g_PreviouseMat;
static bool g_bPreviouseMatSetted = false;
static int g_iFrameCount = 0;

void HeightmapDemo::onRender()
{
	g_iFrameCount++;
	m_Sample->Render();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);

	/*
	m_FullScreenQuad->enable();
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, m_ScreenColorTexture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);
	GL_CHECK(glUseProgram(0));
	*/
	checkGLError();

	RenderInfo info;
	m_Sample->getRenderInfo(info);
	glm::mat4 viewProj = info.ProjMat * info.ViewMat;
	glm::mat4 viewProjInvert = glm::inverse(viewProj);
	if (!g_bPreviouseMatSetted)
	{
		g_bPreviouseMatSetted = true;
		g_PreviouseMat = viewProj;
	}
	
	// 4*n - 1
//	m_PostProcessing->addGaussBlur(79);
//	m_PostProcessing->addBloom(0.35f, 1.0, 3.1);
//	m_PostProcessing->addFXAA(5);
	m_PostProcessing->addStaticMotionBlur(g_PreviouseMat, viewProjInvert);
	FrameAttribs frameAttribs;
	frameAttribs.SceneColorBuffer = (Texture2D*)m_Sample->getColorTexture(0);
	frameAttribs.SceneDepthBuffer = (Texture2D*)m_Sample->getDepthlTexture();
	glTextureParameteri(frameAttribs.SceneColorBuffer->getTexture(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(frameAttribs.SceneColorBuffer->getTexture(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(frameAttribs.SceneDepthBuffer->getTexture(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(frameAttribs.SceneDepthBuffer->getTexture(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	frameAttribs.Viewport = { 0, 0, mWidth, mHeight };

	m_PostProcessing->performancePostProcessing(frameAttribs);
 	checkGLError();

//	if (g_iFrameCount >= 1)
	{
		g_PreviouseMat = viewProj;
		g_iFrameCount = 0;
	}
	
}

void HeightmapDemo::onDispose()
{
	delete m_Sample;
	delete m_PostProcessing;
}

int main(int argc, char** argv)
{
	HeightmapDemo demo;
	demo.getConfig().IsOpenGLESContext = false;
	jet::util::BaseApp::Run(&demo, "Triangles");
	return 0;
}