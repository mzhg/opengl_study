#include "PostProcessingFramework.h"

namespace jet
{
	namespace util
	{
		RenderTargetPool* RenderTargetPool::m_Instance = NULL;

		RenderTargetPool::~RenderTargetPool()
		{
			m_RenderTargetPool.clear();
		}

		RenderTargetPool* RenderTargetPool::getInstance()
		{
			if (m_Instance == NULL)
			{
				m_Instance = new RenderTargetPool();
			}

			return m_Instance;
		}

		void RenderTargetPool::shutDown()
		{
			if (m_Instance != NULL)
			{
				delete m_Instance;
				m_Instance = NULL;
			}
		}

		void RenderTargetPool::findFreeElement(const Texture2DDesc& pDesc, std::unique_ptr<RenderTarget>& out)
		{
			auto it = m_RenderTargetPool.find(pDesc);
			if (it == m_RenderTargetPool.end())
			{
				RenderTarget* pOut = new RenderTarget();
				TextureUtil::createTexture2D(&pDesc, NULL, pOut);

				out = std::unique_ptr<RenderTarget>(pOut);

//				printf("Create a new RenderTexture: [width, height, format] = [%d, %d, %s]\n", pDesc.Width, pDesc.Height, TextureUtil::getFormatName(pDesc.Format));
			}
			else
			{
				out = std::move(it->second);
				m_RenderTargetPool.erase(it);

//				printf("Retrive a RenderTexture From Pool: [width, height, format] = [%d, %d, %s]\n", pDesc.Width, pDesc.Height, TextureUtil::getFormatName(pDesc.Format));
//				printf("The Total Caches: %d\n", m_RenderTargetPool.size());
			}
		}

		void RenderTargetPool::freeUnusedResource(std::unique_ptr<RenderTarget>& in)
		{
			Texture2DDesc desc = in->getDesc();
//			m_RenderTargetPool[desc] = std::move(in);
			m_RenderTargetPool.insert(std::pair<Texture2DDesc, std::unique_ptr<RenderTarget>>(desc, std::move(in)));

//			printf("Collection a RenderTexture: [width, height, format] = [%d, %d, %s]\n", desc.Width, desc.Height, TextureUtil::getFormatName(desc.Format));
//			printf("The Total Caches: %d\n", m_RenderTargetPool.size());
		}

		void PPRenderPass::drawQuad()
		{
			// TODO Unimplemented!
		}

		void PPRenderContext::initlizeGL()
		{
			{
				m_Program = ProgramPipeline::create();
				m_pDefaultScreenQuadVS = new DefaultScreenQuadVertexShader();
				m_pDefaultScreenQuadVS->init();

				GLVersion version = GetOpenGLVersion();
				if (version.es)
				{
					m_IsSupportVertexArray = version.toInt() >= 300;
					m_IsSupportVertexID = version.toInt() >= 300;
				}
				else
				{
					m_IsSupportVertexArray = version.toInt() >= 300;
					m_IsSupportVertexID = version.toInt() >= 300;
				}
				glGenFramebuffers(1, &m_Framebuffer);

				if (m_IsSupportVertexArray)
				{
					glGenVertexArrays(1, &m_VertexArray);
				}

				if (!m_IsSupportVertexID)
				{
					{ // build the Vertex Buffer.
						GLfloat QuadVertices[4][2] = //vec2[4]
						{
							{ -1.0, -1.0 },
							{ 1.0, -1.0 },
							{ -1.0, 1.0 },
							{ 1.0, 1.0 }
						};

						glGenBuffers(1, &m_VertexBuffer);
						glBindBuffer(GL_VERTEX_ARRAY, m_VertexBuffer);
						glBufferData(GL_VERTEX_ARRAY, 8 * sizeof(GLfloat), QuadVertices, GL_STATIC_DRAW);
					}

					// build the Vertex Array if supported.
					if (m_IsSupportVertexArray)
					{
						glBindVertexArray(m_VertexArray);
						{
							glBindBuffer(GL_VERTEX_ARRAY, m_VertexBuffer);
							glEnableVertexAttribArray(0);
							glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);
						}

						glBindVertexArray(0);
					}
					glBindBuffer(GL_VERTEX_ARRAY, 0);
				}
			}
		}

