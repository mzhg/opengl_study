#pragma once

#include "Texture.h"

namespace jet
{
	namespace util
	{
		// Single-ton class.
		class GLStates
		{
		public:
			static GLStates& getGLStates();

			// Retrive all of the gl-states from the opengl context.
			void restoreAll();
			// Reset all of the states to default setting.
			void resetAll(bool force = false);
			// Set the Blend state, pass NULL to make it default.
			void setBlendState(const BlendDesc* pBlend);
			void restoreBlendState();
			void resetBlendState(bool force = false);

			void setDSState(const DepthStencilDesc* pDS);
			void restoreDSState();
			void resetDSState(bool force = false);

			void setRSState(const RasterizerDesc* pRaster);
			void restoreRSState();
			void resetRSState(bool force = false);

			void setViewport(GLint x, GLint y, GLint width, GLint height)
			{
				Rectangle2i viewport = {x,y,width, height};
				setViewports(1, &viewport);
			}
			void setViewports(unsigned int count, const Rectangle2i* pViewports);
			void restoreViewport(unsigned int index = 0);

			void bindProgram(GLuint program);
			GLuint getProgram() const { return m_ProgramState; }
			void restoreProgram();
			void resetProgram(bool force = false);

			void bindFramebuffer(GLuint framebuffer, FramebufferTarget target = FramebufferTarget::FRAMEBUFFER);
			void restoreFramebuffer(FramebufferTarget target = FramebufferTarget::FRAMEBUFFER);
			void resetFramebuffer(FramebufferTarget target = FramebufferTarget::FRAMEBUFFER, bool force = false);
			GLuint getBindingFramebuffer(FramebufferTarget target = FramebufferTarget::FRAMEBUFFER) const
			{
				return m_FramebufferState[static_cast<int>(target)];
			}

			void bindTextures(unsigned int count, const TextureGL** pTextures, const unsigned int* units = nullptr);
			void restoreTextures();
			void resetTextures(bool force = false);

			void bindVAO(GLuint vao);
			void restoreVAO();
			void resetVAO(bool force = false);

			BlendDesc getBlendState() const { return m_BlendState; }
			DepthStencilDesc getDepthStencilState() const { return m_DepthStencilState; }
			RasterizerDesc getRasterizerState() const { return m_RasterizerState; }

			void setActiveTexture(unsigned int unit = 0);
			GLuint getActiveTexture() const { return m_ActiveTextureUnit; }
			void resetActiveTexture(bool force = false);
			void restoreActiveTexture();

			// returns a pair of values indicating the range of widths supported for aliased lines. See glLineWidth.
			static Rangef getAliasedLineWidthRange();
			// returns two values: the smallest and largest supported sizes for points. The smallest size must be at most 1, and the largest size must be at least 1.
			static Rangef getAliasedPointSizeRange();

			// returns a list of symbolic constants of length GL_NUM_COMPRESSED_TEXTURE_FORMATS indicating which compressed texture formats are available. See glCompressedTexImage2D.
			static const GLenum* getCompressedTextureFormats(GLuint& length);
			// return an array of GL_NUM_PROGRAM_BINARY_FORMATS values, indicating the proram binary formats supported by the implementation.
			static const GLenum* getProgramBinaryFormats(GLuint& length);
			// returns two values, the minimum and maximum viewport bounds range. The minimum range should be at least [-32768, 32767].
			static Rangei getViewportBoundsRange();

			void bindProgramPipeline(GLuint program);
			GLuint getProgramPipeline() const { return m_ProgramPipelineState; }
			void restoreProgramPipeline();
			void resetProgramPipeline(bool force = false);

			void setHint(HintTarget target, HintMode mode);
			void restoreHint(HintTarget target);
			void resetHint(HintTarget target, bool force = false);
			HintMode getHint(FramebufferTarget target) const
			{
				return m_HintStates[static_cast<int>(target)];
			}

