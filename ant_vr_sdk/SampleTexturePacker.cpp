#include "SampleTexturePacker.h"
#include "FileUtil.h"
#include "TextureUtil.h"
#include "Transform.h"

namespace jet
{
	namespace util
	{
		SampleTexturePacker::SampleTexturePacker()
		{

		}


		SampleTexturePacker::~SampleTexturePacker()
		{
			for (auto it : m_TestTextureList)
			{
				delete it.second;
			}
			m_TestTextureList.clear();

			delete m_FullScreenQuad;
			delete m_ScreenQuadBuffer;
			delete m_ScreenQuadVAO;
		}

		// Called When sample created 
		void SampleTexturePacker::OnCreate()
		{
			const char* filenames[] =
			{
//				"side2.jpg",
				"lena.jpg",
				"side1.jpg",
				"2.jpg",
				"2_gf.jpg",
				"3.jpg",
				"4.jpg",
				//					
				"6.jpg",
				"7.jpg",
				"8.jpg",
				//					"9.jpg",
				//					"11.jpg",
				"12.jpg",
				"13.jpg",
				
				
				
				"side3.jpg",
				"side5.jpg",
			};

			{ // Load Textures.
				const char* search_path = "../ant_vr_sdk/SampleData/Images/";

				for (const char* filename : filenames)
				{
					std::string filepath = FileUtil::findFilePath(filename, 1, &search_path);
					assert(filepath.length());

					Texture2D* pTex = new Texture2D;
					assert(TextureUtil::createTexture2DFromFile(filepath.c_str(), pTex));

					auto value = std::pair<std::string, Texture2D*>(std::string(filename), pTex);
					m_TestTextureList.insert(value);

					printf("Load %s sucessfully, Size = [%d, %d]\n", filename, pTex->getWidth(), pTex->getHeight());
				}

				printf("\n\\\\\\\Load test image done!.\n");
			}

			{// Create program.
				const char* search_path = "../ant_vr_sdk/";
				std::string vs_path = jet::util::FileUtil::findFilePath("CommonVS.vert", 1, &search_path);
				std::string ps_path = jet::util::FileUtil::findFilePath("CommonPS.frag", 1, &search_path);
				assert(vs_path.length());
				assert(ps_path.length());
				m_FullScreenQuad = GLSLProgram::createFromFiles(vs_path.c_str(), ps_path.c_str());

				GLint texLoc = m_FullScreenQuad->getUniformLocation("g_Texture0");
				GLint mvpLoc = m_FullScreenQuad->getUniformLocation("g_MVP");
				GLint texMatLoc = m_FullScreenQuad->getUniformLocation("g_TexcoordMat");
				assert(texLoc >= 0 && mvpLoc >= 0 && texMatLoc >= 0);
				glm::mat4 identity = glm::mat4();
				glProgramUniform1i(m_FullScreenQuad->getProgram(), texLoc, 0);
				glProgramUniformMatrix4fv(m_FullScreenQuad->getProgram(), mvpLoc, 1, false, (GLfloat*)&identity);
				glProgramUniformMatrix4fv(m_FullScreenQuad->getProgram(), texMatLoc, 1, false, (GLfloat*)&identity);
			}

			{  // Create Buffer

				struct Vertex
				{
					glm::vec2 pos;
					glm::vec2 texcoord;
				};

				m_ScreenQuadBuffer = new ArrayBufferGL<BufferUsage::STATIC_DRAW, GL_MAP_WRITE_BIT>;
				m_ScreenQuadBuffer->bind();

				Vertex data[] =
				{
					{ { -1.0f, -1.0f }, { 0.0f, 0.0f } },    // LB
					{ { +1.0f, -1.0f }, { 1.0f, 0.0f } },    // RB
					{ { -1.0f, +1.0f }, { 0.0f, 1.0f } },    // LT
					{ { +1.0f, +1.0f }, { 1.0f, 1.0f } },    // RT
				};  //Triangle_Strip

				m_ScreenQuadBuffer->init(sizeof(Vertex) * 4, (GLubyte*)data);
				m_ScreenQuadBuffer->unbind();

				m_ScreenQuadVAO = new VertexArrayGL();
				m_ScreenQuadVAO->bind();

				BufferData bufferData;
				bufferData.ArrayBuffer = m_ScreenQuadBuffer;
				bufferData.AttribCount = 2;
				AttribDesc attribDesc[] =
				{
					{ true, 0, 2, DataType::FLOAT, false, sizeof(Vertex), 0, 0 },
					{ true, 2, 2, DataType::FLOAT, false, sizeof(Vertex), 0, (GLvoid*)sizeof(glm::vec2) },
				};
				bufferData.AttribDescs = attribDesc;
				bufferData.ElementBuffer = nullptr;

				m_ScreenQuadVAO->load(&bufferData);
				m_ScreenQuadVAO->unbind();
			}


			{// create Texture Altas.
				TextureAssembly textureList;
				PackSettings settings;
				settings.maxWidth = 4096;
				settings.maxHeight = 4096;
				settings.fast = true;
				for (auto it : m_TestTextureList)
				{
					textureList.add(it.first, it.second);
				}

				
				m_TextureAltas = new TextureAtlas;
				TexturePacker packer;
				packer.pack(textureList, settings, m_TextureAltas);
				printf("done!\n");

				printf("TextureAtlas: width = %d, height = %d.\n", m_TextureAltas->getTexture()->getWidth(), m_TextureAltas->getTexture()->getHeight());
				for (const char* filename : filenames)
				{
					const TextureRegion& region = m_TextureAltas->getTextureRegion(filename);
					const char* flags[] = { "false", "true" };
					printf("TextureRegion: name = %s, (x,y,w,h) = (%d,%d,%d,%d), rotate = %s.\n", filename, 
						region.Region.X, region.Region.Y, region.Region.Width, region.Region.Height, flags[region.Rotated]);
				}
			}

		}
		// Called When the viewport changed!
		void SampleTexturePacker::OnResize(int x, int y, int width, int height)
		{

		}

