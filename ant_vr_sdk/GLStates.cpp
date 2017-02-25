#include "GLStates.h"
#include <GLCapabilities.h>
#include "Numeric.h"
#include <glm.hpp>

#pragma warning(disable:4800)
#pragma warning(disable:4018)
#pragma warning(disable:4244)

namespace jet
{
	namespace util
	{

		static const BlendDesc g_BlendDefault = BlendDesc();
		static const DepthStencilDesc g_DSStateDefault = DepthStencilDesc();
		static const RasterizerDesc g_RSStateDefault = RasterizerDesc();

//		static const GLCapabilities* g_Caps = GLCapabilities::getGLCapabilities();

		GLStates& GLStates::get()
		{
			static GLStates instance;
			return instance;
		}

		// Retrive all of the gl-states from the opengl context.
		void GLStates::restoreAll()
		{
			restoreActiveTexture();
			restoreBlendState();
//			restoreBuffer();
			restoreClearColor();
			restoreClearDepth();
			restoreClearStencil();
			restoreDSState();
			restoreFramebuffer();
//			restoreHint();
			restoreLineWidth();
			restorePointSize();
			restorePrimitiveRestartIndex();
			restoreProgram();
			restoreProgramPipeline();
			restoreRenderbuffer();
			restoreRSState();
			restoreSampler();
			restoreTextures();
			restoreVAO();
			restoreViewport();
		}
		// Reset all of the states to default setting.
		void GLStates::resetAll(bool force)
		{
			resetActiveTexture(force);
			resetBlendState(force);
//			resetBuffer(, false);
			resetClearColor(force);
			resetClearDepth(force);
			resetClearStencil(force);
			resetDSState(force);
			resetFramebuffer(FramebufferTarget::FRAMEBUFFER, force);
//			resetHint(, force);
			resetLineWidth(force);
			resetPointSize(force);
			resetPrimitiveRestartIndex(force);
			resetProgram(force);
			resetProgramPipeline(force);
			resetRenderbuffer(force);
			resetRSState(force);
			resetSampler(0, force); // TODO
			resetTextures(force);
			resetVAO(force);
//			resetViewport();
		}

		void GLStates::resetClearColor(bool force)
		{
//			const GLfloat defualt_colors[4] = { 0, 0, 0, 0 };
			if (force || m_ClearColor[0] != 0.0f || m_ClearColor[1] != 0.0f || m_ClearColor[2] != 0.0f || m_ClearColor[3] != 0.0f)
			{
				glClearColor(0, 0, 0, 0);
				for (int i = 0; i < 4; i++)
					m_ClearColor[i] = 0.0f;
			}
		}

		void GLStates::resetClearDepth(bool force)
		{
			if (force || m_ClearDepth != 1.0f)
			{
				glClearDepth(1.0f);
				m_ClearDepth = 1.0f;
			}
		}

		void GLStates::resetClearStencil(bool force)
		{
			if (force || m_ClearStencil != 0)
			{
				glClearStencil(0);
				m_ClearStencil = 0; 
			}
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
				m_BlendState.Multisample.AlphaToCoverageEnable = glIsEnabled(GL_SAMPLE_ALPHA_TO_COVERAGE);
				m_BlendState.Multisample.AlphaToOneEnable = glIsEnabled(GL_SAMPLE_ALPHA_TO_ONE);
				m_BlendState.Multisample.CoverageEnable = glIsEnabled(GL_SAMPLE_COVERAGE);
				m_BlendState.Multisample.MultisampleEanble = glIsEnabled(GL_MULTISAMPLE);
				GLboolean value;
				glGetBooleanv(GL_SAMPLE_COVERAGE_INVERT, &value);
				m_BlendState.Multisample.SampleConverageInvert = value != 0;

				glGetFloatv(GL_SAMPLE_COVERAGE_VALUE, &m_BlendState.Multisample.SampleCoverageValue);
			}

