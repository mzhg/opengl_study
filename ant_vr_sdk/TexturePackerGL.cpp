#include "TexturePacker.h"
#include "geometry2d.h"
#include "MaxRectsBinPack.h"
#include "FramebufferGL.h"
#include "GLSLProgram.h"
#include <algorithm>
#include "TextureUtil.h"
#include "FileUtil.h"
#include "BufferGL.h"

#include <glm.hpp>
#include <gtc\quaternion.hpp>
#include <gtc\matrix_transform.hpp>

#pragma warning(disable:4244)
namespace jet
{
	namespace util
	{
#define null nullptr

		static GLSLProgram* g_TextureProgram = null;
		static ArrayBufferGL<BufferUsage::STATIC_DRAW, GL_MAP_READ_BIT|GL_MAP_WRITE_BIT>* g_ScreenRectBuffer = null;
		static VertexArrayGL* g_ScreenRectVAO = null;
		static glm::vec2 g_Viewport;

		static void setViewport(GLint width, GLint height)
		{
			GLStates& states = GLStates::get();

			ViewportDesc desc;
			desc.ScissorEnable = false;
			desc.Viewport = { 0, 0, width, height };

			// record the current viewport
			g_Viewport = { width, height };

			states.setViewports(1, &desc);
		}

		static void createGLObjects()
		{
			struct Vertex
			{
				glm::vec2 pos;
				glm::vec2 texcoord;
			};

			if (g_TextureProgram)
				return;

			const char* search_path = "../ant_vr_sdk/";
			std::string vs_path = jet::util::FileUtil::findFilePath("CommonVS.vert", 1, &search_path);
			std::string ps_path = jet::util::FileUtil::findFilePath("CommonPS.frag", 1, &search_path);
			assert(vs_path.length());
			assert(ps_path.length());
			g_TextureProgram = GLSLProgram::createFromFiles(vs_path.c_str(), ps_path.c_str());
			GLint texLoc = g_TextureProgram->getUniformLocation("g_Texture0");
			assert(texLoc >= 0);
			glProgramUniform1i(g_TextureProgram->getProgram(), texLoc, 0);

			g_ScreenRectBuffer = new ArrayBufferGL<BufferUsage::STATIC_DRAW, GL_MAP_READ_BIT | GL_MAP_WRITE_BIT>;
			g_ScreenRectBuffer->bind();

			Vertex data[] =
			{
				{ { -1.0f, -1.0f }, { 0.0f, 0.0f } },    // LB
				{ { +1.0f, -1.0f }, { 1.0f, 0.0f } },    // RB
				{ { -1.0f, +1.0f }, { 0.0f, 1.0f } },    // LT
				{ { +1.0f, +1.0f }, { 1.0f, 1.0f } },    // RT
			};  //Triangle_Strip

			g_ScreenRectBuffer->init(sizeof(data), (GLubyte*)data);
			g_ScreenRectBuffer->unbind();

			g_ScreenRectVAO = new VertexArrayGL();
			g_ScreenRectVAO->bind();

			BufferData bufferData;
			bufferData.ArrayBufferCount = 1;
			BufferGPU* arrayBuffers[] = { g_ScreenRectBuffer };
			bufferData.ArrayBuffers = arrayBuffers;
			AttribDesc attribDesc[] =
			{
				{ 0, 2, DataType::FLOAT, false, sizeof(Vertex), 0, 0},
				{ 2, 2, DataType::FLOAT, false, sizeof(Vertex), 0, (GLvoid*)sizeof(glm::vec2)},
			};

			GeometryAttribDesc arraybufferDescs = { 2, attribDesc };
			bufferData.ArrayBufferDescs = &arraybufferDescs;
			bufferData.ElementBuffer = null;
			g_ScreenRectVAO->load(&bufferData);
		}