		void PPRenderContext::begin()
		{
			{
				m_InBeginBlock = true;
				// Save the current viewport
				glGetIntegerv(GL_VIEWPORT, &m_Viewport[0]);
				glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
				if (m_IsSupportVertexArray)
				{
					glBindVertexArray(m_VertexArray);
				}
				else
				{
					glBindBuffer(GL_VERTEX_ARRAY, m_VertexBuffer);
					glEnableVertexAttribArray(0);
					glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);
				}
				m_Program->enable();
			}
		}

		void PPRenderContext::setRenderTargets(uint32_t count, const RenderTarget** pColorTargets, const RenderTarget* pDepthStencilTarget)
		{
			assert(m_InBeginBlock);

			m_BindingColorTextureCount = count;
			m_HaveDepthStencilBinding = (pDepthStencilTarget != NULL);

			GLenum* drawbuffers = NULL;
			if (count)
			{
				drawbuffers = new GLenum[count];
			}

			for (int i = 0; i < count; i++)
			{
				const RenderTarget* colorTarget = pColorTargets[i];
				m_BindingColorTextureDesc[i] = colorTarget->getDesc();

				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, colorTarget->getTarget(), colorTarget->getTexture(), 0);
				drawbuffers[i] = GL_COLOR_ATTACHMENT0 + i;
			}

			if (pDepthStencilTarget != NULL)
			{
				m_BindingDepthTextureDesc = pDepthStencilTarget->getDesc();
				GLenum attachment;
				GLenum componemt = TextureUtil::measureComponent(m_BindingDepthTextureDesc.Format);
				if (componemt == GL_DEPTH_COMPONENT)
				{
					attachment = GL_DEPTH_ATTACHMENT;
				}
				else if (componemt == GL_DEPTH_STENCIL)
				{
					attachment = GL_DEPTH_STENCIL_ATTACHMENT;
				}
				else
				{
					//Invalid texture formats
					assert(false);
				}

				glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, pDepthStencilTarget->getTarget(), pDepthStencilTarget->getTexture(), 0);
			}

			int viewportWidth, viewportHeight;
#if defined(_DEBUG) || defined(DEBUG)
			// check the textrue dimensions validation.
			if (m_BindingColorTextureCount > 1 || (m_BindingColorTextureCount == 1 && m_HaveDepthStencilBinding))
			{
				viewportWidth = m_BindingColorTextureDesc[0].Width;
				viewportHeight = m_BindingColorTextureDesc[0].Height;

				for (GLuint i = 1; i < m_BindingColorTextureCount; i++)
				{
					Texture2DDesc& destDesc = m_BindingColorTextureDesc[i];
					if (destDesc.Width != viewportWidth || destDesc.Height != viewportHeight)
					{
						printf("The textures' dimension doest not match. (0, %d)\n", i);
					}
				}

				if (m_HaveDepthStencilBinding)
				{
					Texture2DDesc& destDesc = m_BindingDepthTextureDesc;
					if (destDesc.Width != viewportWidth || destDesc.Height != viewportHeight)
					{
						printf("The dimensions of the color texture and depth texture doest not match.\n");
					}
				}
			}
			else
