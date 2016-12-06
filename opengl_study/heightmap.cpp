
#include "heightmap.h"
#include <glm.hpp>

typedef struct Rect
{
	int left, bottom;
	int right, top;
}Rect;


void HeightmapDemo::onCreate()
{
	m_SceneColorBuffer = new Texture2D();
	const GLubyte* render = glGetString(GL_RENDERER);
	const GLubyte* version = glGetString(GL_VERSION);
	const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("Render: %s\n", render);
	printf("Version: %s\n", version);
	printf("GLSLVersion: %s\n", glslVersion);
	
	
	jet::util::GLVersion _version= GetOpenGLVersion();
	printf("GLVersion: %s\n", _version.toString().c_str());

	mpTestTexture = new Texture2D();
	TextureUtil::createTexture2DFromFile("testimg_1280x720.png", mpTestTexture);
	m_FullScreenQuad = GLSLProgram::createFromFiles("DefaultScreenSpaceVS.vert", "DefaultScreenSpacePS.frag");
	CreateAnimatedTrianglesGeometry();

	glEnable(GL_FRAMEBUFFER_SRGB);
	m_PostProcessing = new PostProcessing();
}

typedef glm::vec2 float2;
typedef glm::vec4 float4;

struct ATVertex
{
	float2          Position;
	unsigned int    Color;

	float4          ABCD;

	ATVertex() {}

	ATVertex(const float2 & pos, unsigned int col, const float4 & abcd) : Position(pos), Color(col), ABCD(abcd) { }
};

static const int        g_ATCount = 32;
static GLsizei          g_ATIndexCount = 0;
static GLuint     g_ATPositionSlot = 0;
static GLuint     g_ATColorSlot = 1;
static ATVertex         g_ATVerts[g_ATCount * 3];

#define GL_CHECK(x) x

void HeightmapDemo::CreateAnimatedTrianglesGeometry()
{
	// Initilize the programs
	m_AnimatedTrianglesProgram = GLSLProgram::createFromFiles("DrawSceneVS.vert", "DrawScenePS.frag", true);
	g_ATPositionSlot = m_AnimatedTrianglesProgram->getAttribLocation("Position");
	g_ATColorSlot =    m_AnimatedTrianglesProgram->getAttribLocation("Color");

	int Faces[g_ATCount * 3];

	for (int i = 0; i < g_ATCount * 3; i += 3)
	{
		Faces[i + 0] = i + 0;
		Faces[i + 1] = i + 1;
		Faces[i + 2] = i + 2;
	}

	srand(0);

	for (int i = 0; i < g_ATCount * 3; i++)
	{
		unsigned char r = rand() & 0xFF | 0x40;
		unsigned char g = rand() & 0xFF | 0x40;
		unsigned char b = rand() & 0xFF | 0x40;
		unsigned char a = rand() & 0xFF | 0xA0;
		float x = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;
		float y = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;


		float ha = (rand() / (float)RAND_MAX) * 1.0f;               // triangle shape
		float hb = (rand() / (float)RAND_MAX) * 0.5f - 0.25f;       // triangle width
		float hc = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;        // triangle speed x
		float hd = (rand() / (float)RAND_MAX) * 2.0f - 1.0f;        // triangle speed y

		g_ATVerts[i] = ATVertex(float2(x, y), (a << 24) | (b << 16) | (g << 8) | (r << 0), float4(ha, hb, hc, hd));
	}

	g_ATIndexCount = sizeof(Faces) / sizeof(Faces[0]);

	// Create the VAO:
	glGenVertexArrays(1, &m_ATVAO);
	glBindVertexArray(m_ATVAO);

	// Create the VBO for positions:
//	DEBUG_PRINT(_L("        - Create the VBO pos"));
	GLsizei stride = sizeof(ATVertex);
	GL_CHECK(glGenBuffers(1, &m_ATVAOVertices));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_ATVAOVertices));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sizeof(g_ATVerts), g_ATVerts, GL_DYNAMIC_DRAW));
	GL_CHECK(glEnableVertexAttribArray(g_ATPositionSlot));
	GL_CHECK(glVertexAttribPointer(g_ATPositionSlot, 2, GL_FLOAT, GL_FALSE, stride, 0));
	GL_CHECK(glEnableVertexAttribArray(g_ATColorSlot));
	GL_CHECK(glVertexAttribPointer(g_ATColorSlot, 4, GL_UNSIGNED_BYTE, GL_TRUE, stride, (void*)(2 * sizeof(float))));

	// Create the VBO for indices:
//	DEBUG_PRINT(_L("        - Create the VBO ind"));
	GLuint indices;
	GL_CHECK(glGenBuffers(1, &indices));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indices));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Faces), Faces, GL_STATIC_DRAW));

	GL_CHECK(glBindVertexArray(0));
}