		static void releaseGLObjects()
		{
			if (g_TextureProgram)
			{
				delete g_TextureProgram;
				delete g_ScreenRectBuffer;
				delete g_ScreenRectVAO;

				g_TextureProgram = null;
				g_ScreenRectBuffer = null;
				g_ScreenRectVAO = null;
			}
		}

		static glm::mat4 createOrthoRectMat(const Rect& rect)
		{
			glm::mat4 ortho = glm::ortho(0.0f, g_Viewport.x, 0.0f, g_Viewport.y);
			glm::mat4 rectMat = glm::mat4();
			glm::mat4 rtMat = glm::mat4();

#if 0
			Rect right_region = { window.width / 2, window.height / 4, window.width / 2, window.height / 2 };
			viewMat.m00 = right_region.width * 0.5f;
			viewMat.m11 = right_region.height * 0.5f;
			viewMat.m30 = right_region.x + viewMat.m00;
			viewMat.m31 = right_region.y + viewMat.m11;
#endif
			Rectangle2f region;
			region.X = rect.X;
			region.Y = rect.Y;
			if (rect.Rotated)
			{
				region.Width = rect.Height;
				region.Height = rect.Width;

				const float angle90 = glm::radians(90.0f);
				const glm::vec3 zAxis(0, 0, 1);
				glm::mat4 trans_to_local = glm::mat4();
				trans_to_local[3] = { -region.X, -region.Y, 0.0f, 1.0f };
				
				glm::mat4 rotate90 = glm::rotate(rtMat, angle90, zAxis);
//				glm::mat4 translate = glm::mat4();
//				translate[3] = {region.Height,0,0,1};

				glm::mat4 trans_to_world = glm::mat4();
				trans_to_world[3] = { region.X + region.Height, region.Y, 0.0f, 1.0f };

				rtMat = trans_to_world /** translate*/ * rotate90 * trans_to_local;
			}
			else
			{
				region.Width = rect.Width;
				region.Height = rect.Height;
			}

			rectMat[0][0] = region.Width * 0.5f;
			rectMat[1][1] = region.Height * 0.5f;
			rectMat[3][0] = region.X + rectMat[0][0];
			rectMat[3][1] = region.Y + rectMat[1][1];

			return ortho * rtMat * rectMat;
		}

