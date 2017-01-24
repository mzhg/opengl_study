#include "GLStates.h"
#include <GLCapabilities.h>
#include "Numeric.h"

namespace jet
{
	namespace util
	{

		static const BlendDesc g_BlendDefault = BlendDesc();
		static const DepthStencilDesc g_DSStateDefault = DepthStencilDesc();
		static const RasterizerDesc g_RSStateDefault = RasterizerDesc();

		static const GLCapabilities* g_Caps = GLCapabilities::getGLCapabilities();

		GLStates& GLStates::getGLStates()
		{
			static GLStates instance;
			return instance;
		}

		void GLStates::setBlendState(const BlendDesc* pBlend)
		{
			if (pBlend)
			{
				setBlendState(*pBlend);
			}
			else
			{
				setBlendState(g_BlendDefault);
			}
		}

		

		void GLStates::restoreBlendState()
		{
#if 1
			for (int i = 0; i < 8; i++)
#else
			for (int i = 0; i < 1; i++)
#endif
			{
				{
					GLint value;
					glGetIntegeri_v(GL_BLEND_EQUATION_RGB,i, &value);
					m_BlendState.RenderTargets[i].BlendOp = ConvertGLenumToBlendOpFunc(value);
				}

				{
					GLint value;
					glGetIntegeri_v(GL_BLEND_EQUATION_ALPHA, i, &value);
					m_BlendState.RenderTargets[i].BlendOpAlpha = ConvertGLenumToBlendOpFunc(value);
				}

				{
					GLint value;
					glGetIntegeri_v(GL_BLEND_SRC_RGB, i, &value);
					m_BlendState.RenderTargets[i].SrcBlend = ConvertGLenumToBlendFunc(value);
				}

				{
					GLint value;
					glGetIntegeri_v(GL_BLEND_SRC_ALPHA, i, &value);
					m_BlendState.RenderTargets[i].SrcBlendAlpha = ConvertGLenumToBlendFunc(value);
				}

				{
					GLint value;
					glGetIntegeri_v(GL_BLEND_DST_RGB, i, &value);
					m_BlendState.RenderTargets[i].DestBlend = ConvertGLenumToBlendFunc(value);
				}

				{
					GLint value;
					glGetIntegeri_v(GL_BLEND_DST_ALPHA, i, &value);
					m_BlendState.RenderTargets[i].DestBlendAlpha = ConvertGLenumToBlendFunc(value);
				}

				{
					m_BlendState.RenderTargets[i].BlendEnable = glIsEnabledi(GL_BLEND, i);
				}
			}

			{
				m_BlendState.AlphaToCoverageEnable = glIsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
			}
		}

		void GLStates::resetBlendState(bool force)
		{
			if (force)
			{
				glDisable(GL_BLEND);
				glBlendFunc(GL_ONE, GL_ZERO);
				glBlendEquation(GL_FUNC_ADD);
				glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);

				m_BlendState.AlphaToCoverageEnable = false;
				m_BlendState.IndependentBlendEnable = false;
				for (int i = 0; i < 8; i++)
				{
					m_BlendState.RenderTargets[i] = RenderTargetBlendDesc();
				}
			}
			else
			{
				if (m_BlendState.AlphaToCoverageEnable)
				{
					glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
					m_BlendState.AlphaToCoverageEnable = false;
				}

				if (m_BlendState.IndependentBlendEnable)
				{
					for (int i = 0; i < 8; i++)
					{
						if (m_BlendState.RenderTargets[i].BlendEnable)
						{
							glDisablei(GL_BLEND, i);
							m_BlendState.RenderTargets[i].BlendEnable = false;
						}
					}
				}
				else
				{
					if (m_BlendState.RenderTargets[0].BlendEnable)
					{
						glDisable(GL_BLEND);
						m_BlendState.RenderTargets[0].BlendEnable = false;
					}
				}
			}
		}

		void GLStates::setDSState(const DepthStencilDesc* pDS)
		{
			if (pDS)
			{
				setDSState(*pDS);
			}
			else
			{
				setDSState(g_DSStateDefault);
			}
		}