			{
				m_BlendState.AntialiasedLineEnable = glIsEnabled(GL_LINE_SMOOTH);
				m_BlendState.AntialiasedPointEnable = glIsEnabled(GL_POINT_SMOOTH);
				m_BlendState.AntialiasedPolygonEnable = glIsEnabled(GL_POLYGON_SMOOTH);
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
				glDisable(GL_SAMPLE_ALPHA_TO_ONE);
				glDisable(GL_SAMPLE_COVERAGE);
				glDisable(GL_MULTISAMPLE);
				glDisable(GL_LINE_SMOOTH);
				glDisable(GL_POINT_SMOOTH);
				glDisable(GL_POLYGON_SMOOTH);

				m_BlendState = g_BlendDefault;
			}
			else
			{
				if (m_BlendState.Multisample.AlphaToCoverageEnable)
				{
					glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
					m_BlendState.Multisample.AlphaToCoverageEnable = false;
				}

				if (m_BlendState.Multisample.AlphaToOneEnable)
				{
					glDisable(GL_SAMPLE_ALPHA_TO_ONE);
					m_BlendState.Multisample.AlphaToOneEnable = false;
				}

				if (m_BlendState.Multisample.CoverageEnable)
				{
					glDisable(GL_SAMPLE_COVERAGE);
					m_BlendState.Multisample.CoverageEnable = false;
				}

				if (m_BlendState.Multisample.MultisampleEanble)
				{
					glDisable(GL_MULTISAMPLE);
					m_BlendState.Multisample.MultisampleEanble = false;
				}

				if (m_BlendState.AntialiasedLineEnable)
				{
					glDisable(GL_LINE_SMOOTH);
					m_BlendState.AntialiasedLineEnable = false;
				}

				if (m_BlendState.AntialiasedPointEnable)
				{
					glDisable(GL_POINT_SMOOTH);
					m_BlendState.AntialiasedPointEnable = false;
				}

				if (m_BlendState.AntialiasedPolygonEnable)
				{
					glDisable(GL_POLYGON_SMOOTH);
					m_BlendState.AntialiasedPolygonEnable = false;
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
				m_RasterizerState.Dither = glIsEnabled(GL_DITHER);
				m_RasterizerState.SRGB = glIsEnabled(GL_FRAMEBUFFER_SRGB);
				m_bPolygonSmoothStates[static_cast<int>(PolygonMode::FILL)] = glIsEnabled(GL_POLYGON_OFFSET_FILL);
				m_bPolygonSmoothStates[static_cast<int>(PolygonMode::LINE)] = glIsEnabled(GL_POLYGON_OFFSET_LINE);
				m_bPolygonSmoothStates[static_cast<int>(PolygonMode::POINT)] = glIsEnabled(GL_POLYGON_OFFSET_POINT);
				m_RasterizerState.PolygonOffsetEnable = m_bPolygonSmoothStates[static_cast<int>(m_RasterizerState.FillMode)];
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
			GLint const maxTextureUnits = getMaxCombinedTextureImageUnits();
#if defined(_DEBUG)
			if (count > maxTextureUnits)
			{
				printf("Binding texture units beyong the limits: count = %d, limits = %d.\n", count, maxTextureUnits);
			}
#endif
			count = Numeric::min(count, static_cast<unsigned>(maxTextureUnits));
			m_TextureCount = 0;
			bool ordered = true;
			bool tag = (units == nullptr) && glBindTextures;

			for (unsigned int i = 0; i < count; i++)
			{
				GLenum target = pTextures && pTextures[i] ? pTextures[i]->getTarget() : 0;
				GLuint texture = pTextures && pTextures[i] ? pTextures[i]->getTexture() : 0;
				int unit = units ? units[i] : i;

				if (tag)
				{
					if (m_TextureStates[unit].textureID != texture)
					{
						m_TextureCount++;
					}

					m_TextureNames[m_TextureCount] = texture;
					m_TextureStates[unit].target = target;
					m_TextureStates[unit].textureID = texture;
					m_TextureUnits[m_TextureCount] = unit;
				}
				else if (m_TextureStates[unit].textureID != texture)
				{
					m_TextureNames[m_TextureCount] = texture;
					m_TextureStates[unit].target = target;
					m_TextureStates[unit].textureID = texture;
					m_TextureUnits[m_TextureCount] = unit;

					if (m_TextureCount == 0 && unit != 0)
					{
						ordered = false;
					}

					if (m_TextureCount && unit - m_TextureUnits[m_TextureCount - 1] != 1)
					{
						ordered = false;
					}

					m_TextureCount++;
				}
			}

			if (!m_TextureCount)
			{
				return;
			}

			if (ordered && glBindTextures)
			{
				glBindTextures(0, m_TextureCount, m_TextureNames);
			}
			else
			{
				if (glBindTextureUnit)
				{
					for (unsigned int i = 0; i < m_TextureCount; i++)
					{
						glBindTextureUnit(m_TextureUnits[i], m_TextureStates[m_TextureUnits[i]].textureID);
					}
				}
				else
				{
					for (unsigned int i = 0; i < m_TextureCount; i++)
					{
						setActiveTexture(m_TextureUnits[i]);
						glBindTexture(m_TextureStates[m_TextureUnits[i]].target, m_TextureStates[m_TextureUnits[i]].textureID);
					}
				}
			}
		}

		void GLStates::restoreTextures()
		{
			GLint count = getMaxCombinedTextureImageUnits();
			const GLenum TEXTURE_TARGETS_BINDING[] =
			{
				GL_TEXTURE_BINDING_1D,
				GL_TEXTURE_BINDING_2D,
				GL_TEXTURE_BINDING_3D,
				GL_TEXTURE_BINDING_1D_ARRAY,
				GL_TEXTURE_BINDING_2D_ARRAY,
				GL_TEXTURE_BINDING_RECTANGLE,
				GL_TEXTURE_BINDING_CUBE_MAP,
				GL_TEXTURE_BINDING_CUBE_MAP_ARRAY,
				GL_TEXTURE_BINDING_2D_MULTISAMPLE,
				GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY
			};

			m_TextureCount = 0;
			for (int i = 0; i < count; i++)
			{
				bool founded = false;
				setActiveTexture(i);
				for (int j = 0; j < _countof(TEXTURE_TARGETS_BINDING) && !founded; j++)
				{
					GLuint textureID;
					glGetIntegerv(TEXTURE_TARGETS_BINDING[j], (GLint*)&textureID);

					if (textureID && glIsTexture(textureID))
					{
						m_TextureUnits[m_TextureCount++] = i;
						m_TextureStates[i].target = TEXTURE_TARGETS[j];
						m_TextureStates[i].textureID = textureID;
						founded = true;
					}
				}
			}
		}

		void GLStates::resetTextures(bool force)
		{
			if (force)
			{
				GLint count = getMaxCombinedTextureImageUnits();
				if (glBindTextures)
				{
					glBindTextures(0, count, nullptr);
				}
				else
				{
					for (int i = 0; i < count; i++)
					{
						if (glBindTextureUnit)
						{
							glBindTextureUnit(i, 0);
						}
						else
						{
							setActiveTexture(count - i - 1);
							for (int i = 0; i < _countof(TEXTURE_TARGETS); i++)
							{
								glBindTexture(TEXTURE_TARGETS[i], 0);
							}
						}
					}
				}

				m_TextureCount = 0;
				memset(m_TextureStates, 0, sizeof(BindTexture) * count);
			}
			else /*if (m_TextureCount)*/
			{
				if (m_TextureCount == 0)
				{
					return;
				}

				GLint count = getMaxCombinedTextureImageUnits();
				if (glBindTextures)
				{
					glBindTextures(0, count, nullptr);
					for (unsigned i = 0; i < m_TextureCount; i++)
					{
						m_TextureStates[m_TextureUnits[i]].textureID = 0;
					}
				}
				else
				{
					for (int i = 0; i < m_TextureCount; i++)
					{
						
						if (glBindTextureUnit)
						{
							glBindTextureUnit(m_TextureUnits[i], 0);
						}
						else
						{
							setActiveTexture(i);
							glBindTexture(m_TextureStates[m_TextureUnits[i]].target, 0);
						}
						m_TextureStates[m_TextureUnits[i]].textureID = 0;
					}
				}

				m_TextureCount = 0;
			}
		}

		void GLStates::setViewports(unsigned int count, const ViewportDesc* pViewports)
		{
			if (count && !glViewportIndexedf)  // TODO
			{
				if (m_ViewportStates[0].Viewport != pViewports[0].Viewport)
				{
					glViewport(pViewports[0].Viewport.X, pViewports[0].Viewport.Y, pViewports[0].Viewport.Width, pViewports[0].Viewport.Height);
					m_ViewportStates[0].Viewport = pViewports[0].Viewport;
				}

				if (m_ViewportStates[0].ScissorEnable != pViewports[0].ScissorEnable)
				{
					
					if (pViewports[0].ScissorEnable)
					{
						m_ViewportStates[0].ScissorEnable = pViewports[0].ScissorEnable;
						glEnable(GL_SCISSOR_TEST);

						if (m_ViewportStates[0].ScissorBox != pViewports[0].ScissorBox)
						{
							glScissor(pViewports[0].ScissorBox.X, pViewports[0].ScissorBox.Y, pViewports[0].ScissorBox.Width, pViewports[0].ScissorBox.Height);
							m_ViewportStates[0].ScissorBox = pViewports[0].ScissorBox;
						}
					}
					else
					{
						glDisable(GL_SCISSOR_TEST);
					}
				}
				else if (pViewports[0].ScissorEnable)
				{
					if (m_ViewportStates[0].ScissorBox != pViewports[0].ScissorBox)
					{
						glScissor(pViewports[0].ScissorBox.X, pViewports[0].ScissorBox.Y, pViewports[0].ScissorBox.Width, pViewports[0].ScissorBox.Height);
						m_ViewportStates[0].ScissorBox = pViewports[0].ScissorBox;
					}
				}
			}
			else if (count)
			{
				count = Numeric::min(count, static_cast<unsigned>(getMaxViewports()));

				for (unsigned i = 0; i < count; i++)
				{
					if (m_ViewportStates[i].Viewport != pViewports[i].Viewport)
					{
						glViewportIndexedf(i, pViewports[i].Viewport.X, pViewports[i].Viewport.Y, pViewports[i].Viewport.Width, pViewports[i].Viewport.Height);
						m_ViewportStates[i].Viewport = pViewports[i].Viewport;
					}

					if (m_ViewportStates[i].ScissorEnable != pViewports[i].ScissorEnable)
					{

						if (pViewports[i].ScissorEnable)
						{
							m_ViewportStates[i].ScissorEnable = pViewports[i].ScissorEnable;
							glEnablei(GL_SCISSOR_TEST, i);

							if (m_ViewportStates[i].ScissorBox != pViewports[i].ScissorBox)
							{
								glScissorIndexed(i, pViewports[i].ScissorBox.X, pViewports[i].ScissorBox.Y, pViewports[i].ScissorBox.Width, pViewports[i].ScissorBox.Height);
								m_ViewportStates[i].ScissorBox = pViewports[i].ScissorBox;
							}
						}
						else
						{
							glDisablei(GL_SCISSOR_TEST, i);
						}
					}
					else if (pViewports[i].ScissorEnable)
					{
						if (m_ViewportStates[i].ScissorBox != pViewports[i].ScissorBox)
						{
							glScissorIndexed(i, pViewports[i].ScissorBox.X, pViewports[i].ScissorBox.Y, pViewports[i].ScissorBox.Width, pViewports[i].ScissorBox.Height);
							m_ViewportStates[i].ScissorBox = pViewports[i].ScissorBox;
						}
					}
				}
			}

			m_ViewportCount = count;
		}
		void GLStates::restoreViewport(unsigned int index)
		{
			if (GLCapabilities::get()->OpenGL40)
			{
				assert(index < getMaxViewports());
				glGetIntegeri_v(GL_VIEWPORT, index, (GLint *)&m_ViewportStates[index].Viewport);
				m_ViewportStates[index].ScissorEnable = glIsEnabledi(GL_SCISSOR_TEST, index);
				glGetIntegeri_v(GL_SCISSOR_BOX, index, (GLint *)&m_ViewportStates[index].ScissorBox);
			}
			else
			{
				assert(index == 0);
				glGetIntegerv(GL_VIEWPORT, (GLint *)&m_ViewportStates[index].Viewport);
				m_ViewportStates[index].ScissorEnable = glIsEnabled(GL_SCISSOR_TEST);
				glGetIntegerv(GL_SCISSOR_BOX, (GLint *)&m_ViewportStates[index].ScissorBox);
			}
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
			if (m_BlendState.Multisample.AlphaToCoverageEnable != blend.Multisample.AlphaToCoverageEnable)
			{
				if (blend.Multisample.AlphaToCoverageEnable)
				{
					glEnable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				}
				else
				{
					glDisable(GL_SAMPLE_ALPHA_TO_COVERAGE);
				}

				m_BlendState.Multisample.AlphaToCoverageEnable = blend.Multisample.AlphaToCoverageEnable;
			}

			if (m_BlendState.Multisample.AlphaToOneEnable != blend.Multisample.AlphaToOneEnable)
			{
				if (blend.Multisample.AlphaToOneEnable)
				{
					glEnable(GL_SAMPLE_ALPHA_TO_ONE);
				}
				else
				{
					glDisable(GL_SAMPLE_ALPHA_TO_ONE);
				}

				m_BlendState.Multisample.AlphaToOneEnable = blend.Multisample.AlphaToOneEnable;
			}

			if (m_BlendState.Multisample.CoverageEnable != blend.Multisample.CoverageEnable)
			{
				if (blend.Multisample.CoverageEnable)
				{
					glEnable(GL_SAMPLE_COVERAGE);
				}
				else
				{
					glDisable(GL_SAMPLE_COVERAGE);
				}

				m_BlendState.Multisample.CoverageEnable = blend.Multisample.CoverageEnable;
			}

			if (m_BlendState.Multisample.MultisampleEanble != blend.Multisample.MultisampleEanble)
			{
				if (blend.Multisample.MultisampleEanble)
				{
					glEnable(GL_MULTISAMPLE);
				}
				else
				{
					glDisable(GL_MULTISAMPLE);
				}

				m_BlendState.Multisample.MultisampleEanble = blend.Multisample.MultisampleEanble;
			}

			if (m_BlendState.AntialiasedLineEnable != blend.AntialiasedLineEnable)
			{
				if (blend.AntialiasedLineEnable)
				{
					glEnable(GL_LINE_SMOOTH);
				}
				else
				{
					glDisable(GL_LINE_SMOOTH);
				}

				m_BlendState.AntialiasedLineEnable = blend.AntialiasedLineEnable;
			}

			if (m_BlendState.AntialiasedPointEnable != blend.AntialiasedPointEnable)
			{
				if (blend.AntialiasedPointEnable)
				{
					glEnable(GL_POINT_SMOOTH);
				}
				else
				{
					glDisable(GL_POINT_SMOOTH);
				}

				m_BlendState.AntialiasedPointEnable = blend.AntialiasedPointEnable;
			}

			if (m_BlendState.AntialiasedPolygonEnable != blend.AntialiasedPolygonEnable)
			{
				if (blend.AntialiasedPolygonEnable)
				{
					glEnable(GL_POLYGON_SMOOTH);
				}
				else
				{
					glDisable(GL_POLYGON_SMOOTH);
				}

				m_BlendState.AntialiasedPolygonEnable = blend.AntialiasedPolygonEnable;
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
			else if (ds.DepthEnable)
			{
				if (m_DepthStencilState.DepthFunc != ds.DepthFunc)
				{
					glDepthFunc(ConvertCompareFuncToGLenum(ds.DepthFunc));
					m_DepthStencilState.DepthFunc = ds.DepthFunc;
				}
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
			else if (raster.CullFaceEnable)
			{
				if (m_RasterizerState.CullMode != raster.CullMode)
				{
					glCullFace(ConvertFaceModeToGLenum(raster.CullMode));
					m_RasterizerState.CullMode = raster.CullMode;
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
			else if (raster.DepthClampEnable)
			{
				if (m_RasterizerState.DepthRange.FarVal != raster.DepthRange.FarVal || m_RasterizerState.DepthRange.NearVal != raster.DepthRange.NearVal)
				{
					glDepthRange(raster.DepthRange.NearVal, raster.DepthRange.FarVal);
					m_RasterizerState.DepthRange = raster.DepthRange;
				}
			}

			GEN_CAP_CODE(Dither, GL_DITHER);
			GEN_CAP_CODE(SRGB, GL_FRAMEBUFFER_SRGB);
			GEN_CAP_CODE(RasterizedDiscardEnable, GL_RASTERIZER_DISCARD);
			GEN_CAP_CODE(ProgramPointSizeEnable, GL_PROGRAM_POINT_SIZE);
			GEN_CAP_CODE(PrimitiveRestartEnable, GL_PRIMITIVE_RESTART);
#undef GEN_CAP_CODE

			if (m_bPolygonSmoothStates[static_cast<int>(raster.FillMode)] != raster.PolygonOffsetEnable)
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

				m_bPolygonSmoothStates[static_cast<int>(raster.FillMode)] = raster.PolygonOffsetEnable;
				if (raster.PolygonOffsetEnable)
				{
					glEnable(cap);
				}
				else
				{
					glDisable(cap);
				}
			}
			
			if (m_bPolygonSmoothStates[static_cast<int>(raster.FillMode)])
			{
				if (m_RasterizerState.PolygonOffset.factor != raster.PolygonOffset.factor || m_RasterizerState.PolygonOffset.units != raster.PolygonOffset.units)
				{
					glPolygonOffset(raster.PolygonOffset.factor, raster.PolygonOffset.units);
					m_RasterizerState.PolygonOffset = raster.PolygonOffset;
				}
			}
			
			if (m_RasterizerState.LogicMode.LogicEnable != raster.LogicMode.LogicEnable)
			{
				m_RasterizerState.LogicMode.LogicEnable = raster.LogicMode.LogicEnable;
				if (raster.LogicMode.LogicEnable)
				{
					glEnable(GL_COLOR_LOGIC_OP);
				}
				else
				{
					glDisable(GL_COLOR_LOGIC_OP);
				}
			}

			if (raster.LogicMode.LogicEnable)
			{
				if (m_RasterizerState.LogicMode.LogicOp != raster.LogicMode.LogicOp)
				{
					glLogicOp(ConvertLogicFuncToGLenum(raster.LogicMode.LogicOp));
					m_RasterizerState.LogicMode.LogicOp = raster.LogicMode.LogicOp;
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
				}
				else
				{
					glDisable(GL_POINT_SPRITE);
				}
			}

			if (raster.PointSprite.PointSpriteEnabled)
			{
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
			if (out.Max < 0.0f)
			{
				glGetFloatv(GL_ALIASED_LINE_WIDTH_RANGE, (GLfloat*)&out);
			}
			
			return out;
		}
		// returns two values: the smallest and largest supported sizes for points. The smallest size must be at most 1, and the largest size must be at least 1.
		Rangef GLStates::getAliasedPointSizeRange()
		{
			static Rangef out = { -1, -1 };
			if (out.Max < 0.0f)
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
				CHECK_GL(glBindBuffer(gl_target, buffer));
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
			bool bSame = true;
			for (int i = 0; i < 4; i++)
			{
				if (color[i] != m_ClearColor[i])
				{
					bSame = false;
					m_ClearColor[i] = color[i];
				}
			}

			if (!bSame)
			{
				glClearColor(color[0], color[1], color[2], color[3]);
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
		{
			GLint maxTextureUnits = getMaxCombinedTextureImageUnits();
			assert(maxTextureUnits >= 8);
			m_TextureStates = new BindTexture[maxTextureUnits];
			m_TextureUnits = new GLuint[maxTextureUnits];
			m_TextureNames = new GLuint[maxTextureUnits];

			GLint maxViewports = getMaxViewports();
			m_ViewportStates = new ViewportDesc[maxViewports];
		}

		GLStates::~GLStates()
		{
			delete[] m_TextureStates;
			delete[] m_TextureUnits;

			delete[] m_ViewportStates;
			delete[] m_TextureNames;
		}

		GLint GLStates::getMaxComputeShaderStorageBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxCombinedShaderStorageBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxComputeUniformBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxComputeTextureImageUnits()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxComputeUniformComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxComputeAtomicCounters()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxComputeAtomicCounterBuffers()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxCombinedComputeUniformComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxComputeWorkGroupInvocations()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxComputeWorkGroupCount(int index)
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, index, &out);
			}
			return out;
		}

		GLint GLStates::getMaxComputeWorkGroupSize(int index)
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, index, &out);
			}
			return out;
		}

		GLint GLStates::getMaxDebugGroupStackDepth()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH, &out);
			}
			return out;
		}

		GLint GLStates::getDebugGroupStackDepth()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_DEBUG_GROUP_STACK_DEPTH, &out);
			}
			return out;
		}

		GLint GLStates::getContextFlags()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_CONTEXT_FLAGS, &out);
			}
			return out;
		}

		GLboolean GLStates::getDoublebuffer()
		{
			static GLboolean out = 2;
			if (out == 2){
				glGetBooleanv(GL_DOUBLEBUFFER, &out);
			}
			return out;
		}

		GLint GLStates::getMax3dTextureSize()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE, &out);
			}
			return out;
		}

		GLint GLStates::getMaxArrayTextureLayers()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxClipDistances()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_CLIP_DISTANCES, &out);
			}
			return out;
		}

		GLint GLStates::getMaxColorTextureSamples()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &out);
			}
			return out;
		}

		GLint GLStates::getMaxCombinedAtomicCounters()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxCombinedFragmentUniformComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxCombinedGeometryUniformComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxCombinedTextureImageUnits()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxCombinedUniformBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxCombinedVertexUniformComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxCubeMapTextureSize()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE, &out);
			}
			return out;
		}

		GLint GLStates::getMaxDepthTextureSamples()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &out);
			}
			return out;
		}

		GLint GLStates::getMaxDrawBuffers()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_DRAW_BUFFERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxDualSourceDrawBuffers()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxElementsIndices()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_ELEMENTS_INDICES, &out);
			}
			return out;
		}

		GLint GLStates::getMaxElementsVertices()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_ELEMENTS_VERTICES, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFragmentAtomicCounters()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFragmentShaderStorageBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFragmentInputComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFragmentUniformComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFragmentUniformVectors()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFragmentUniformBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFramebufferWidth()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFramebufferHeight()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFramebufferLayers()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxFramebufferSamples()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES, &out);
			}
			return out;
		}

		GLint GLStates::getMaxGeometryAtomicCounters()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxGeometryShaderStorageBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxGeometryInputComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxGeometryOutputComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxGeometryTextureImageUnits()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxGeometryUniformBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxGeometryUniformComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxIntegerSamples()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_INTEGER_SAMPLES, &out);
			}
			return out;
		}

		GLint GLStates::getMinMapBufferAlignment()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT, &out);
			}
			return out;
		}

		GLint GLStates::getMaxLabelLength()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_LABEL_LENGTH, &out);
			}
			return out;
		}

		GLint GLStates::getMaxProgramTexelOffset()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET, &out);
			}
			return out;
		}

		GLint GLStates::getMinProgramTexelOffset()
		{
			static GLint out = 0x7fffffff;
			if (out == 0x7fffffff){
				glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET, &out);
			}
			return out;
		}

		GLint GLStates::getMaxRectangleTextureSize()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE, &out);
			}
			return out;
		}

		GLint GLStates::getMaxRenderbufferSize()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE, &out);
			}
			return out;
		}

		GLint GLStates::getMaxSampleMaskWords()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxServerWaitTimeout()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT, &out);
			}
			return out;
		}

		GLint GLStates::getMaxShaderStorageBufferBindings()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxTessControlAtomicCounters()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxTessEvaluationAtomicCounters()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxTessControlShaderStorageBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxTessEvaluationShaderStorageBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxTextureBufferSize()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE, &out);
			}
			return out;
		}

		GLint GLStates::getMaxTextureImageUnits()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &out);
			}
			return out;
		}

		GLfloat GLStates::getMaxTextureLodBias()
		{
			static GLfloat out = -1;
			if (out == -1){
				glGetFloatv(GL_MAX_TEXTURE_LOD_BIAS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxTextureSize()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_TEXTURE_SIZE, &out);
			}
			return out;
		}

		GLint GLStates::getMaxUniformBufferBindings()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxUniformBlockSize()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, &out);
			}
			return out;
		}

		GLint GLStates::getMaxUniformLocations()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVaryingComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VARYING_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVaryingVectors()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VARYING_VECTORS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVaryingFloats()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VARYING_FLOATS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVertexAtomicCounters()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVertexAttribs()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVertexShaderStorageBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVertexTextureImageUnits()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVertexUniformComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVertexUniformVectors()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVertexOutputComponents()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVertexUniformBlocks()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxViewportDims()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VIEWPORT_DIMS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxViewports()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VIEWPORTS, &out);
			}
			return out;
		}

		GLint GLStates::getNumCompressedTextureFormats()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &out);
			}
			return out;
		}

		GLint GLStates::getNumExtensions()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_NUM_EXTENSIONS, &out);
			}
			return out;
		}

		GLint GLStates::getNumProgramBinaryFormats()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &out);
			}
			return out;
		}

		GLint GLStates::getNumShaderBinaryFormats()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS, &out);
			}
			return out;
		}

		GLint GLStates::getSmoothLineWidthGranularity()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_SMOOTH_LINE_WIDTH_GRANULARITY, &out);
			}
			return out;
		}

		GLboolean GLStates::getStereo()
		{
			static GLboolean out = 2;
			if (out == 2){
				glGetBooleanv(GL_STEREO, &out);
			}
			return out;
		}

		GLint GLStates::getSubpixelBits()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_SUBPIXEL_BITS, &out);
			}
			return out;
		}

		GLint GLStates::getViewportSubpixelBits()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_VIEWPORT_SUBPIXEL_BITS, &out);
			}
			return out;
		}

		GLint GLStates::getMaxElementIndex()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_ELEMENT_INDEX, &out);
			}
			return out;
		}

		GLint GLStates::getSampleBuffers()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_SAMPLE_BUFFERS, &out);
			}
			return out;
		}

		GLint GLStates::getSamples()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_SAMPLES, &out);
			}
			return out;
		}

		GLint GLStates::getShaderStorageBufferOffsetAlignment()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT, &out);
			}
			return out;
		}

		GLint GLStates::getTextureBufferOffsetAlignment()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT, &out);
			}
			return out;
		}

		GLint GLStates::getMaxVertexAttribRelativeOffset()
		{
			static GLint out = -1;
			if (out == -1){
				glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET, &out);
			}
			return out;
		}
	}
}