		void TexturePacker::generateTextureAtlas(class TextureAtlas* output)
		{
			if (m_pages.empty())
			{
				return;
			}
			FramebufferGL framebuffer;
			std::vector<Texture2D*> pTexs;
			std::vector<TextureAttachDesc> attachDesc;
			attachDesc.resize(m_pages.size());

			if (m_settings.arrayed)
			{
				int texWidth = 0, texHeight = 0;
				for (size_t i = 0; i < m_pages.size(); i++)
				{
					texWidth = Numeric::max(texWidth, m_pages[i]->Width);
					texHeight = Numeric::max(texHeight, m_pages[i]->Height);
					attachDesc[i].Index = i;
					attachDesc[i].Layer = i;
					attachDesc[i].Level = 0;
					attachDesc[i].Type = m_pages.size() > 1 ? AttachType::TEXTURE_LAYER : AttachType::TEXTURE_2D;
				}

				pTexs.resize(1);
				Texture2DDesc desc(texWidth, texHeight, m_settings.format);
				desc.ArraySize = m_pages.size();
				Texture2D* pTex = new Texture2D;
				TextureUtil::createTexture2D(&desc, null, pTex);
				pTexs[0] = pTex;
			}
			else
			{
				for (size_t i = 0; i < m_pages.size(); i++)
				{
					int texWidth = m_pages[i]->Width;
					int texHeight = m_pages[i]->Height;

					attachDesc[i].Index = i;
					attachDesc[i].Layer = 0;
					attachDesc[i].Level = 0;
					attachDesc[i].Type = AttachType::TEXTURE_2D;

					Texture2DDesc desc(texWidth, texHeight, m_settings.format);
					Texture2D* pTex = new Texture2D;
					TextureUtil::createTexture2D(&desc, null, pTex);
					pTexs.push_back(pTex);
				}
			}

			// Attach to framebuffer
			framebuffer.enable();
			for (size_t i = 0; i < m_pages.size(); i++)
			{
				const TextureGL* pTex[] = { m_settings.arrayed ? pTexs[0] : pTexs[i] };
				framebuffer.addTextures(1, pTex, &attachDesc[i]);
			}

			createGLObjects();
			g_TextureProgram->enable();

			// Render to the framebuffer.
			GLStates& states = GLStates::get();
			RasterizerDesc rsstate;
			rsstate.ColorWriteMask = { true, true, true, true };
			rsstate.CullFaceEnable = false;
			rsstate.FillMode = PolygonMode::FILL;
			rsstate.RasterizedDiscardEnable = false;

			BlendDesc bsstate;
			bsstate.IndependentBlendEnable = false;
			bsstate.RenderTargets[0].BlendEnable = true;
#if 0
			bsstate.RenderTargets[0].BlendOp = BlendOpFunc::ADD;
			bsstate.RenderTargets[0].BlendOpAlpha = BlendOpFunc::ADD;
			bsstate.RenderTargets[0].SrcBlend = BlendFunc::SRC_ALPHA;
			bsstate.RenderTargets[0].SrcBlendAlpha = BlendFunc::SRC_ALPHA;
#endif
			states.setRSState(&rsstate);
			states.setBlendState(&bsstate);
			states.setDSState(null);

			GLint iMVPMatLoc = g_TextureProgram->getUniformLocation("g_MVP");
			GLint iTexcoordMatLoc = g_TextureProgram->getUniformLocation("g_TexcoordMat");
			assert(iMVPMatLoc >= 0 && iTexcoordMatLoc >=0);

			framebuffer.enable();
			g_ScreenRectVAO->bind();
			for (size_t i = 0; i < m_pages.size(); i++)
			{
				Texture2D* pTex = (m_settings.arrayed ? pTexs[0] : pTexs[i]);
				glDrawBuffer(GL_COLOR_ATTACHMENT0 + i);
				setViewport(pTex->getWidth(), pTex->getHeight());

				RectPage* pPage = m_pages[i].get();
				const std::vector<Rect>& rectList = pPage->OutputRects;
				for (const Rect& rect : rectList)
				{
					const Texture2D* pSrc = getTexture(rect.UniqueID);
					assert(pSrc);

					glm::mat4 mvp = createOrthoRectMat(rect);
					glm::mat4 texcoordMat = glm::mat4();

					glProgramUniformMatrix4fv(g_TextureProgram->getProgram(), iMVPMatLoc, 1, false, (GLfloat*)(&mvp));
					glProgramUniformMatrix4fv(g_TextureProgram->getProgram(), iTexcoordMatLoc, 1, false, (GLfloat*)(&texcoordMat));

					const TextureGL* texSrc[] = { pSrc };
					states.bindTextures(1, texSrc);
//					glActiveTexture(GL_TEXTURE0);
//					glBindTexture(GL_TEXTURE_2D, texSrc[0]->getTexture());

					glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

					TextureRegion region;
					region.Layer = m_settings.arrayed ? i : 0;
					region.pTexture = pTex;
					region.Region = { rect.X, rect.Y, rect.Width, rect.Height };
					region.Rotated = rect.Rotated;

					std::string name = m_idsToNames[rect.UniqueID];

					output->m_textureRegions.insert(std::pair<std::string, TextureRegion>(name, region));
				}

				if (m_settings.arrayed)
				{
					if (i == 0)
						output->m_textures.push_back(pTex);
				}
				else
				{
					output->m_textures.push_back(pTex);
				}
			}

			g_ScreenRectVAO->unbind();
			framebuffer.disable();
			g_TextureProgram->disable();

			releaseGLObjects();
		}
	}
};