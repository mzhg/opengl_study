
#include "heightmap.h"
#include "FileUtil.h"
#include <glm.hpp>
#include "simple_sdk.h"
#include <SampleTexturePacker.h>

struct Rect
{
	int left, bottom;
	int right, top;
};

static bool g_test_sdk = false;
static GLSLProgram* m_ScreenProgram;
static Texture2D*   m_TestTexture;

extern void rect_pack_test();

void HeightmapDemo::onCreate()
{
	struct MyStruct
	{
		bool b : 1;
		int  a : 6;

		operator bool ()
		{
			return b || a;
		}
	};

	printf("sizeof(MyStruct) = %d.\n", sizeof(MyStruct));

	MyStruct array[] =
	{
		{ true, 1 },
		{ false, 2 },
		{ false, 3 },
		{0}
	};

	printf("array size = %d.\n", Numeric::arraySize(array));

	struct MyStruct2
	{
		char name[128];
	};

	MyStruct2 first;
	strcpy_s(first.name, "I am jet");

	MyStruct2 second = first;
	printf("second.name = %s.\n", second.name);
	rect_pack_test();

	if (g_test_sdk)
	{
		const char* search_path = "../ant_vr_sdk/";
		std::string texpath = jet::util::FileUtil::findFilePath("testimg_1280x720.png", 1, &search_path);
		TextureData texData;
		int width;
		int height;
		jet::util::TextureUtil::loadTextureDataFromFile(texpath.c_str(), &texData, &width, &height);

		m_TestTexture = new Texture2D;
		Texture2DDesc desc(width, height, GL_RGBA8);
		jet::util::TextureUtil::createTexture2D(&desc, &texData, m_TestTexture);

		std::string vs_path = jet::util::FileUtil::findFilePath("DefaultScreenSpaceVS.vert", 1, &search_path);
		std::string ps_path = jet::util::FileUtil::findFilePath("DefaultScreenSpacePS.frag", 1, &search_path);
		assert(vs_path.length());
		assert(ps_path.length());
		m_ScreenProgram = GLSLProgram::createFromFiles(vs_path.c_str(), ps_path.c_str());

		ogl_init();
//		ogl_set_rect_texture(width, height, texData.Format, (const char*)texData.pData[0]);
//		ogl_set_rect_texture(width, height, texData.Format, (const char*)texData.pData);
		ogl_set_rect_size((1280 - 600)/2, (720 - 200)/2, 600, 200);
		ogl_create_background_default_texture(512, 512, false);
		ogl_create_background_default_texture(512, 512, true);
		/*
		Texture2D* tex2d = new Texture2D();
		Texture2DDesc  desc = Texture2DDesc(1280, 720, GL_RGB8);
		TextureUtil::createTexture2D(&desc, &texData, tex2d);
		ogl_set_rect_texture_id(tex2d->getTexture());*/
		return;
	}

	m_Sample = new SampleTexturePacker();
	m_Sample->Create();
	m_Input = new InputAdapter();
	setInputAdapter(m_Input);
	m_Input->SetMouseCallback(m_Sample);
	m_PostProcessing = new PostProcessing();
}

void HeightmapDemo::onResize(GLuint width, GLuint height)
{
	if (width == 0 || height == 0)
		return;

	if (width == mWidth && height == mHeight)
		return;

	if (g_test_sdk)
	{
		ogl_resize((int)width, (int)height);
		return;
	}

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
#if 0
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
#endif
}

static glm::mat4 g_PreviouseMat;
static bool g_bPreviouseMatSetted = false;
static int g_iFrameCount = 0;

void HeightmapDemo::onRender()
{
	g_iFrameCount++;
	if (g_test_sdk)
	{
		ogl_begin_record();

		m_ScreenProgram->enable();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(m_TestTexture->getTarget(), m_TestTexture->getTexture());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

		ogl_end_record();
		ogl_set_rect_from_screen_record();
		ogl_set_background_rotation(g_iFrameCount * 0.01f, 0.0f, 0.0f);
//		ogl_read_texels_from_renderbuffer();

		float mat[16];
		memset(mat, 0, 16 * sizeof(float));
		mat[0] = mat[5] = mat[10] = mat[15] = 1.0f;
		ScreenResult out;
		ogl_update(out, mat);
		ogl_render(0.0f);
		return;
	}

	
	m_Sample->Render(false);
	return;
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
	m_PostProcessing->addBloom(0.45f, 1.0f, 3.1f);
	m_PostProcessing->addFXAA(5);
	m_PostProcessing->addStaticMotionBlur(g_PreviouseMat, viewProjInvert);
	FrameAttribs frameAttribs;
	frameAttribs.SceneColorBuffer = (Texture2D*)m_Sample->getColorTexture(0);
	frameAttribs.SceneDepthBuffer = (Texture2D*)m_Sample->getDepthlTexture();
	glTextureParameteri(frameAttribs.SceneColorBuffer->getTexture(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(frameAttribs.SceneColorBuffer->getTexture(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(frameAttribs.SceneDepthBuffer->getTexture(), GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(frameAttribs.SceneDepthBuffer->getTexture(), GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	frameAttribs.Viewport = { 0, 0, mWidth, mHeight };
	CHECK_GL_ERROR;

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
	if (g_test_sdk)
	{
		ogl_destroy();
		return;
	}
	delete m_Sample;
	delete m_PostProcessing;
}


#include <SimpleScene.h>

int main(int argc, char** argv)
{
#if 1
	HeightmapDemo demo;
	demo.getConfig().IsOpenGLESContext = false;
	jet::util::BaseApp::Run(&demo, "Triangles", 1280,720);
#else
	SimpleScene scene;
	scene.start();
#endif
	return 0;
}