		// Render Loop...
		void SampleTexturePacker::OnRender(float elpsedTime)
		{
			GLStates& states = GLStates::get();

			ViewportDesc desc;
			desc.ScissorEnable = false;
			desc.Viewport = { X, Y, Width, Height };
			states.resetFramebuffer();   CHECK_GL_ERROR;
			states.setViewports(1, &desc); CHECK_GL_ERROR;
			states.setDSState(nullptr); CHECK_GL_ERROR;
			states.setRSState(nullptr);  CHECK_GL_ERROR;//defualt is ok.
			states.setBlendState(nullptr); CHECK_GL_ERROR;

			states.setClearColor(0.5f, 0.5f, 0.5f, 1); CHECK_GL_ERROR;
			glClear(GL_COLOR_BUFFER_BIT);


			m_FullScreenQuad->enable(); CHECK_GL_ERROR;
			m_ScreenQuadVAO->bind(); CHECK_GL_ERROR;
			const TextureGL* pTexs[] = { m_TextureAltas->getTexture() };
//			const TextureGL* pTexs[] = { m_TestTextureList["2.jpg"] };
			states.bindTextures(1, pTexs); CHECK_GL_ERROR;

			float width_ratio = Numeric::ratio(pTexs[0]->getWidth(), Width);
			float height_ratio = Numeric::ratio(pTexs[0]->getHeight(), Height);
			float max_ratio = Numeric::max(width_ratio, height_ratio);
			max_ratio = 1.0f/Numeric::max(1.0f, max_ratio);

			Rectangle2ui texRect = { 0u, 0u, static_cast<uint32_t>(max_ratio * pTexs[0]->getWidth()), static_cast<uint32_t>(max_ratio * pTexs[0]->getHeight()) };
			glm::mat4 mvp = Transform::ortho(Width, Height, texRect);
			GLint mvpLoc = m_FullScreenQuad->getUniformLocation("g_MVP");
			glProgramUniformMatrix4fv(m_FullScreenQuad->getProgram(), mvpLoc, 1, false, (GLfloat*)&mvp);

//			glActiveTexture(GL_TEXTURE0);
//			glBindTexture(GL_TEXTURE_2D, m_TextureAltas->getTexture()->getTexture());
//			glBindTexture(GL_TEXTURE_2D, pTexs[0]->getTexture());
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4); CHECK_GL_ERROR;
//			states.reset
		}
	}
}