		void GLStates::restoreDSState()
		{
			m_DepthStencilState.DepthEnable = glIsEnabled(GL_DEPTH_TEST);
			{
				GLboolean value;
				glGetBooleanv(GL_DEPTH_WRITEMASK, &value);
				m_DepthStencilState.DepthWriteMask = value;
			}

			{
				GLint value;
				glGetIntegerv(GL_DEPTH_FUNC, &value);
				m_DepthStencilState.DepthFunc = ConvertGLenumToCompareFunc(value);
			}

			{
				m_DepthStencilState.StencilEnable = glIsEnabled(GL_STENCIL_TEST);
			}

			{
				GLint value;
				glGetIntegerv(GL_STENCIL_FAIL, &value);
				m_DepthStencilState.FrontFace.StencilFailOp = ConvertGLenumToStencilOp(value);
			}

			{
				GLint value;
				glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS, &value);
				m_DepthStencilState.FrontFace.StencilPassOp = ConvertGLenumToStencilOp(value);
			}

			{
				GLint value;
				glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL, &value);
				m_DepthStencilState.FrontFace.StencilDepthFailOp = ConvertGLenumToStencilOp(value);
			}

			{
				GLint value;
				glGetIntegerv(GL_STENCIL_BACK_FAIL, &value);
				m_DepthStencilState.BackFace.StencilFailOp = ConvertGLenumToStencilOp(value);
			}

			{
				GLint value;
				glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS, &value);
				m_DepthStencilState.BackFace.StencilPassOp = ConvertGLenumToStencilOp(value);
			}

			{
				GLint value;
				glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL, &value);
				m_DepthStencilState.BackFace.StencilDepthFailOp = ConvertGLenumToStencilOp(value);
			}

			{
				GLint value;
				glGetIntegerv(GL_STENCIL_FUNC, &value);
				m_DepthStencilState.FrontFace.StencilFunc = ConvertGLenumToCompareFunc(value);
			}

			{
				glGetIntegerv(GL_STENCIL_VALUE_MASK, &m_DepthStencilState.FrontFace.StencilMask);
			}

			{
				glGetIntegerv(GL_STENCIL_REF, &m_DepthStencilState.FrontFace.StencilRef);
			}

			{
				GLint value;
				glGetIntegerv(GL_STENCIL_BACK_FUNC, &value);
				m_DepthStencilState.BackFace.StencilFunc = ConvertGLenumToCompareFunc(value);
			}

			{
				glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK, &m_DepthStencilState.BackFace.StencilMask);
			}

			{
				glGetIntegerv(GL_STENCIL_BACK_REF, &m_DepthStencilState.BackFace.StencilRef);
			}

			{
				glGetIntegerv(GL_STENCIL_WRITEMASK, &m_DepthStencilState.FrontFace.StencilWriteMask);
			}

			{
				glGetIntegerv(GL_STENCIL_BACK_WRITEMASK, &m_DepthStencilState.BackFace.StencilWriteMask);
			}
		}
		void GLStates::resetDSState(bool force)
		{
			if (force)
			{
				glDisable(GL_DEPTH_TEST);
				glDepthFunc(GL_LESS);
				glDepthMask(true);

				glDisable(GL_STENCIL_TEST);
				glStencilFunc(GL_ALWAYS, 0, 0xFF);
				glStencilMask(0xFF);
				glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

				m_DepthStencilState = g_DSStateDefault;
			}
			else
			{
				setDSState(g_DSStateDefault);
			}
		}

		void GLStates::setRSState(const RasterizerDesc* pRaster)
		{
			if (pRaster)
			{
				setRSState(*pRaster);
			}
			else
			{
				setRSState(g_RSStateDefault);
			}
		}

		void GLStates::restoreRSState()
		{
			{
				GLint value;
				glGetIntegerv(GL_POLYGON_MODE, &value);
				m_RasterizerState.FillMode = ConvertGLenumToPolygonMode(value);
			}

			{
				GLint value;
				glGetIntegerv(GL_CULL_FACE_MODE, &value);
				m_RasterizerState.CullMode = ConvertGLenumToFaceMode(value);
			}

			{
				m_RasterizerState.CullFaceEnable = glIsEnabled(GL_CULL_FACE);
			}

			{
				GLint value;
				glGetIntegerv(GL_FRONT_FACE, &value);
				m_RasterizerState.FrontCounterClockwise = (value == GL_CW);
			}

			{
				glGetDoublev(GL_DEPTH_RANGE, (GLdouble*)&m_RasterizerState.DepthRange);
			}

			{
				m_RasterizerState.DepthClampEnable = glIsEnabled(GL_DEPTH_CLAMP);
			}

			{
				m_RasterizerState.ScissorEnable = glIsEnabled(GL_SCISSOR_TEST);
			}

			{
				glGetIntegerv(GL_SCISSOR_BOX, (GLint*)&m_RasterizerState.ScissorBox);
			}

			{
				m_RasterizerState.MultisampleEanble = glIsEnabled(GL_MULTISAMPLE);
			}

			{
				m_RasterizerState.AntialiasedLineEnable = glIsEnabled(GL_LINE_SMOOTH);
			}

			{
				m_RasterizerState.AntialiasedPolygonEnable = glIsEnabled(GL_POLYGON_SMOOTH);
			}

			{
				m_RasterizerState.Dither = glIsEnabled(GL_DITHER);
				m_RasterizerState.SRGB = glIsEnabled(GL_FRAMEBUFFER_SRGB);
				m_RasterizerState.PolygonOffsetEnable = glIsEnabled(GL_DITHER);
				m_RasterizerState.Dither = glIsEnabled(GL_DITHER);
				m_RasterizerState.PolygonOffsetEnable = glIsEnabled(GL_POLYGON_OFFSET_FILL) || glIsEnabled(GL_POLYGON_OFFSET_LINE) || glIsEnabled(GL_POLYGON_OFFSET_POINT);
				m_RasterizerState.RasterizedDiscardEnable = glIsEnabled(GL_RASTERIZER_DISCARD);
				m_RasterizerState.ProgramPointSizeEnable = glIsEnabled(GL_PROGRAM_POINT_SIZE);
				m_RasterizerState.PrimitiveRestartEnable = glIsEnabled(GL_PRIMITIVE_RESTART);
#if 0
				for (int i = 0; i < 6; i++)
				{
					m_RasterizerState.ClipPlanes[i].ClipPlaneEnable = glIsEnabled(GL_CLIP_DISTANCE0);
				}
#endif			
				glGetFloatv(GL_POLYGON_OFFSET_FACTOR, &m_RasterizerState.PolygonOffset.factor);
				glGetFloatv(GL_POLYGON_OFFSET_UNITS,  &m_RasterizerState.PolygonOffset.units);

				m_RasterizerState.LogicMode.LogicEnable = glIsEnabled(GL_COLOR_LOGIC_OP);
				GLint value;
				glGetIntegerv(GL_LOGIC_OP_MODE, &value);
				m_RasterizerState.LogicMode.LogicOp = ConvertGLenumToLogicFunc(value);
			}
		}

		void GLStates::resetRSState(bool force)
		{
			if (force)
			{
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
				glCullFace(GL_BACK);
				glFrontFace(GL_CCW);
				glDisable(GL_DEPTH_CLAMP);
				glDepthRange(0.0, 1.0);
				glDisable(GL_SCISSOR_TEST);
				glDisable(GL_MULTISAMPLE);
				glDisable(GL_LINE_SMOOTH);
				glDisable(GL_POLYGON_SMOOTH);

				glDisable(GL_DITHER);
				glDisable(GL_FRAMEBUFFER_SRGB);
				glDisable(GL_POLYGON_OFFSET_FILL);
				glDisable(GL_POLYGON_OFFSET_LINE);
				glDisable(GL_POLYGON_OFFSET_POINT);
				glDisable(GL_RASTERIZER_DISCARD);
				glDisable(GL_PRIMITIVE_RESTART);
				glPolygonOffset(0.0f, 0.0f);
				glDisable(GL_COLOR_LOGIC_OP);
				glLogicOp(GL_COPY);

				m_RasterizerState = g_RSStateDefault;
			}
			else
			{
				setRSState(g_RSStateDefault);
			}
		}

		void GLStates::bindProgram(GLuint program)
		{
			if (m_ProgramState != program)
			{
				glUseProgram(program);
				m_ProgramState = program;
			}
		}

		void GLStates::restoreProgram()
		{
			glGetIntegerv(GL_CURRENT_PROGRAM, (GLint*)&m_ProgramState);
		}

		void GLStates::resetProgram(bool force)
		{
			if (force || m_ProgramState != 0)
			{
				glUseProgram(0);
			}
			
			m_ProgramState = 0;
		}

		void GLStates::bindFramebuffer(GLuint framebuffer)
		{
			if (m_FramebufferState != framebuffer)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
				m_FramebufferState = framebuffer;
			}
		}

		void GLStates::restoreFramebuffer()
		{
			glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&m_FramebufferState);
		}

		void GLStates::resetFramebuffer(bool force)
		{
			if (force || m_FramebufferState != 0)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferState);
			}

			m_FramebufferState = 0;
		}

		void GLStates::bindTextures(unsigned int count, const TextureGL** pTextures, const int* units)
		{
			static GLuint textureNames[80];
			for (unsigned int i = 0; i < count; i++)
			{
				textureNames[i] = pTextures && pTextures[i] ? pTextures[i]->getTexture() : 0;
				m_TextureStates[i].Target = pTextures && pTextures[i] ? pTextures[i]->getTarget() : 0;
				m_TextureStates[i].TextureID = textureNames[i];
				m_TextureStates[i].Unit = units ? units[i] : i;
			}

			if (units == nullptr)
			{
				if (glBindTextures)
				{
					glBindTextures(0, count, textureNames);
				}
				else
				{
					for (unsigned int i = 0; i < count; i++)
					{
						glActiveTexture(GL_TEXTURE0 + i);
						glBindTexture(m_TextureStates[i].Target, m_TextureStates[i].TextureID);
					}
				}
			}
			else
			{
				if (glBindTextureUnit)
				{
					for (unsigned int i = 0; i < count; i++)
					{
						glBindTextureUnit(m_TextureStates[i].Unit, m_TextureStates[i].TextureID);
					}
				}
				else
				{
					for (unsigned int i = 0; i < count; i++)
					{
						glActiveTexture(GL_TEXTURE0 + units[i]);
						glBindTexture(m_TextureStates[i].Target, m_TextureStates[i].TextureID);
					}
				}
			}
		}

		void GLStates::restoreTextures()
		{
			// TODO
		}
		void GLStates::resetTextures(bool force)
		{
			// TODO
		}

		void GLStates::setViewports(unsigned int count, const Rectangle2i* pViewports)
		{
			if (count == 1)
			{
				if (m_ViewportStates[0] != pViewports[0])
				{
					glViewport(pViewports[0].X, pViewports[0].Y, pViewports[0].Width, pViewports[0].Height);
					m_ViewportStates[0] = pViewports[0];
				}
			}
			else if (count)
			{
				count = Numeric::min(count, 16u);
				if (glViewportArrayv)
				{
					glViewportArrayv(0, count, (const GLfloat*)pViewports);
					memcpy(m_ViewportStates, pViewports, sizeof(Rectangle2i) * count);
				}
				else
				{
					glViewport(pViewports[0].X, pViewports[0].Y, pViewports[0].Width, pViewports[0].Height);
					m_ViewportStates[0] = pViewports[0];
				}
			}
		}
		void GLStates::restoreViewport(unsigned int index)
		{
			glGetIntegeri_v(GL_VIEWPORT, index, (GLint *)&m_ViewportStates[index]);
		}

		void GLStates::bindVAO(GLuint vao)
		{
			if (m_VertexArrayState != vao)
			{
				glBindVertexArray(vao);
				m_VertexArrayState = vao;
			}
		}
		void GLStates::restoreVAO()
		{
			glGetIntegerv(GL_VERTEX_ARRAY_BINDING, (GLint*)&m_VertexArrayState);
		}
		void GLStates::resetVAO(bool force)
		{
			if (m_VertexArrayState != 0 || force)
			{
				glBindVertexArray(0);
			}

			m_VertexArrayState = 0;
		}

		template<bool Independent>
		static void setBlendStateGL(GLuint buf, RenderTargetBlendDesc& dst, const RenderTargetBlendDesc& src)
		{
			if (dst.BlendEnable != src.BlendEnable)
			{
				dst.BlendEnable = src.BlendEnable;
				if (src.BlendEnable)
				{
					if (Independent)
					{
						glEnablei(GL_BLEND, buf);
					}
					else
					{
						glEnable(GL_BLEND);
					}
				}
				else
				{
					if (Independent)
					{
						glDisablei(GL_BLEND, buf);
					}
					else
					{
						glDisable(GL_BLEND);
					}

					return;
				}
			}

			if (dst.SrcBlend != src.SrcBlend || 
				dst.SrcBlendAlpha != src.SrcBlendAlpha || 
				dst.DestBlend != src.DestBlend || 
				dst.DestBlendAlpha != src.DestBlendAlpha)
			{
				if(Independent)
				{
					glBlendFuncSeparatei(buf, ConvertBlendFuncToGLenum(src.SrcBlend), ConvertBlendFuncToGLenum(src.DestBlend), 
						ConvertBlendFuncToGLenum(src.SrcBlendAlpha), ConvertBlendFuncToGLenum(src.DestBlendAlpha));
				}
				else
				{
					glBlendFuncSeparate(ConvertBlendFuncToGLenum(src.SrcBlend), ConvertBlendFuncToGLenum(src.DestBlend), 
						ConvertBlendFuncToGLenum(src.SrcBlendAlpha), ConvertBlendFuncToGLenum(src.DestBlendAlpha));
				}
				
				dst.SrcBlend = src.SrcBlend;
				dst.SrcBlendAlpha = src.SrcBlendAlpha;
				dst.DestBlend = src.DestBlend;
				dst.DestBlendAlpha = src.DestBlendAlpha;
			}

			if (dst.BlendOp != src.BlendOp || dst.BlendOpAlpha != src.BlendOpAlpha)
			{
				if (Independent)
				{
					glBlendEquationSeparatei(buf, ConvertBlendOpFuncToGLenum(src.BlendOp), ConvertBlendOpFuncToGLenum(src.BlendOpAlpha));
				}
				else
				{
					glBlendEquationSeparate(ConvertBlendOpFuncToGLenum(src.BlendOp), ConvertBlendOpFuncToGLenum(src.BlendOpAlpha));
				}

				dst.BlendOp = src.BlendOp;
				dst.BlendOpAlpha = src.BlendOpAlpha;
			}
		}

		void GLStates::setBlendState(const BlendDesc& blend)
		{
			if (m_BlendState.AlphaToCoverageEnable != blend.AlphaToCoverageEnable)
			{
				if (blend.AlphaToCoverageEnable)
				{
					glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				}
				else
				{
					glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				}

				m_BlendState.AlphaToCoverageEnable = blend.AlphaToCoverageEnable;
			}

			if (blend.IndependentBlendEnable)
			{
				m_BlendState.IndependentBlendEnable = true;
				for (GLuint i = 0; i < 8u; i++)
				{
					setBlendStateGL<true>(i, m_BlendState.RenderTargets[i], blend.RenderTargets[i]);
				}
			}
			else
			{
				setBlendStateGL<false>(0, m_BlendState.RenderTargets[0], blend.RenderTargets[0]);
				m_BlendState.IndependentBlendEnable = false;
			}
		}
		void GLStates::setDSState(const DepthStencilDesc& ds)
		{
			if (m_DepthStencilState.DepthWriteMask != ds.DepthWriteMask)
			{
				if (ds.DepthWriteMask)
				{
					glDepthMask(GL_TRUE);
				}
				else
				{
					glDepthMask(GL_FALSE);
				}
			}

			if (m_DepthStencilState.DepthEnable != ds.DepthEnable)
			{
				if (ds.DepthEnable)
				{
					glEnable(GL_DEPTH_TEST);

					if (m_DepthStencilState.DepthFunc != ds.DepthFunc)
					{
						glDepthFunc(ConvertCompareFuncToGLenum(ds.DepthFunc));
						m_DepthStencilState.DepthFunc = ds.DepthFunc;
					}
				}
				else
				{
					glDisable(GL_DEPTH_TEST);
				}

				m_DepthStencilState.DepthEnable = ds.DepthEnable;
			}

			if (m_DepthStencilState.StencilEnable != ds.StencilEnable)
			{
				m_DepthStencilState.StencilEnable = ds.StencilEnable;
				if (ds.StencilEnable)
				{
					glEnable(GL_STENCIL_TEST);
				}
				else
				{
					glDisable(GL_STENCIL_TEST);
					return;
				}
			}

			if (m_DepthStencilState.FrontFace.StencilWriteMask != ds.FrontFace.StencilWriteMask || 
				m_DepthStencilState.BackFace.StencilWriteMask  != ds.BackFace.StencilWriteMask)
			{
				if (ds.FrontFace.StencilWriteMask != ds.BackFace.StencilWriteMask)
				{
					glStencilMaskSeparate(GL_FRONT, ds.FrontFace.StencilWriteMask);
					glStencilMaskSeparate(GL_BACK,  ds.BackFace.StencilWriteMask);
				}
				else
				{
					glStencilMask(ds.FrontFace.StencilWriteMask);
				}

				m_DepthStencilState.FrontFace.StencilWriteMask = ds.FrontFace.StencilWriteMask;
				m_DepthStencilState.BackFace.StencilWriteMask = ds.BackFace.StencilWriteMask;
			}

			if (m_DepthStencilState.FrontFace.StencilFunc != ds.FrontFace.StencilFunc ||
				m_DepthStencilState.FrontFace.StencilMask != ds.FrontFace.StencilMask ||
				m_DepthStencilState.FrontFace.StencilRef  != ds.FrontFace.StencilRef ||
				m_DepthStencilState.BackFace.StencilFunc  != ds.BackFace.StencilFunc ||
				m_DepthStencilState.BackFace.StencilMask  != ds.BackFace.StencilMask ||
				m_DepthStencilState.BackFace.StencilRef   != ds.BackFace.StencilRef )
			{
				if (ds.FrontFace.StencilFunc != ds.BackFace.StencilFunc ||
					ds.FrontFace.StencilMask != ds.BackFace.StencilMask ||
					ds.FrontFace.StencilRef != ds.BackFace.StencilRef)
				{
					glStencilFuncSeparate(GL_FRONT, ConvertCompareFuncToGLenum(ds.FrontFace.StencilFunc), ds.FrontFace.StencilRef, ds.FrontFace.StencilMask);
					glStencilFuncSeparate(GL_BACK,  ConvertCompareFuncToGLenum(ds.BackFace.StencilFunc), ds.BackFace.StencilRef, ds.BackFace.StencilMask);
				}
				else
				{
					glStencilFunc(ConvertCompareFuncToGLenum(ds.FrontFace.StencilFunc), ds.FrontFace.StencilRef, ds.FrontFace.StencilMask);
				}

				m_DepthStencilState.FrontFace.StencilFunc = ds.FrontFace.StencilFunc;
				m_DepthStencilState.FrontFace.StencilMask = ds.FrontFace.StencilMask;
				m_DepthStencilState.FrontFace.StencilRef = ds.FrontFace.StencilRef;
				m_DepthStencilState.BackFace.StencilFunc = ds.BackFace.StencilFunc;
				m_DepthStencilState.BackFace.StencilMask = ds.BackFace.StencilMask;
				m_DepthStencilState.BackFace.StencilRef = ds.BackFace.StencilRef;
			}

			if (m_DepthStencilState.FrontFace.StencilFailOp != ds.FrontFace.StencilFailOp ||
				m_DepthStencilState.FrontFace.StencilDepthFailOp != ds.FrontFace.StencilDepthFailOp ||
				m_DepthStencilState.FrontFace.StencilPassOp != ds.FrontFace.StencilPassOp || 

				m_DepthStencilState.BackFace.StencilFailOp != ds.BackFace.StencilFailOp ||
				m_DepthStencilState.BackFace.StencilDepthFailOp != ds.BackFace.StencilDepthFailOp ||
				m_DepthStencilState.BackFace.StencilPassOp != ds.BackFace.StencilPassOp)
			{

				if (ds.FrontFace.StencilFailOp != ds.BackFace.StencilFailOp ||
					ds.FrontFace.StencilDepthFailOp != ds.BackFace.StencilDepthFailOp ||
					ds.FrontFace.StencilPassOp != ds.BackFace.StencilPassOp)
				{
					glStencilOpSeparate(GL_FRONT, ConvertStencilOpToGLenum(ds.FrontFace.StencilFailOp), ConvertStencilOpToGLenum(ds.FrontFace.StencilDepthFailOp), ConvertStencilOpToGLenum(ds.FrontFace.StencilPassOp));
					glStencilOpSeparate(GL_BACK,  ConvertStencilOpToGLenum(ds.BackFace.StencilFailOp), ConvertStencilOpToGLenum(ds.BackFace.StencilDepthFailOp), ConvertStencilOpToGLenum(ds.BackFace.StencilPassOp));
				}
				else
				{
					glStencilOpSeparate(GL_FRONT, ConvertStencilOpToGLenum(ds.FrontFace.StencilFailOp), ConvertStencilOpToGLenum(ds.FrontFace.StencilDepthFailOp), ConvertStencilOpToGLenum(ds.FrontFace.StencilPassOp));
				}

				m_DepthStencilState.FrontFace.StencilFailOp = ds.FrontFace.StencilFailOp;
				m_DepthStencilState.FrontFace.StencilDepthFailOp = ds.FrontFace.StencilDepthFailOp;
				m_DepthStencilState.FrontFace.StencilPassOp = ds.FrontFace.StencilPassOp;
				m_DepthStencilState.BackFace.StencilFailOp = ds.BackFace.StencilFailOp;
				m_DepthStencilState.BackFace.StencilDepthFailOp = ds.BackFace.StencilDepthFailOp;
				m_DepthStencilState.BackFace.StencilPassOp = ds.BackFace.StencilPassOp;
			}
		}
		void GLStates::setRSState(const RasterizerDesc& raster)
		{
#define GEN_CAP_CODE(varname, cap) if (m_RasterizerState.varname != raster.varname)\
						{\
			m_RasterizerState.varname = raster.varname;\
			if (raster.varname)\
										{\
				glEnable(cap);\
										}\
														else\
							{\
				glDisable(cap);\
							}\
					}
			
			if (m_RasterizerState.FillMode != raster.FillMode)
			{
				glPolygonMode(GL_FRONT_AND_BACK, ConvertPolygonModeToGLenum(raster.FillMode));
				m_RasterizerState.FillMode = raster.FillMode;
			}

			if (m_RasterizerState.CullFaceEnable != raster.CullFaceEnable)
			{
				m_RasterizerState.CullFaceEnable = raster.CullFaceEnable;
				if (raster.CullFaceEnable)
				{
					glEnable(GL_CULL_FACE);
					if (m_RasterizerState.CullMode != raster.CullMode)
					{
						glCullFace(ConvertFaceModeToGLenum(raster.CullMode));
						m_RasterizerState.CullMode = raster.CullMode;
					}
				}
				else
				{
					glDisable(GL_CULL_FACE);
				}
			}

			if (m_RasterizerState.DepthClampEnable != raster.DepthClampEnable)
			{
				m_RasterizerState.DepthClampEnable = raster.DepthClampEnable;
				if (raster.DepthClampEnable)
				{
					glEnable(GL_DEPTH_CLAMP);

					if (m_RasterizerState.DepthRange.FarVal != raster.DepthRange.FarVal || m_RasterizerState.DepthRange.NearVal != raster.DepthRange.NearVal)
					{
						glDepthRange(raster.DepthRange.NearVal, raster.DepthRange.FarVal);
						m_RasterizerState.DepthRange = raster.DepthRange;
					}
				}
				else
				{
					glDisable(GL_DEPTH_CLAMP);
				}
			}

			if (m_RasterizerState.ScissorEnable != raster.ScissorEnable)
			{
				m_RasterizerState.ScissorEnable = raster.ScissorEnable;
				if (raster.ScissorEnable)
				{
					glEnable(GL_SCISSOR_TEST);
					if (m_RasterizerState.ScissorBox != raster.ScissorBox)
					{
						glScissor(raster.ScissorBox.X, raster.ScissorBox.Y, raster.ScissorBox.Width, raster.ScissorBox.Height);
						m_RasterizerState.ScissorBox = raster.ScissorBox;
					}
				}
				else
				{
					glDisable(GL_SCISSOR_TEST);
				}
			}

			GEN_CAP_CODE(MultisampleEanble, GL_MULTISAMPLE);
			GEN_CAP_CODE(AntialiasedLineEnable, GL_LINE_SMOOTH);
			GEN_CAP_CODE(AntialiasedPolygonEnable, GL_POLYGON_SMOOTH);
			GEN_CAP_CODE(Dither, GL_DITHER);
			GEN_CAP_CODE(SRGB, GL_FRAMEBUFFER_SRGB);
			GEN_CAP_CODE(RasterizedDiscardEnable, GL_RASTERIZER_DISCARD);
			GEN_CAP_CODE(ProgramPointSizeEnable, GL_PROGRAM_POINT_SIZE);
			GEN_CAP_CODE(PrimitiveRestartEnable, GL_PRIMITIVE_RESTART);
#undef GEN_CAP_CODE

			if (m_RasterizerState.PolygonOffsetEnable != raster.PolygonOffsetEnable)
			{
				m_RasterizerState.PolygonOffsetEnable = raster.PolygonOffsetEnable;
				if (raster.PolygonOffsetEnable)
				{
					GLenum cap;
					switch (raster.FillMode)
					{
					case PolygonMode::FILL:
						cap = GL_POLYGON_OFFSET_FILL;
						break;
					case PolygonMode::LINE:
						cap = GL_POLYGON_OFFSET_LINE;
						break;
					case PolygonMode::POINT:
						cap = GL_POLYGON_OFFSET_POINT;
						break;
					default:
						assert(false);
						break;
					}

					glEnable(cap);

					if (m_RasterizerState.PolygonOffset.factor != raster.PolygonOffset.factor || m_RasterizerState.PolygonOffset.units != raster.PolygonOffset.units)
					{
						glPolygonOffset(raster.PolygonOffset.factor, raster.PolygonOffset.units);
						m_RasterizerState.PolygonOffset = raster.PolygonOffset;
					}
				}
				else
				{
					glDisable(GL_SCISSOR_TEST);
				}
			}

			if (m_RasterizerState.LogicMode.LogicEnable != raster.LogicMode.LogicEnable)
			{
				m_RasterizerState.LogicMode.LogicEnable = raster.LogicMode.LogicEnable;
				if (raster.LogicMode.LogicEnable)
				{
					glEnable(GL_COLOR_LOGIC_OP);
					if (m_RasterizerState.LogicMode.LogicOp != raster.LogicMode.LogicOp)
					{
						glLogicOp(ConvertLogicFuncToGLenum(raster.LogicMode.LogicOp));
						m_RasterizerState.LogicMode.LogicOp = raster.LogicMode.LogicOp;
					}
				}
				else
				{
					glDisable(GL_COLOR_LOGIC_OP);
				}
			}
			
		}

		GLStates::GLStates(): 
			m_BlendState(),
			m_DepthStencilState(),
			m_RasterizerState(),

			m_ProgramState(0),
			m_FramebufferState(0),
			m_VertexArrayState(0),

			m_TextureCount(0),
			m_ViewportCount(0)
		{}


		GLStates::~GLStates()
		{
		}
	}
}
