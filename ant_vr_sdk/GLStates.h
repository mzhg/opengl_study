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
			static GLStates& get();

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

			void setViewports(unsigned int count, const ViewportDesc* pViewports);
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
			GLuint getBindingVAO() const { return m_VertexArrayState; }

			BlendDesc getBlendState() const { return m_BlendState; }
			DepthStencilDesc getDepthStencilState() const { return m_DepthStencilState; }
			RasterizerDesc getRasterizerState() const { return m_RasterizerState; }

			void setActiveTexture(unsigned int unit = 0);
			GLuint getActiveTexture() const { return m_ActiveTextureUnit; }
			void resetActiveTexture(bool force = false);
			void restoreActiveTexture();

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
			void resetClearColor(bool force = false);
			void resetClearDepth(bool force = false);
			void resetClearStencil(bool force = false);

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

			//data returns one value, the maximum number of active shader storage blocks that may be accessed by a compute shader.
			static GLint getMaxComputeShaderStorageBlocks();
			//data returns one value, the maximum total number of active shader storage blocks that may be accessed by all active shaders.
			static GLint getMaxCombinedShaderStorageBlocks();
			//data returns one value, the maximum number of uniform blocks per compute shader. The value must be at least 14. See glUniformBlockBinding.
			static GLint getMaxComputeUniformBlocks();
			//data returns one value, the maximum supported texture image units that can be used to access texture maps from the compute shader. The value may be at least 16. See glActiveTexture.
			static GLint getMaxComputeTextureImageUnits();
			//data returns one value, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a compute shader. The value must be at least 1024. See glUniform.
			static GLint getMaxComputeUniformComponents();
			//data returns a single value, the maximum number of atomic counters available to compute shaders.
			static GLint getMaxComputeAtomicCounters();
			//data returns a single value, the maximum number of atomic counter buffers that may be accessed by a compute shader.
			static GLint getMaxComputeAtomicCounterBuffers();
			//data returns one value, the number of words for compute shader uniform variables in all uniform blocks (including default). The value must be at least 1. See glUniform.
			static GLint getMaxCombinedComputeUniformComponents();
			//data returns one value, the number of invocations in a single local work group (i.e., the product of the three dimensions) that may be dispatched to a compute shader.
			static GLint getMaxComputeWorkGroupInvocations();
			//Accepted by the indexed versions of glGet. data the maximum number of work groups that may be dispatched to a compute shader. Indices 0, 1, and 2 correspond to the X, Y and Z dimensions, respectively.
			static GLint getMaxComputeWorkGroupCount(int index);
			//Accepted by the indexed versions of glGet. data the maximum size of a work groups that may be used during compilation of a compute shader. Indices 0, 1, and 2 correspond to the X, Y and Z dimensions, respectively.
			static GLint getMaxComputeWorkGroupSize(int index);
			//data returns a single value, the maximum depth of the debug message group stack.
			static GLint getMaxDebugGroupStackDepth();
			//data returns a single value, the current depth of the debug message group stack.
			static GLint getDebugGroupStackDepth();
			//data returns one value, the flags with which the context was created (such as debugging functionality).
			static GLint getContextFlags();
			//data returns a single boolean value indicating whether double buffering is supported.
			static GLboolean getDoublebuffer();
			//data returns one value, a rough estimate of the largest 3D texture that the GL can handle. The value must be at least 64. Use GL_PROXY_TEXTURE_3D to determine if a texture is too large. See glTexImage3D.
			static GLint getMax3dTextureSize();
			//data returns one value. The value indicates the maximum number of layers allowed in an array texture, and must be at least 256. See glTexImage2D.
			static GLint getMaxArrayTextureLayers();
			//data returns one value, the maximum number of application-defined clipping distances. The value must be at least 8.
			static GLint getMaxClipDistances();
			//data returns one value, the maximum number of samples in a color multisample texture.
			static GLint getMaxColorTextureSamples();
			//data returns a single value, the maximum number of atomic counters available to all active shaders.
			static GLint getMaxCombinedAtomicCounters();
			//data returns one value, the number of words for fragment shader uniform variables in all uniform blocks (including default). The value must be at least 1. See glUniform.
			static GLint getMaxCombinedFragmentUniformComponents();
			//data returns one value, the number of words for geometry shader uniform variables in all uniform blocks (including default). The value must be at least 1. See glUniform.
			static GLint getMaxCombinedGeometryUniformComponents();
			//data returns one value, the maximum supported texture image units that can be used to access texture maps from the vertex shader and the fragment processor combined. If both the vertex shader and the fragment processing stage access the same texture image unit, then that counts as using two texture image units against this limit. The value must be at least 48. See glActiveTexture.
			static GLint getMaxCombinedTextureImageUnits();
			//data returns one value, the maximum number of uniform blocks per program. The value must be at least 70. See glUniformBlockBinding.
			static GLint getMaxCombinedUniformBlocks();
			//data returns one value, the number of words for vertex shader uniform variables in all uniform blocks (including default). The value must be at least 1. See glUniform.
			static GLint getMaxCombinedVertexUniformComponents();
			//data returns one value. The value gives a rough estimate of the largest cube-map texture that the GL can handle. The value must be at least 1024. Use GL_PROXY_TEXTURE_CUBE_MAP to determine if a texture is too large. See glTexImage2D.
			static GLint getMaxCubeMapTextureSize();
			//data returns one value, the maximum number of samples in a multisample depth or depth-stencil texture.
			static GLint getMaxDepthTextureSamples();
			//data returns one value, the maximum number of simultaneous outputs that may be written in a fragment shader. The value must be at least 8. See glDrawBuffers.
			static GLint getMaxDrawBuffers();
			//data returns one value, the maximum number of active draw buffers when using dual-source blending. The value must be at least 1. See glBlendFunc and glBlendFuncSeparate.
			static GLint getMaxDualSourceDrawBuffers();
			//data returns one value, the recommended maximum number of vertex array indices. See glDrawRangeElements.
			static GLint getMaxElementsIndices();
			//data returns one value, the recommended maximum number of vertex array vertices. See glDrawRangeElements.
			static GLint getMaxElementsVertices();
			//data returns a single value, the maximum number of atomic counters available to fragment shaders.
			static GLint getMaxFragmentAtomicCounters();
			//data returns one value, the maximum number of active shader storage blocks that may be accessed by a fragment shader.
			static GLint getMaxFragmentShaderStorageBlocks();
			//data returns one value, the maximum number of components of the inputs read by the fragment shader, which must be at least 128.
			static GLint getMaxFragmentInputComponents();
			//data returns one value, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a fragment shader. The value must be at least 1024. See glUniform.
			static GLint getMaxFragmentUniformComponents();
			//data returns one value, the maximum number of individual 4-vectors of floating-point, integer, or boolean values that can be held in uniform variable storage for a fragment shader. The value is equal to the value of GL_MAX_FRAGMENT_UNIFORM_COMPONENTS divided by 4 and must be at least 256. See glUniform.
			static GLint getMaxFragmentUniformVectors();
			//data returns one value, the maximum number of uniform blocks per fragment shader. The value must be at least 12. See glUniformBlockBinding.
			static GLint getMaxFragmentUniformBlocks();
			//data returns one value, the maximum width for a framebuffer that has no attachments, which must be at least 16384. See glFramebufferParameter.
			static GLint getMaxFramebufferWidth();
			//data returns one value, the maximum height for a framebuffer that has no attachments, which must be at least 16384. See glFramebufferParameter.
			static GLint getMaxFramebufferHeight();
			//data returns one value, the maximum number of layers for a framebuffer that has no attachments, which must be at least 2048. See glFramebufferParameter.
			static GLint getMaxFramebufferLayers();
			//data returns one value, the maximum samples in a framebuffer that has no attachments, which must be at least 4. See glFramebufferParameter.
			static GLint getMaxFramebufferSamples();
			//data returns a single value, the maximum number of atomic counters available to geometry shaders.
			static GLint getMaxGeometryAtomicCounters();
			//data returns one value, the maximum number of active shader storage blocks that may be accessed by a geometry shader.
			static GLint getMaxGeometryShaderStorageBlocks();
			//data returns one value, the maximum number of components of inputs read by a geometry shader, which must be at least 64.
			static GLint getMaxGeometryInputComponents();
			//data returns one value, the maximum number of components of outputs written by a geometry shader, which must be at least 128.
			static GLint getMaxGeometryOutputComponents();
			//data returns one value, the maximum supported texture image units that can be used to access texture maps from the geometry shader. The value must be at least 16. See glActiveTexture.
			static GLint getMaxGeometryTextureImageUnits();
			//data returns one value, the maximum number of uniform blocks per geometry shader. The value must be at least 12. See glUniformBlockBinding.
			static GLint getMaxGeometryUniformBlocks();
			//data returns one value, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a geometry shader. The value must be at least 1024. See glUniform.
			static GLint getMaxGeometryUniformComponents();
			//data returns one value, the maximum number of samples supported in integer format multisample buffers.
			static GLint getMaxIntegerSamples();
			//data returns one value, the minimum alignment in basic machine units of pointers returned fromglMapBuffer and glMapBufferRange. This value must be a power of two and must be at least 64.
			static GLint getMinMapBufferAlignment();
			//data returns one value, the maximum length of a label that may be assigned to an object. See glObjectLabel and glObjectPtrLabel.
			static GLint getMaxLabelLength();
			//data returns one value, the maximum texel offset allowed in a texture lookup, which must be at least 7.
			static GLint getMaxProgramTexelOffset();
			//data returns one value, the minimum texel offset allowed in a texture lookup, which must be at most -8.
			static GLint getMinProgramTexelOffset();
			//data returns one value. The value gives a rough estimate of the largest rectangular texture that the GL can handle. The value must be at least 1024. Use GL_PROXY_TEXTURE_RECTANGLE to determine if a texture is too large. See glTexImage2D.
			static GLint getMaxRectangleTextureSize();
			//data returns one value. The value indicates the maximum supported size for renderbuffers. See glFramebufferRenderbuffer.
			static GLint getMaxRenderbufferSize();
			//data returns one value, the maximum number of sample mask words.
			static GLint getMaxSampleMaskWords();
			//data returns one value, the maximum glWaitSync timeout interval.
			static GLint getMaxServerWaitTimeout();
			//data returns one value, the maximum number of shader storage buffer binding points on the context, which must be at least 8.
			static GLint getMaxShaderStorageBufferBindings();
			//data returns a single value, the maximum number of atomic counters available to tessellation control shaders.
			static GLint getMaxTessControlAtomicCounters();
			//data returns a single value, the maximum number of atomic counters available to tessellation evaluation shaders.
			static GLint getMaxTessEvaluationAtomicCounters();
			//data returns one value, the maximum number of active shader storage blocks that may be accessed by a tessellation control shader.
			static GLint getMaxTessControlShaderStorageBlocks();
			//data returns one value, the maximum number of active shader storage blocks that may be accessed by a tessellation evaluation shader.
			static GLint getMaxTessEvaluationShaderStorageBlocks();
			//data returns one value. The value gives the maximum number of texels allowed in the texel array of a texture buffer object. Value must be at least 65536.
			static GLint getMaxTextureBufferSize();
			//data returns one value, the maximum supported texture image units that can be used to access texture maps from the fragment shader. The value must be at least 16. See glActiveTexture.
			static GLint getMaxTextureImageUnits();
			//data returns one value, the maximum, absolute value of the texture level-of-detail bias. The value must be at least 2.0.
			static GLfloat getMaxTextureLodBias();
			//data returns one value. The value gives a rough estimate of the largest texture that the GL can handle. The value must be at least 1024. Use a proxy texture target such as GL_PROXY_TEXTURE_1D or GL_PROXY_TEXTURE_2D to determine if a texture is too large. See glTexImage1D and glTexImage2D.
			static GLint getMaxTextureSize();
			//data returns one value, the maximum number of uniform buffer binding points on the context, which must be at least 36.
			static GLint getMaxUniformBufferBindings();
			//data returns one value, the maximum size in basic machine units of a uniform block, which must be at least 16384.
			static GLint getMaxUniformBlockSize();
			//data returns one value, the maximum number of explicitly assignable uniform locations, which must be at least 1024.
			static GLint getMaxUniformLocations();
			//data returns one value, the number components for varying variables, which must be at least 60.
			static GLint getMaxVaryingComponents();
			//data returns one value, the number 4-vectors for varying variables, which is equal to the value of GL_MAX_VARYING_COMPONENTS and must be at least 15.
			static GLint getMaxVaryingVectors();
			//data returns one value, the maximum number of interpolators available for processing varying variables used by vertex and fragment shaders. This value represents the number of individual floating-point values that can be interpolated; varying variables declared as vectors, matrices, and arrays will all consume multiple interpolators. The value must be at least 32.
			static GLint getMaxVaryingFloats();
			//data returns a single value, the maximum number of atomic counters available to vertex shaders.
			static GLint getMaxVertexAtomicCounters();
			//data returns one value, the maximum number of 4-component generic vertex attributes accessible to a vertex shader. The value must be at least 16. See glVertexAttrib.
			static GLint getMaxVertexAttribs();
			//data returns one value, the maximum number of active shader storage blocks that may be accessed by a vertex shader.
			static GLint getMaxVertexShaderStorageBlocks();
			//data returns one value, the maximum supported texture image units that can be used to access texture maps from the vertex shader. The value may be at least 16. See glActiveTexture.
			static GLint getMaxVertexTextureImageUnits();
			//data returns one value, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a vertex shader. The value must be at least 1024. See glUniform.
			static GLint getMaxVertexUniformComponents();
			//data returns one value, the maximum number of 4-vectors that may be held in uniform variable storage for the vertex shader. The value of GL_MAX_VERTEX_UNIFORM_VECTORS is equal to the value of GL_MAX_VERTEX_UNIFORM_COMPONENTS and must be at least 256.
			static GLint getMaxVertexUniformVectors();
			//data returns one value, the maximum number of components of output written by a vertex shader, which must be at least 64.
			static GLint getMaxVertexOutputComponents();
			//data returns one value, the maximum number of uniform blocks per vertex shader. The value must be at least 12. See glUniformBlockBinding.
			static GLint getMaxVertexUniformBlocks();
			//data returns two values: the maximum supported width and height of the viewport. These must be at least as large as the visible dimensions of the display being rendered to. See glViewport.
			static GLint getMaxViewportDims();
			//data returns one value, the maximum number of simultaneous viewports that are supported. The value must be at least 16. See glViewportIndexed.
			static GLint getMaxViewports();
			//data returns a single integer value indicating the number of available compressed texture formats. The minimum value is 4. See glCompressedTexImage2D.
			static GLint getNumCompressedTextureFormats();
			//data returns one value, the number of extensions supported by the GL implementation for the current context. See glGetString.
			static GLint getNumExtensions();
			//data returns one value, the number of program binary formats supported by the implementation.
			static GLint getNumProgramBinaryFormats();
			//data returns one value, the number of binary shader formats supported by the implementation. If this value is greater than zero, then the implementation supports loading binary shaders. If it is zero, then the loading of binary shaders by the implementation is not supported.
			static GLint getNumShaderBinaryFormats();
			//data returns a single value indicating the level of quantization applied to smooth line width parameters.
			static GLint getSmoothLineWidthGranularity();
			//data returns a single boolean value indicating whether stereo buffers (left and right) are supported.
			static GLboolean getStereo();
			//data returns one value, an estimate of the number of bits of subpixel resolution that are used to position rasterized geometry in window coordinates. The value must be at least 4.
			static GLint getSubpixelBits();
			//data returns a single value, the number of bits of sub-pixel precision which the GL uses to interpret the floating point viewport bounds. The minimum value is 0.
			static GLint getViewportSubpixelBits();
			//data returns a single value, the maximum index that may be specified during the transfer of generic vertex attributes to the GL.
			static GLint getMaxElementIndex();
			//data returns a single integer value indicating the number of sample buffers associated with the framebuffer. See glSampleCoverage.
			static GLint getSampleBuffers();
			//data returns a single integer value indicating the coverage mask size. See glSampleCoverage.
			static GLint getSamples();
			//data returns a single value, the minimum required alignment for shader storage buffer sizes and offset. The initial value is 1. See glShaderStorageBlockBinding.
			static GLint getShaderStorageBufferOffsetAlignment();
			//data returns a single value, the minimum required alignment for texture buffer sizes and offset. The initial value is 1. See glUniformBlockBinding.
			static GLint getTextureBufferOffsetAlignment();
			//data returns a single integer value containing the maximum offset that may be added to a vertex binding offset.
			static GLint getMaxVertexAttribRelativeOffset();

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
				GLenum target;
				GLuint textureID;
				bool   tag;

				BindTexture() :
					target(0), textureID(0), tag(false){}
			};

			BindTexture* m_TextureStates;
			GLuint     * m_TextureUnits;
			GLuint     * m_TextureNames;
			unsigned int m_TextureCount;

			ViewportDesc* m_ViewportStates;
			unsigned int m_ViewportCount;

			GLuint m_ActiveTextureUnit;
			GLuint m_BufferStates[static_cast<int>(BufferTarget::COUNT)];
			bool   m_bPolygonSmoothStates[3];

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