#endif
			{
				Texture2DDesc* pDesc = m_BindingColorTextureCount >= 1 ? &m_BindingColorTextureDesc[0] :
					m_HaveDepthStencilBinding ? &m_BindingDepthTextureDesc : NULL;
				if (pDesc != NULL)
				{
					viewportWidth = pDesc->Width;
					viewportHeight = pDesc->Height;
				}
			}
			// setup viewport
			if (m_BindingColorTextureCount > 0 || m_HaveDepthStencilBinding)
			{
				glViewport(0, 0, viewportWidth, viewportHeight);
			}

			// setup drawbuffers
			if (drawbuffers)
			{
				glDrawBuffers(m_BindingColorTextureCount, drawbuffers);

				delete[] drawbuffers;
			}
			else
			{
				glDrawBuffer(GL_NONE);
			}
		}

		void PPRenderContext::clearColorTarget(uint32_t location, const GLfloat clearColors[4])
		{
			assert(m_InBeginBlock);
			if (location < m_BindingColorTextureCount)
			{
				glClearBufferfv(GL_COLOR, location, clearColors);
			}
		}

		void PPRenderContext::clearDepthStencilTarget(float clearDepth, int clearStencil)
		{
			assert(m_InBeginBlock);
			if (m_HaveDepthStencilBinding)
			{
				GLenum bufferType;
				GLenum componemt = TextureUtil::measureComponent(m_BindingDepthTextureDesc.Format);
				if (componemt == GL_DEPTH_COMPONENT)
				{
					bufferType = GL_DEPTH;
				}
				else if (componemt == GL_DEPTH_STENCIL)
				{
					bufferType = GL_DEPTH_STENCIL;
				}
				else
				{
					//Invalid texture formats
					assert(false);
				}

				glClearBufferfi(bufferType, 0, clearDepth, clearStencil);
			}
		}

		void PPRenderContext::drawQuad()
		{
			assert(m_InBeginBlock);
	//		if (!m_IsRendered)
			{
				glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
				m_IsRendered = true;
			}

		}

		// unbind resources.
		void PPRenderContext::end(bool renderQuad)
		{
			assert(m_InBeginBlock);
			if (renderQuad)
			{
				drawQuad();
			}

			// restore the viewport
			glViewport(m_Viewport[0], m_Viewport[1], m_Viewport[2], m_Viewport[3]);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			if (m_IsSupportVertexArray)
			{
				glBindVertexArray(0);
			}
			else
			{
				glBindBuffer(GL_VERTEX_ARRAY, 0);
				glDisableVertexAttribArray(0);
			}
			m_Program->disable();
			m_InBeginBlock = false;
		}

		void PPRenderContext::renderTo(Texture2D* src, Texture2D* dst, PPRectangle viewport)
		{
			if (m_pDefualtScreenQuadPS == NULL)
			{
				m_pDefualtScreenQuadPS = new PixelShader();
				GLSLProgram::createShaderFromFile("DefaultScreenSpacePS.frag", m_pDefualtScreenQuadPS);
			}

			if (dst != NULL)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, dst->getTarget(), dst->getTexture(), 0);
				glDrawBuffer(GL_COLOR_ATTACHMENT0);
			}
			else
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
			}
			
			glViewport(viewport.X, viewport.Y, viewport.Width, viewport.Height);
			glDisable(GL_DEPTH_TEST);

			AttribBinding attribs = { "aPosition", 0 };
			m_Program->setPreLoadAttribs(1, &attribs);
			m_Program->enable();
			m_Program->setRenderShader(m_pDefaultScreenQuadVS, m_pDefualtScreenQuadPS);
			m_Program->setUniform1i("g_Texture", 0);

			glActiveTexture(GL_TEXTURE0);	
			glBindTexture(src->getTarget(), src->getTexture());

			if (m_IsSupportVertexArray)
			{
				glBindVertexArray(m_VertexArray);
			}
			else
			{
				glBindBuffer(GL_VERTEX_ARRAY, m_VertexBuffer);
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const void*)0);
			}

			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

			if (m_IsSupportVertexArray)
			{
				glBindVertexArray(0);
			}
			else
			{
				glBindBuffer(GL_VERTEX_ARRAY, 0);
				glDisableVertexAttribArray(0);
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			m_Program->disable();
		}

		void PPRenderContext::shutDown()
		{
			SAFE_DISPOSE(m_Program);
			SAFE_DISPOSE(m_pDefaultScreenQuadVS);
			SAFE_DISPOSE(m_pDefualtScreenQuadPS);
			SAFE_RELEASE_FRAMEBUFFER(m_Framebuffer);
			SAFE_RELEASE_BUFFER(m_VertexBuffer);
			if (m_IsSupportVertexArray)
			{
				SAFE_RELEASE_VERTEX_ARRAY(m_VertexArray);
			}
		}

		void PPRenderContext::performancePostProcessing()
		{
			for (auto it = m_RenderPassList.begin(); it != m_RenderPassList.end(); it++)
			{
				(*it)->reset();
			}

			Texture2DDesc outputDesc;
			for (auto it = m_RenderPassList.begin(); it != m_RenderPassList.end(); it++)
			{
				m_CurrentPass = (*it);
				if (m_CurrentPass->m_bProcessed)
				{
					continue;
				}


				std::vector<PPRenderPass*> currentDependencyPasses;
				// Prepare the input textures for the current Post-Processing-Pass.
				PPRenderPassDesc desc = m_CurrentPass->getPassDesc();

				Texture2D** inputTextures = NULL;
				if (desc.InputCount)
				{
					inputTextures = new Texture2D*[desc.InputCount];
				}

				for (int i = 0; i < desc.InputCount; i++)
				{
					InputDesc inputDesc = desc.InputDescs[i];
					if (inputDesc.DependencyPass != INPUT_NONE)
					{
						auto inputPassIt = m_RenderPasses.find(inputDesc.DependencyPass);
						assert(inputPassIt != m_RenderPasses.end());
						PPRenderPass* inputPass = inputPassIt->second.get();

						currentDependencyPasses.push_back(inputPass);
						inputTextures[i] = inputPass->getOutput(inputDesc.Slot);
					}
				}

				m_CurrentPass->setInputTextures(inputTextures);
				for (int i = 0; i < desc.OutputCount; i++)
				{
					m_CurrentPass->computeOutDesc(i, outputDesc);
					std::unique_ptr<RenderTarget> temp;
					RenderTargetPool::getInstance()->findFreeElement(outputDesc, temp);
					m_CurrentPass->setOutputRenderTexture(i, temp);
				}

				m_CurrentPass->_process(this, *m_Parameters);
				resolveDependencies(currentDependencyPasses);
				m_PrevPass = m_CurrentPass;

				if (inputTextures)
				{
					delete[] inputTextures;
				}
			}

			m_CurrentPass.reset();
			m_PrevPass.reset();
		}
	}
}
