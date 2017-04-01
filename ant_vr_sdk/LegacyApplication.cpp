#include "LegacyApplication.h"
#include <BaseApp.h>
#include "Geometry.h"
#include "GLStates.h"

namespace jet
{
	namespace util
	{
		class AppWrapper : public BaseApp
		{
		public:
			AppWrapper(SampleApp* pApp) : m_Sample(pApp){}

			void onCreate() override
			{
				m_Input = new InputAdapter();
				setInputAdapter(m_Input);
				m_Input->SetMouseCallback(m_Sample);
				m_Sample->Create();
			}

			void onResize(GLuint width, GLuint height) override
			{
				if (width == 0 || height == 0)
					return;

				if (width == mWidth && height == mHeight)
					return;

				mWidth = width;
				mHeight = height;
				m_Sample->Resize(0, 0, width, height);
			}

			void onRender() override{	m_Sample->Render(false);}

			void onDispose() override{}

			~AppWrapper()
			{
				delete m_Input;
			}
		private:
			int mWidth;
			int mHeight;

			SampleApp* m_Sample;

			InputAdapter* m_Input;
		};

		LegacyApplication::LegacyApplication()
		{
			m_pScene = new Scene();
			m_pRoot = new Node("Root");
		}

		LegacyApplication::~LegacyApplication()
		{
			delete m_pRoot;
		}

		void LegacyApplication::start(uint32_t width, uint32_t height)
		{
			AppWrapper app(this);
			app.getConfig().IsOpenGLESContext = false;
			jet::util::BaseApp::Run(&app, "LegacyApplication");

//			app.~AppWrapper();
		}

		// Called When sample created 
		void LegacyApplication::OnCreate()
		{
			GLStates& states = GLStates::get();
			m_pProgram = GLSLProgram::createFromFiles("CommonVS.vert", "CommonPS.frag");

			glGenBuffers(1, &m_ArrayBuffer);
			glGenBuffers(1, &m_ElementBuffer);
# if 0
			states.bindBuffer(m_ArrayBuffer, BufferTarget::ARRAY);
			glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 1024, nullptr, GL_DYNAMIC_DRAW);
			states.bindBuffer(m_ElementBuffer, BufferTarget::ELEMENT);
			glBufferData(GL_ARRAY_BUFFER, sizeof(uint16_t) * 1024, nullptr, GL_DYNAMIC_DRAW);
#endif
			initialize();

			// TODO
			renderNode(m_pRoot);
		}
		// Called When the viewport changed!
		void LegacyApplication::OnResize(int x, int y, int width, int height)
		{

		}

		void LegacyApplication::renderNode(Node* pNode)
		{
			GLStates& states = GLStates::get();
			std::vector<Spatial*> pChildren = pNode->getChildren();
			int count = pChildren.size();
			for (int i = 0; i < count; i++)
			{
				Spatial* pChild = pChildren[i];
				Node* _pNode = dynamic_cast<Node*>(pChild);
				if (_pNode)
				{
					renderNode(_pNode);
				}
				else
				{
					if (pChild->getVertexCount())
					{
						Geometry* pGeo = dynamic_cast<Geometry*>(pChild);
						if (pGeo)
						{
							ShapePtr pShape =  pGeo->getMesh();
							if (pShape.get())
							{
								MeteDataPtr arrayData = pShape->getVertexData(MeshAttrib::POSITION3);
								MeteDataPtr elementData = pShape->getVertexData(MeshAttrib::INDICES);

								states.bindBuffer(m_ArrayBuffer, BufferTarget::ARRAY);
								glBufferData(GL_ARRAY_BUFFER, arrayData->uiLength, arrayData->pData, GL_STATIC_DRAW);
								states.bindBuffer(m_ElementBuffer, BufferTarget::ELEMENT);
								glBufferData(GL_ELEMENT_ARRAY_BUFFER, elementData->uiLength, elementData->pData, GL_STATIC_DRAW);

								print_mete_data<glm::vec3>(arrayData.get(), "Box ArrayBuffer");
								print_mete_data<uint16_t>(elementData.get(), "Box ElementBuffer");

								glm::vec4 color_source[8] =
								{
									glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
									glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
									glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
									glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),

									glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
									glm::vec4(1.0f, 0.0f, 1.0f, 1.0f),
									glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
									glm::vec4(0.5f, 0.5f, 0.5f, 1.0f),
								};

								glm::vec4 vertex_colors[24];
								const uint16_t* pIndices = reinterpret_cast<const uint16_t*>(elementData->pData);
								for (int i = 0; i < 24; i++)
								{
									vertex_colors[i] = color_source[pIndices[i]%8];
								}

								CHECK_GL(glProgramUniform4fv(m_pProgram->getProgram(), m_pProgram->getUniformLocation("g_VertexColors"), 24, reinterpret_cast<const GLfloat*>(vertex_colors)));
							}
						}
					}
				}
			}
		}

		// Render Loop...
		void LegacyApplication::OnRender(float elpsedTime)
		{
			update(elpsedTime);

			GLStates& states = GLStates::get();
			states.setClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			states.setClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			DepthStencilDesc dsstate;
			dsstate.DepthEnable = true;
			dsstate.DepthFunc = CompareFunc::LESS;
			dsstate.StencilEnable = false;
			states.setDSState(&dsstate);

			RasterizerDesc rsstate;
			rsstate.CullFaceEnable = false;
			states.setRSState(&rsstate);

			m_pProgram->enable();

			static float x_rotate = 0.0f;
			static float y_rotate = 0.0f;

			Transform transform;
			transform.setRotateFromEulerAngle(x_rotate, y_rotate, 0.0f);
			transform.setScale(glm::vec3(0.7f));
			transform.setTranslateX(1.0f);

			x_rotate += 0.008f;
			y_rotate += 0.009f;

			glm::mat4 proj = glm::perspective(glm::radians(60.0f), 1280.0f / 720.0f, 0.1f, 100.0f);
			glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 mvp = proj * view * transform.getCombinedMatrix();

			
			GLint location = m_pProgram->getUniformLocation("g_MVP");
			assert(location >= 0);
			CHECK_GL(glUniformMatrix4fv(location, 1, false, reinterpret_cast<const GLfloat*>(&mvp)));
			states.bindBuffer(m_ArrayBuffer, BufferTarget::ARRAY);
			CHECK_GL(glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, reinterpret_cast<const GLvoid*>(0)));
			CHECK_GL(glEnableVertexAttribArray(0));
			states.bindBuffer(m_ElementBuffer, BufferTarget::ELEMENT);

			CHECK_GL(glDrawElements(GL_TRIANGLES, 30, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid*>(12)));
//			CHECK_GL(glDrawRangeElements(GL_TRIANGLES, 6, 12, 12, GL_UNSIGNED_SHORT, reinterpret_cast<const GLvoid*>(0)));
			CHECK_GL(glDisableVertexAttribArray(0));
		}
	}
}