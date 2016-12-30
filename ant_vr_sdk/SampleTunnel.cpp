#include "SampleTunnel.h"
#include "FileUtil.h"
#include "TextureUtil.h"
#include "GLUtil.h"
#include <gtc/matrix_transform.hpp>

namespace jet
{
	namespace util
	{
		SampleTunnel::SampleTunnel() :m_VAO(0), m_VBO(0), m_IBO(0), m_FloorTex(0), m_WallTex(0), m_CeilingTex(0), m_TexSample(0),
			m_SceneProgram(0), m_Time(0.0f)
		{}

		// Called When sample created 
		void SampleTunnel::OnCreate()
		{
			glGenSamplers(1, &m_TexSample);
			glSamplerParameteri(m_TexSample, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glSamplerParameteri(m_TexSample, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glSamplerParameteri(m_TexSample, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE );
			glSamplerParameteri(m_TexSample, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glSamplerParameteri(m_TexSample, GL_TEXTURE_WRAP_S, GL_REPEAT);

			const char* file_dir = "../ant_vr_sdk/";
			std::string floorTexPath = FileUtil::findFilePath("images/floor.jpg", 1, &file_dir);
			assert(floorTexPath.length());
			m_FloorTex = new Texture2D();
			TextureUtil::createTexture2DFromFile(floorTexPath.c_str(), m_FloorTex);
			glGenerateTextureMipmap(m_FloorTex->getTexture());
			
			std::string wallTexPath = FileUtil::findFilePath("images/wall.jpg", 1, &file_dir);
			assert(wallTexPath.length());
			m_WallTex = new Texture2D();
			TextureUtil::createTexture2DFromFile(wallTexPath.c_str(), m_WallTex);
			glGenerateTextureMipmap(m_WallTex->getTexture());

			std::string ceilingTexPath = FileUtil::findFilePath("images/ceiling.jpg", 1, &file_dir);
			assert(ceilingTexPath.length());
			m_CeilingTex = new Texture2D();
			TextureUtil::createTexture2DFromFile(ceilingTexPath.c_str(), m_CeilingTex);
			glGenerateTextureMipmap(m_CeilingTex->getTexture());

			std::string vertFile = FileUtil::findFilePath("SampleTunnel/SceneVS.vert", 1, &file_dir);
			std::string fragFile = FileUtil::findFilePath("SampleTunnel/ScenePS.frag", 1, &file_dir);
			assert(vertFile.length());
			assert(fragFile.length());
			m_SceneProgram = GLSLProgram::createFromFiles(vertFile.c_str(), fragFile.c_str());

			glm::vec3 vertexs[4 * 2];
			{
				const float distance = 10000;
				// front
				vertexs[0] = glm::vec3(-1.0f, -1.0f, 0.0f);
				vertexs[1] = glm::vec3(+1.0f, -1.0f, 0.0f);
				vertexs[2] = glm::vec3(+1.0f, +1.0f, 0.0f);
				vertexs[3] = glm::vec3(-1.0f, +1.0f, 0.0f);

				// back
				vertexs[4] = glm::vec3(-1.0f, -1.0f, -distance);
				vertexs[5] = glm::vec3(+1.0f, -1.0f, -distance);
				vertexs[6] = glm::vec3(+1.0f, +1.0f, -distance);
				vertexs[7] = glm::vec3(-1.0f, +1.0f, -distance);
			}

			short indices[6 * 4];
			{
				// floor
				indices[0] = 0;
				indices[1] = 1;
				indices[2] = 5;
				indices[3] = 0;
				indices[4] = 5;
				indices[5] = 4;

				// left
				indices[6] = 0;
				indices[7] = 4;
				indices[8] = 7;
				indices[9] = 0;
				indices[10] = 7;
				indices[11] = 3;

				// right
				indices[12] = 1;
				indices[13] = 5;
				indices[14] = 6;
				indices[15] = 1;
				indices[16] = 6;
				indices[17] = 2;

				// top
				indices[18] = 2;
				indices[19] = 3;
				indices[20] = 7;
				indices[21] = 2;
				indices[22] = 7;
				indices[23] = 6;
			}

			glm::vec3 expand_vertexs[24];
			for (int i = 0; i < 24; i++)
			{
				expand_vertexs[i] = vertexs[indices[i]];
			}

			glGenVertexArrays(1, &m_VAO);
			glBindVertexArray(m_VAO);
			{
				glGenBuffers(1, &m_VBO);
				glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
				glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 24, &expand_vertexs[0], GL_STATIC_DRAW);

				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (const void*)0);
				glEnableVertexAttribArray(0);

//				glGenBuffers(1, &m_IBO);
//				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IBO);
//				glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(short) * 24, &indices[0], GL_STATIC_DRAW);
			}
			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		}

		// Called When the viewport changed!
		void SampleTunnel::OnResize(int x, int y, int width, int height)
		{
			m_ProjMat = glm::perspective(glm::radians(90.0f), static_cast<float>(width) / static_cast<float>(height), 0.1f, 100.0f);
		}

		// Render Loop...
		void SampleTunnel::OnRender(float elpsedTime)
		{
			glClearColor(0.29f, 0.29f, 0.29f, 0);
			glClearDepth(1.0f);
			glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_CULL_FACE);
			glDepthFunc(GL_LESS);

			glm::vec3 f3EyePos = { 0.0f, 0.0f, 1.0f };
			glm::vec3 f3EyeDir = { 0.0f, 0.0f, -1.0f };

			const float fCameraSpeed = 3.0f;
			m_Time += 0.016f;

			CHECK_GL_ERROR;
			glBindVertexArray(m_VAO);
			m_SceneProgram->enable();

			glm::vec3 f3NewEyePos = f3EyePos + glm::vec3(0.0f, 0.0f, -m_Time * fCameraSpeed);
			m_ViewMat = glm::lookAt(f3NewEyePos, f3NewEyePos + f3EyeDir, glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 viewProj = m_ProjMat * m_ViewMat;
			GLint matLocation = m_SceneProgram->getUniformLocation("g_ViewProjection");
			assert(matLocation >= 0);
			glUniformMatrix4fv(matLocation, 1, false, (const GLfloat*)&viewProj);

//			GLint horizontalLocation = m_SceneProgram->getUniformLocation("g_Horizontal");
//			assert(horizontalLocation >= 0);
//			glUniform1i(horizontalLocation, 1);
			glBindTextureUnit(0, m_FloorTex->getTexture());
			glBindTextureUnit(1, m_WallTex->getTexture());
			glBindTextureUnit(2, m_CeilingTex->getTexture());
			glBindSampler(0, m_TexSample);
			glBindSampler(1, m_TexSample);
			glBindSampler(2, m_TexSample);
//			glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_SHORT, (const void*)(0));
			glDrawArrays(GL_TRIANGLES, 0, 24);
			/*
			{
				// draw the floor
				glUniform1i(horizontalLocation, 1);
				glBindTextureUnit(0, m_FloorTex->getTexture());
				glDrawRangeElements(GL_TRIANGLES, 0, 5, 6, GL_UNSIGNED_SHORT, (const void*)0);
			}

			{
				// draw the left wall
				glUniform1i(horizontalLocation, 0);
				glBindTextureUnit(0, m_WallTex->getTexture());
				glDrawRangeElements(GL_TRIANGLES, 6, 11, 6, GL_UNSIGNED_SHORT, (const void*)0);
			}

			{
				// draw the right wall
				glUniform1i(horizontalLocation, 0);
				glBindTextureUnit(0, m_FloorTex->getTexture());
				glDrawRangeElements(GL_TRIANGLES, 12, 17, 6, GL_UNSIGNED_SHORT, (const void*)0);
			}

			{
				// draw the top
				glUniform1i(horizontalLocation, 1);
				glBindTextureUnit(0, m_CeilingTex->getTexture());
				glDrawRangeElements(GL_TRIANGLES, 18, 23, 6, GL_UNSIGNED_SHORT, (const void*)0);
			}
			*/

			glBindSampler(0, 0);
			glBindVertexArray(0);
			m_SceneProgram->disable();
			CHECK_GL_ERROR;

			if (f3NewEyePos.z < -10000.0f)
			{
				m_Time = 0.0f;
			}
		}

		SampleTunnel::~SampleTunnel()
		{
			SAFE_RELEASE_VERTEX_ARRAY(m_VAO);
			SAFE_RELEASE_BUFFER(m_VBO);
			SAFE_RELEASE_BUFFER(m_IBO);
			SAFE_DISPOSE(m_FloorTex);
			SAFE_DISPOSE(m_WallTex);
			SAFE_DISPOSE(m_CeilingTex);
		}
	}
}
