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

//		static const GLCapabilities* g_Caps = GLCapabilities::getGLCapabilities();

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

				GLboolean colorWriteMask[4];
				glGetBooleanv(GL_COLOR_WRITEMASK, colorWriteMask);
				m_RasterizerState.ColorWriteMask.RedWriteMask = colorWriteMask[0] != 0;
				m_RasterizerState.ColorWriteMask.GreenWriteMask = colorWriteMask[1] != 0;
				m_RasterizerState.ColorWriteMask.BlueWriteMask = colorWriteMask[2] != 0;
				m_RasterizerState.ColorWriteMask.AlphaWriteMask = colorWriteMask[3] != 0;
			}

			{
				m_RasterizerState.PointSprite.PointSpriteEnabled = glIsEnabled(GL_POINT_SPRITE);
				m_RasterizerState.PointSprite.ProgramPointSizeEnabled = glIsEnabled(GL_PROGRAM_POINT_SIZE);
				glGetFloatv(GL_POINT_FADE_THRESHOLD_SIZE, &m_RasterizerState.PointSprite.PointFadeThresholdSize);

				GLint out;
				glGetIntegerv(GL_POINT_SPRITE_COORD_ORIGIN, &out);
				m_RasterizerState.PointSprite.PointSpriteCoordOrigin = ConvertGLenumToSpriteCoordOrigin(out);
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
				glColorMask(true, true, true, true);
				glDisable(GL_POINT_SPRITE);
				glDisable(GL_PROGRAM_POINT_SIZE);

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

		void GLStates::bindFramebuffer(GLuint framebuffer, FramebufferTarget target)
		{
			const int idx = static_cast<int>(target);
			if (m_FramebufferState[idx] != framebuffer)
			{
				glBindFramebuffer(ConvertFramebufferTargetToGLenum(target), framebuffer);
				m_FramebufferState[idx] = framebuffer;
			}
		}

		void GLStates::restoreFramebuffer(FramebufferTarget target)
		{
			const int idx = static_cast<int>(target);
			GLenum mode;
			switch (target)
			{
			case FramebufferTarget::FRAMEBUFFER:
			case FramebufferTarget::DRAW: 
				mode = GL_DRAW_FRAMEBUFFER_BINDING;
				break;
			case FramebufferTarget::READ:
				mode = GL_READ_FRAMEBUFFER_BINDING;
				break;
			default:
				assert(false);
				break;
			}

			glGetIntegerv(mode, (GLint*)&m_FramebufferState[idx]);
		}

		void GLStates::resetFramebuffer(FramebufferTarget target, bool force)
		{
			const int idx = static_cast<int>(target);
			if (force || m_FramebufferState != 0)
			{
				glBindFramebuffer(ConvertFramebufferTargetToGLenum(target), 0);
			}

			m_FramebufferState[idx] = 0;
		}

		void GLStates::bindTextures(unsigned int count, const TextureGL** pTextures, const unsigned int* units)
		{
			static GLuint textureNames[80];
			assert(count <= 80);
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

			if (m_RasterizerState.ColorWriteMask.RedWriteMask != raster.ColorWriteMask.RedWriteMask ||
				m_RasterizerState.ColorWriteMask.GreenWriteMask != raster.ColorWriteMask.GreenWriteMask ||
				m_RasterizerState.ColorWriteMask.BlueWriteMask != raster.ColorWriteMask.BlueWriteMask ||
				m_RasterizerState.ColorWriteMask.AlphaWriteMask != raster.ColorWriteMask.AlphaWriteMask)
			{
				m_RasterizerState.ColorWriteMask = raster.ColorWriteMask;
				glColorMask(raster.ColorWriteMask.RedWriteMask, raster.ColorWriteMask.GreenWriteMask, raster.ColorWriteMask.BlueWriteMask, raster.ColorWriteMask.AlphaWriteMask);
			}
			
			if (m_RasterizerState.PointSprite.PointSpriteEnabled != raster.PointSprite.PointSpriteEnabled)
			{
				if (raster.PointSprite.PointSpriteEnabled)
				{
					glEnable(GL_POINT_SPRITE);

					if (raster.PointSprite.PointFadeThresholdSize != m_RasterizerState.PointSprite.PointFadeThresholdSize)
					{
						assert(raster.PointSprite.PointFadeThresholdSize >= 0.0);
						m_RasterizerState.PointSprite.PointFadeThresholdSize = raster.PointSprite.PointFadeThresholdSize;
						
						glPointParameterf(GL_POINT_FADE_THRESHOLD_SIZE, raster.PointSprite.PointFadeThresholdSize);
					}

					if (raster.PointSprite.PointSpriteCoordOrigin != m_RasterizerState.PointSprite.PointSpriteCoordOrigin)
					{
						m_RasterizerState.PointSprite.PointSpriteCoordOrigin = raster.PointSprite.PointSpriteCoordOrigin;

						glPointParameteri(GL_POINT_SPRITE_COORD_ORIGIN, ConvertSpriteCoordOriginToGLenum(raster.PointSprite.PointSpriteCoordOrigin));
					}
					
				}
				else
				{
					glDisable(GL_POINT_SPRITE);
				}
			}

			if (raster.PointSprite.ProgramPointSizeEnabled != m_RasterizerState.PointSprite.ProgramPointSizeEnabled)
			{
				m_RasterizerState.PointSprite.ProgramPointSizeEnabled = raster.PointSprite.ProgramPointSizeEnabled;
				if (raster.PointSprite.ProgramPointSizeEnabled)
				{
					glEnable(GL_PROGRAM_POINT_SIZE);
				}
				else
				{
					glDisable(GL_PROGRAM_POINT_SIZE);
				}
			}
		}

		void GLStates::setActiveTexture(unsigned int unit)
		{
			if (unit != m_ActiveTextureUnit)
			{
				m_ActiveTextureUnit = unit;
				glActiveTexture(GL_TEXTURE0 + unit);
			}
		}
		
		void GLStates::resetActiveTexture(bool force)
		{
			if (force || m_ActiveTextureUnit)
			{
				glActiveTexture(GL_TEXTURE0);
				m_ActiveTextureUnit = 0;
			}
		}

		void GLStates::restoreActiveTexture()
		{
			glGetIntegerv(GL_ACTIVE_TEXTURE, (GLint*)&m_ActiveTextureUnit);
			m_ActiveTextureUnit -= GL_TEXTURE0;
		}

		// returns a pair of values indicating the range of widths supported for aliased lines. See glLineWidth.
		Rangef GLStates::getAliasedLineWidthRange()
		{
			static Rangef out = {-1, -1};
			if (out.Max == -1)
			{
				glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, (GLfloat*)&out);
			}
			
			return out;
		}
		// returns two values: the smallest and largest supported sizes for points. The smallest size must be at most 1, and the largest size must be at least 1.
		Rangef GLStates::getAliasedPointSizeRange()
		{
			static Rangef out = { -1, -1 };
			if (out.Max == -1)
			{
				glGetFloatv(GL_ALIASED_POINT_SIZE_RANGE, (GLfloat*)&out);
			}

			return out;
		}

		// returns a list of symbolic constants of length GL_NUM_COMPRESSED_TEXTURE_FORMATS indicating which compressed texture formats are available. See glCompressedTexImage2D.
		const GLenum* GLStates::getCompressedTextureFormats(GLuint& length)
		{
			static GLint number = -1;
			static GLenum* formats = nullptr;

			if (!formats)
			{
				glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &number);
				if (!number)
				{
					length = 0;
					return nullptr;
				}

				formats = new GLenum[number];
				glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, (GLint*)formats);
			}

			length = number;
			return formats;
		}
		// return an array of GL_NUM_PROGRAM_BINARY_FORMATS values, indicating the proram binary formats supported by the implementation.
		const GLenum* GLStates::getProgramBinaryFormats(GLuint& length)
		{
			static GLint number = -1;
			static GLenum* formats = nullptr;

			if (!formats)
			{
				glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &number);
				if (!number)
				{
					length = 0;
					return nullptr;
				}
					
				formats = new GLenum[number];
				glGetIntegerv(GL_PROGRAM_BINARY_FORMATS, (GLint*)formats);
			}

			length = number;
			return formats;
		}
		// returns two values, the minimum and maximum viewport bounds range. The minimum range should be at least [-32768, 32767].
		Rangei GLStates::getViewportBoundsRange()
		{
			Rangei out = {-1,-1};
			if (out.Max == -1)
			{
				glGetIntegerv(GL_VIEWPORT_BOUNDS_RANGE, (GLint*)&out);
			}

			return out;
		}

		void GLStates::bindProgramPipeline(GLuint program)
		{
			if (m_ProgramPipelineState != program)
			{
				m_ProgramPipelineState = program;
				glBindProgramPipeline(program);
			}
		}
		void GLStates::restoreProgramPipeline()
		{
			glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING, (GLint*)&m_ProgramPipelineState);
		}

		void GLStates::resetProgramPipeline(bool force)
		{
			if (force || m_ProgramPipelineState)
			{
				m_ProgramPipelineState = 0;
				glBindProgramPipeline(0);
			}
		}

		void GLStates::setHint(HintTarget target, HintMode mode)
		{
			const int idx = static_cast<int>(target);
			if (m_HintStates[idx] != mode)
			{
				glHint(ConvertHintTargetToGLenum(target), ConvertHintModeToGLenum(mode));
				m_HintStates[idx] = mode;
			}
		}
		void GLStates::restoreHint(HintTarget target)
		{
			const int idx = static_cast<int>(target);
			GLint mode;
			GLenum pname = ConvertHintTargetToGLenum(target);
			glGetIntegerv(pname, &mode);
			m_HintStates[idx] = ConvertGLenumToHintMode(mode);
		}
		void GLStates::resetHint(HintTarget target, bool force)
		{
			const int idx = static_cast<int>(target);
			GLenum pname = ConvertHintTargetToGLenum(target);
			if (m_HintStates[idx] != HintMode::DONT_CARE)
			{
				glHint(pname, GL_DONT_CARE);
				m_HintStates[idx] = HintMode::DONT_CARE;
			}
		}

		void GLStates::bindBuffer(GLuint buffer, BufferTarget target)
		{
			const int idx = static_cast<int>(target);
			GLenum gl_target = ConvertBufferTargetToGLenum(target);
			if (m_BufferStates[idx] != buffer)
			{
				m_BufferStates[idx] = buffer;
				glBindBuffer(gl_target, buffer);
			}
		}
		
		void GLStates::resetBuffer(BufferTarget target, bool force)
		{
			const int idx = static_cast<int>(target);
			GLenum gl_target = ConvertBufferTargetToGLenum(target);
			if (force || m_BufferStates[idx])
			{
				m_BufferStates[idx] = 0;
				glBindBuffer(gl_target, 0);
			}
		}

		void GLStates::restoreBuffer(BufferTarget target)
		{
			static const GLenum BINDINGS[] =
			{
				GL_ARRAY_BUFFER_BINDING,
				GL_COPY_READ_BUFFER_BINDING,
				GL_COPY_WRITE_BUFFER_BINDING,
				GL_DISPATCH_INDIRECT_BUFFER_BINDING,
				GL_DRAW_INDIRECT_BUFFER_BINDING,
				GL_ELEMENT_ARRAY_BUFFER_BINDING,
				GL_PIXEL_PACK_BUFFER_BINDING,
				GL_PIXEL_UNPACK_BUFFER_BINDING,
				GL_TEXTURE_BUFFER_BINDING,
				GL_QUERY_BUFFER_BINDING,
				GL_ATOMIC_COUNTER_BUFFER_BINDING,
				GL_SHADER_STORAGE_BUFFER_BINDING,
				GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,
				GL_UNIFORM_BUFFER_BINDING,
			};

			const int idx = static_cast<int>(target);
			glGetIntegerv(BINDINGS[idx], (GLint*)&m_BufferStates[idx]);
		}

		void GLStates::setClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a)
		{
			GLfloat color[4] = { r, g, b, a };
			setClearColor(color);
		}

		void GLStates::setClearColor(GLfloat color[4])
		{
			if (!Numeric::isEqual<GLfloat, 4>(color, m_ClearColor))
			{
				glClearColor(color[0], color[1], color[2], color[3]);
				for (int i = 0; i < 4; i++)
				{
					m_ClearColor[i] = color[i];
				}
			}
		}

		void GLStates::restoreClearColor()
		{
			glGetFloatv(GL_COLOR_CLEAR_VALUE, m_ClearColor);
		}

		void GLStates::setClearDepth(GLfloat depth)
		{
			if (m_ClearDepth != depth)
			{
				glClearDepth(depth);
				m_ClearDepth = depth;
			}
		}

		void GLStates::restoreClearDepth()
		{
			glGetFloatv(GL_DEPTH_CLEAR_VALUE, &m_ClearDepth);
		}

		void GLStates::setClearStencil(GLint stencil)
		{
			if (m_ClearStencil != stencil)
			{
				m_ClearStencil = stencil;
				glClearStencil(stencil);
			}
		}

		void GLStates::restoreClearStencil()
		{
			glGetIntegerv(GL_STENCIL_CLEAR_VALUE, (GLint*)m_ClearStencil);
		}

		void GLStates::setLineWidth(GLfloat width)
		{
			if (m_LineWidth != width)
			{
				glLineWidth(width);
				m_LineWidth = width;
			}
		}

		void GLStates::restoreLineWidth()
		{
			glGetFloatv(GL_LINE_WIDTH, &m_LineWidth);
		}

		void GLStates::resetLineWidth(bool force)
		{
			if (force || m_LineWidth != 1.0f)
			{
				glLineWidth(1.0f);
				m_LineWidth = 1.0f;
			}
		}

		void GLStates::setPointSize(GLfloat size)
		{
			if (m_PointSize != size)
			{
				glPointSize(size);
				m_PointSize = size;
			}
		}
		void GLStates::restorePointSize()
		{
			glGetFloatv(GL_POINT_SIZE, &m_LineWidth);
		}

		void GLStates::resetPointSize(bool force)
		{
			if (force || m_PointSize != 1.0f)
			{
				glPointSize(1.0f);
				m_PointSize = 1.0f;
			}
		}

		void GLStates::setPrimitiveRestartIndex(GLint index)
		{
			if (m_PrimitiveRestartIndex != index)
			{
				m_PrimitiveRestartIndex = index;
				glPrimitiveRestartIndex(index);
			}
		}
		void GLStates::restorePrimitiveRestartIndex()
		{
			glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX, (GLint*)&m_PrimitiveRestartIndex);
		}
		void GLStates::resetPrimitiveRestartIndex(bool force)
		{
			if (force || m_PrimitiveRestartIndex)
			{
				m_PrimitiveRestartIndex = 0;
				glPrimitiveRestartIndex(0);
			}
		}

		void GLStates::setRenderbuffer(GLuint renderbuffer)
		{
			if (m_RenderbufferState != renderbuffer)
			{
				glBindRenderbuffer(GL_RENDERBUFFER, renderbuffer);
				m_RenderbufferState = renderbuffer;
			}
		}
		void GLStates::restoreRenderbuffer()
		{
			glGetIntegerv(GL_RENDERBUFFER_BINDING, (GLint*)&m_RenderbufferState);
		}

		void GLStates::resetRenderbuffer(bool force)
		{
			if (force || m_RenderbufferState)
			{
				glBindRenderbuffer(GL_RENDERBUFFER, 0);
				m_RenderbufferState = 0;
			}
		}

		void GLStates::setSampler(GLuint sampler, GLuint unit)
		{
			if (m_SamplerStates[unit] != sampler)
			{
				glBindSampler(unit, sampler);
				m_SamplerStates[unit] = sampler;
			}
		}
		
		void GLStates::restoreSampler(GLuint unit)
		{
			glGetIntegeri_v(GL_SAMPLER_BINDING, unit, (GLint*)m_SamplerStates[unit]);
		}

		void GLStates::resetSampler(GLuint unit, bool force)
		{
			if (force || m_SamplerStates[unit])
			{
				glBindSampler(unit, 0);
				m_SamplerStates[unit] = 0;
			}
		}
#if 0
		void GLStates::setProvoke(ProvokeMode mode);
		ProvokeMode getProvoke() const { return m_ProvokeState; }
		void GLStates::restoreProvoke();
		void GLStates::resetProvoke(bool force = false);
#endif

		GLStates::GLStates() :
			m_BlendState(),
			m_DepthStencilState(),
			m_RasterizerState(),

			m_ProgramState(0),
			m_FramebufferState(),
			m_VertexArrayState(0),

			m_TextureCount(0),
			m_ViewportCount(0),

			m_ActiveTextureUnit(0),
			m_LineWidth(1.0f),
			m_PointSize(1.0f),
			m_PrimitiveRestartIndex(0),
			m_RenderbufferState(0),
			m_ProvokeState(ProvokeMode::FIRST)
		{}


		GLStates::~GLStates()
		{
		}
	}
}