void HeightmapDemo::DrawAnimatedTriangles(double deltaSeconds, bool messWithTheGraph)
{
	for (int i = 0; i < g_ATCount * 3; i++)
	{
		ATVertex & v = g_ATVerts[i];
		if ((i % 3) == 1)
		{
			ATVertex & vL = g_ATVerts[i - 1];
			ATVertex & vR = g_ATVerts[i + 1];

			float2 diff = vR.Position - vL.Position;
			float2 diff2 = float2(diff.y, diff.x);

			v.Position = vL.Position + diff * v.ABCD.x + diff2 * v.ABCD.y;
		}
		else
		{
			const float speed = (float)deltaSeconds * 0.05f;

			v.Position.x += v.ABCD.z * speed;
			if (v.Position.x > 1.0f || v.Position.x < -1.0f)
			{
				v.ABCD.z = -v.ABCD.z;
				v.Position.x += v.ABCD.z * speed;
			}
			v.Position.y += v.ABCD.w * speed;
			if (v.Position.y > 1.0f || v.Position.y < -1.0f)
			{
				v.ABCD.w = -v.ABCD.w;
				v.Position.y += v.ABCD.w * speed;
			}
		}
	}

	// gray out the background
	g_ATVerts[0] = ATVertex(float2(-3.0f, -3.0f), (0x80U << 24) | (0x00U << 16) | (0x00U << 8) | (0x00U << 0), float4(0.0f, 0.0f, 0.0f, 0.0f));
	g_ATVerts[1] = ATVertex(float2(3.0f, -3.0f), (0x80U << 24) | (0x00U << 16) | (0x00U << 8) | (0x00U << 0), float4(0.0f, 0.0f, 0.0f, 0.0f));
	g_ATVerts[2] = ATVertex(float2(0.0f, 3.0f), (0x80U << 24) | (0x00U << 16) | (0x00U << 8) | (0x00U << 0), float4(0.0f, 0.0f, 0.0f, 0.0f));
	if (messWithTheGraph)
	{
		g_ATVerts[0].Color = 0;
		g_ATVerts[1].Color = 0;
		g_ATVerts[2].Color = 0;
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GL_CHECK(m_AnimatedTrianglesProgram->enable());
	GL_CHECK(glBindVertexArray(m_ATVAO));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_ATVAOVertices));
	GL_CHECK(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(g_ATVerts), g_ATVerts));
	GL_CHECK(glDrawElements(GL_TRIANGLES, g_ATIndexCount, GL_UNSIGNED_INT, 0));
	GL_CHECK(glUseProgram(0));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CHECK(glBindVertexArray(0));

	glDisable(GL_BLEND);
}

void HeightmapDemo::onResize(GLuint width, GLuint height)
{
	if (width == 0 || height == 0)
		return;

	if (width == mWidth && height == mHeight)
		return;

	mWidth = width;
	mHeight = height;
	glViewport(0, 0, width, height);

	SAFE_RELEASE_FRAMEBUFFER(m_FBO);
	SAFE_RELEASE_TEXTURE(m_ScreenColorTexture);

	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	glGenTextures(1, &m_ScreenColorTexture);
	glBindTexture(GL_TEXTURE_2D, m_ScreenColorTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8_ALPHA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ScreenColorTexture, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	TextureUtil::createTexture2D(GL_TEXTURE_2D, m_ScreenColorTexture, m_SceneColorBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);
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

void HeightmapDemo::onRender()
{
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	{
		m_FullScreenQuad->enable();
		glBindVertexArray(0);
		glBindTexture(GL_TEXTURE_2D, mpTestTexture->getTexture());
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
		glBindTexture(GL_TEXTURE_2D, 0);
		GL_CHECK(glUseProgram(0));

		DrawAnimatedTriangles(0.016f, false);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_FullScreenQuad->enable();
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, m_ScreenColorTexture);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindTexture(GL_TEXTURE_2D, 0);
	GL_CHECK(glUseProgram(0));

	checkGLError();

	// 4*n - 1
	m_PostProcessing->addGaussBlur(35);
	FrameAttribs frameAttribs;
	frameAttribs.SceneColorBuffer = m_SceneColorBuffer;

	m_PostProcessing->performancePostProcessing(frameAttribs);
 	checkGLError();
}

void HeightmapDemo::onDispose(){
	printf("onDispose called!!!\n");
	SAFE_DISPOSE(m_AnimatedTrianglesProgram);
	SAFE_DISPOSE(mpTestTexture);
	SAFE_RELEASE_VERTEX_ARRAY(m_ATVAO);
	SAFE_RELEASE_BUFFER(m_ATVAOVertices);
	SAFE_RELEASE_FRAMEBUFFER(m_FBO);
	SAFE_DISPOSE(m_SceneColorBuffer);
	
	delete m_PostProcessing;
}

int main(int argc, char** argv)
{
	HeightmapDemo demo;
	demo.getConfig().IsOpenGLESContext = false;
	jet::util::BaseApp::Run(&demo, "Triangles");
	return 0;
}