			void bindBuffer(GLuint buffer, BufferTarget target);
			GLuint getBindingBuffer(BufferTarget target) const
			{
				return m_BufferStates[static_cast<int>(target)];
			}
			void resetBuffer(BufferTarget target,bool force = false);
			void restoreBuffer(BufferTarget target);

			void setClearColor(GLfloat r, GLfloat g, GLfloat b, GLfloat a);
			void setClearColor(GLfloat color[4]);
			void getClearColor(GLfloat color[4]) const 
			{ 
				color[0] = m_ClearColor[0];
				color[1] = m_ClearColor[1];
				color[2] = m_ClearColor[2];
				color[3] = m_ClearColor[3];
			}

			void restoreClearColor();

			void setClearDepth(GLfloat depth);
			GLfloat getClearDepth() const
			{
				return m_ClearDepth;
			}

			void restoreClearDepth();

			void setClearStencil(GLint stencil);
			GLint getClearStencil() const
			{
				return m_ClearStencil;
			}

			void restoreClearStencil();

			void setLineWidth(GLfloat width);
			GLfloat getLineWidth()const { return m_LineWidth; }
			void restoreLineWidth();
			void resetLineWidth(bool force = false);

			void setPointSize(GLfloat size);
			GLfloat getPointSize()const { return m_PointSize; }
			void restorePointSize();
			void resetPointSize(bool force = false);

			void setPrimitiveRestartIndex(GLint size);
			GLint getPrimitiveRestartIndex()const { return m_PrimitiveRestartIndex; }
			void restorePrimitiveRestartIndex();
			void resetPrimitiveRestartIndex(bool force = false);

			void setRenderbuffer(GLuint renderbuffer);
			GLuint getRenderbuffer()const { return m_RenderbufferState; }
			void restoreRenderbuffer();
			void resetRenderbuffer(bool force = false);

			void setSampler(GLuint sampler, GLuint unit = 0);
			GLuint getSampler(GLuint unit = 0)const { return m_SamplerStates[unit]; }
			void restoreSampler(GLuint unit = 0);
			void resetSampler(GLuint unit = 0, bool force = false);

#if 0
			void setProvoke(ProvokeMode mode);
			ProvokeMode getProvoke() const { return m_ProvokeState; }
			void restoreProvoke();
			void resetProvoke(bool force = false);
#endif

			~GLStates();
		public:
			static const unsigned MAX_TEXTURES_BINDING = 80;

		private:

			void setBlendState(const BlendDesc& blend);
			void setDSState(const DepthStencilDesc& ds);
			void setRSState(const RasterizerDesc& raster);

			GLStates();
			GLStates(GLStates&) = delete;
			GLStates operator=(GLStates&) = delete;

			BlendDesc m_BlendState;
			DepthStencilDesc m_DepthStencilState;
			RasterizerDesc  m_RasterizerState;

			GLuint m_ProgramState;  // Current program
			GLuint m_ProgramPipelineState; //Current Program Pipeline.
			GLuint m_FramebufferState[3];  // Current framebuffer object
			GLuint m_VertexArrayState;  // Current Vertex Array Object

			struct BindTexture
			{
				GLenum Target;
				GLuint TextureID;
				GLuint Unit;

				BindTexture() :
					Target(0), TextureID(0), Unit(0){}
			};

			BindTexture m_TextureStates[80];
			unsigned int m_TextureCount;

			Rectangle2i m_ViewportStates[16];
			unsigned int m_ViewportCount;

			GLuint m_ActiveTextureUnit;
			GLuint m_BufferStates[static_cast<int>(BufferTarget::COUNT)];

			GLfloat m_ClearColor[4];
			GLfloat m_ClearDepth;
			GLint   m_ClearStencil;

			// line width
			GLfloat m_LineWidth;
			GLfloat m_PointSize;

			HintMode m_HintStates[4];
			ProvokeMode m_ProvokeState;

			GLint m_PrimitiveRestartIndex;
			GLuint m_RenderbufferState;
			GLuint m_SamplerStates[MAX_TEXTURES_BINDING];
		};

	}
}

