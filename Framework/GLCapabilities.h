#pragma once
#include <set>

namespace jet
{
	namespace util
	{
		class GLCapabilities
		{
		private:
			GLCapabilities(std::set<std::string>& ext, bool fc);

		public:
			static const GLCapabilities* get();

			static void release();

		public:
			/** When true, {@link GL11} is supported. */
			const bool OpenGL11;
			/** When true, {@link GL12} is supported. */
			const bool OpenGL12;
			/** When true, {@link GL13} is supported. */
			const bool OpenGL13;
			/** When true, {@link GL14} is supported. */
			const bool OpenGL14;
			/** When true, {@link GL15} is supported. */
			const bool OpenGL15;
			/** When true, {@link GL20} is supported. */
			const bool OpenGL20;
			/** When true, {@link GL21} is supported. */
			const bool OpenGL21;
			/** When true, {@link GL30} is supported. */
			const bool OpenGL30;
			/** When true, {@link GL31} is supported. */
			const bool OpenGL31;
			/** When true, {@link GL32} is supported. */
			const bool OpenGL32;
			/** When true, {@link GL33} is supported. */
			const bool OpenGL33;
			/** When true, {@link GL40} is supported. */
			const bool OpenGL40;
			/** When true, {@link GL41} is supported. */
			const bool OpenGL41;
			/** When true, {@link GL42} is supported. */
			const bool OpenGL42;
			/** When true, {@link GL43} is supported. */
			const bool OpenGL43;
			/** When true, {@link GL44} is supported. */
			const bool OpenGL44;
			/** When true, {@link GL45} is supported. */
			const bool OpenGL45;
			/** When true, {@link AMDBlendMinmaxFactor} is supported. */
			const bool AMD_blend_minmax_factor;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/conservative_depth.txt">AMD_conservative_depth</a> extension is supported.
			*
			* <p>There is a common optimization for hardware accelerated implementation of OpenGL which relies on an early depth test to be run before the fragment
			* shader so that the shader evaluation can be skipped if the fragment ends up being discarded because it is occluded.</p>
			*
			* <p>This optimization does not affect the final rendering, and is typically possible when the fragment does not change the depth programmatically. (i.e.: it
			* does not write to the built-in {@code gl_FragDepth} output). There are, however a class of operations on the depth in the shader which could still be
			* performed while allowing the early depth test to operate.</p>
			*
			* <p>This extension allows the application to pass enough information to the GL implementation to activate such optimizations safely.</p>
			*
			* <p>Requires {@link GL30 OpenGL 3.0}. Promoted to core in {@link GL42 OpenGL 4.2}.</p>
			*/
			const bool AMD_conservative_depth;
			/** When true, {@link AMDDebugOutput} is supported. */
			const bool AMD_debug_output;
			/** When true, {@link AMDDepthClampSeparate} is supported. */
			const bool AMD_depth_clamp_separate;
			/** When true, {@link AMDDrawBuffersBlend} is supported. */
			const bool AMD_draw_buffers_blend;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/gcn_shader.txt">AMD_gcn_shader</a> extension is supported.
			*
			* <p>This extension exposes miscellaneous features of the AMD "Graphics Core Next" shader architecture that do not cleanly fit into other extensions
			* and are not significant enough alone to warrant their own extensions. This includes cross-SIMD lane ballots, cube map query functions and a
			* functionality to query the elapsed shader core time.</p>
			*
			* <p>Requires {@link #AMD_gpu_shader_int64 AMD_gpu_shader_int64} or {@link #NV_gpu_shader5 NV_gpu_shader5}.</p>
			*/
			const bool AMD_gcn_shader;
			/** When true, {@link AMDGPUShaderHalfFloat} is supported. */
			const bool AMD_gpu_shader_half_float;
			/** When true, {@link AMDGPUShaderInt64} is supported. */
			const bool AMD_gpu_shader_int64;
			/** When true, {@link AMDInterleavedElements} is supported. */
			const bool AMD_interleaved_elements;
			/** When true, {@link AMDOcclusionQueryEvent} is supported. */
			const bool AMD_occlusion_query_event;
			/** When true, {@link AMDPerformanceMonitor} is supported. */
			const bool AMD_performance_monitor;
			/** When true, {@link AMDPinnedMemory} is supported. */
			const bool AMD_pinned_memory;
			/** When true, {@link AMDQueryBufferObject} is supported. */
			const bool AMD_query_buffer_object;
			/** When true, {@link AMDSamplePositions} is supported. */
			const bool AMD_sample_positions;
			/** When true, {@link AMDSeamlessCubemapPerTexture} is supported. */
			const bool AMD_seamless_cubemap_per_texture;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/shader_atomic_counter_ops.txt">AMD_shader_atomic_counter_ops</a> extension is supported.
			*
			* <p>This extension is written against the OpenGL 4.3 (core) specification and the GLSL 4.30.7 specification.</p>
			*
			* <p>Requires {@link GL42 OpenGL 4.2} or {@link #ARB_shader_atomic_counters ARB_shader_atomic_counters}.</p>
			*/
			const bool AMD_shader_atomic_counter_ops;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/shader_ballot.txt">AMD_shader_ballot</a> extension is supported.
			*
			* <p>The extensions {@code ARB_shader_group_vote} and {@code ARB_shader_ballot} introduced the concept of sub-groups and a set of operations that allow data
			* exchange across shader invocations within a sub-group.</p>
			*
			* <p>This extension further extends the capabilities of these extensions with additional sub-group operations.</p>
			*
			* <p>Requires {@link #ARB_shader_group_vote ARB_shader_group_vote}, {@link #ARB_shader_ballot ARB_shader_ballot} and {@link ARBGPUShaderInt64 ARB_gpu_shader_int64} or {@link AMDGPUShaderInt64 AMD_gpu_shader_int64}.</p>
			*/
			const bool AMD_shader_ballot;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/shader_explicit_vertex_parameter.txt">AMD_shader_explicit_vertex_parameter</a> extension is supported.
			*
			* <p>Unextended GLSL provides a set of fixed function interpolation modes and even those are limited to certain types of interpolants (for example,
			* interpolation of integer and double isn't supported).</p>
			*
			* <p>This extension introduces new built-in functions allowing access to vertex parameters explicitly in the fragment shader. It also exposes barycentric
			* coordinates as new built-in variables, which can be used to implement custom interpolation algorithms using shader code.</p>
			*
			* <p>Requires {@link GL20 OpenGL 2.0} or {@link ARBShaderObjects ARB_shader_objects}.</p>
			*/
			const bool AMD_shader_explicit_vertex_parameter;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/shader_stencil_export.txt">AMD_shader_stencil_export</a> extension is supported.
			*
			* <p>In OpenGL, the stencil test is a powerful mechanism to selectively discard fragments based on the content of the stencil buffer. However, facilites to
			* update the content of the stencil buffer are limited to operations such as incrementing the existing value, or overwriting with a fixed reference value.</p>
			*
			* <p>This extension provides a mechanism whereby a shader may generate the stencil reference value per invocation. When stencil testing is enabled, this
			* allows the test to be performed against the value generated in the shader. When the stencil operation is set to {@link GL11#REPLACE REPLACE}, this allows a value generated
			* in the shader to be written to the stencil buffer directly.</p>
			*
			* <p>Requires {@link #ARB_fragment_shader ARB_fragment_shader}.</p>
			*/
			const bool AMD_shader_stencil_export;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/shader_trinary_minmax.txt">AMD_shader_trinary_minmax</a> extension is supported.
			*
			* <p>This extension introduces three new trinary built-in functions to the OpenGL Shading Languages. These functions allow the minimum, maximum or median of
			* three inputs to be found with a single function call. These operations may be useful for sorting and filtering operations, for example. By explicitly
			* performing a trinary operation with a single built-in function, shader compilers and optimizers may be able to generate better instruction sequences for
			* perform sorting and other multi-input functions.</p>
			*
			* <p>Requires {@link GL20 OpenGL 2.0} or {@link #ARB_shader_objects ARB_shader_objects}.</p>
			*/
			const bool AMD_shader_trinary_minmax;
			/** When true, {@link AMDSparseTexture} is supported. */
			const bool AMD_sparse_texture;
			/** When true, {@link AMDStencilOperationExtended} is supported. */
			const bool AMD_stencil_operation_extended;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/texture_texture4.txt">AMD_texture_texture4</a> extension is supported.
			*
			* <p>This extension adds new shading language built-in texture functions to the shading language.</p>
			*
			* <p>These texture functions may be used to access one component textures.</p>
			*
			* <p>The {@code texture4} built-in function returns a texture value derived from a 2x2 set of texels in the image array of level levelbase is selected. These
			* texels are selected in the same way as when the value of {@link GL11#TEXTURE_MIN_FILTER TEXTURE_MIN_FILTER} is {@link GL11#LINEAR LINEAR}, but instead of these texels being filtered to generate the
			* texture value, the R, G, B and A texture values are derived directly from these four texels.</p>
			*/
			const bool AMD_texture_texture4;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/transform_feedback3_lines_triangles.txt">AMD_transform_feedback3_lines_triangles</a> extension is supported.
			*
			* <p>OpenGL 4.0 introduced the ability to record primitives into multiple output streams using transform feedback. However, the restriction that all streams
			* must output {@link GL11#POINT POINT} primitives when more than one output stream is active was also introduced. This extension simply removes that restriction, allowing
			* the same set of primitives to be used with multiple transform feedback streams as with a single stream.</p>
			*
			* <p>Requires {@link GL40 OpenGL 4.0} or {@link ARBTransformFeedback3 ARB_transform_feedback3}.</p>
			*/
			const bool AMD_transform_feedback3_lines_triangles;
			/** When true, {@link AMDTransformFeedback4} is supported. */
			const bool AMD_transform_feedback4;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/vertex_shader_layer.txt">AMD_vertex_shader_layer</a> extension is supported.
			*
			* <p>The {@code gl_Layer} built-in shading language variable was introduced with the {@link #ARB_geometry_shader4 ARB_geometry_shader4} extension and subsequently promoted to core
			* OpenGL in version 3.2. This variable is an output from the geometry shader stage that allows rendering to be directed to a specific layer of an array
			* texture, slice of a 3D texture or face of a cube map or cube map array attachment of the framebuffer. Thus, this extremely useful functionality is only
			* available if a geometry shader is present - even if the geometry shader is not otherwise required by the application. This adds overhead to the graphics
			* processing pipeline, and complexity to applications. It also precludes implementations that cannot support geometry shaders from supporting rendering to
			* layered framebuffer attachments.</p>
			*
			* <p>This extension exposes the {@code gl_Layer} built-in variable in the vertex shader, allowing rendering to be directed to layered framebuffer attachments
			* with only a vertex and fragment shader present. Combined with features such as instancing, or static vertex attributes and so on, this allows a wide
			* variety of techniques to be implemented without the requirement for a geometry shader to be present.</p>
			*
			* <p>Requires {@link GL30 OpenGL 3.0} or {@link #EXT_texture_array EXT_texture_array}.</p>
			*/
			const bool AMD_vertex_shader_layer;
			/** When true, {@link AMDVertexShaderTessellator} is supported. */
			const bool AMD_vertex_shader_tessellator;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/AMD/vertex_shader_viewport_index.txt">AMD_vertex_shader_viewport_index</a> extension is supported.
			*
			* <p>The {@code gl_ViewportIndex} built-in variable was introduced by the {@link #ARB_viewport_array ARB_viewport_array} extension and {@link GL41 OpenGL 4.1}. This variable is available
			* in un-extended OpenGL only to the geometry shader. When written in the geometry shader, it causes geometry to be directed to one of an array of several
			* independent viewport rectangles.</p>
			*
			* <p>In order to use any viewport other than zero, a geometry shader must be present. Geometry shaders introduce processing overhead and potential
			* performance issues. This extension exposes the {@code gl_ViewportIndex} built-in variable to the vertex shader, allowing the functionality introduced by
			* ARB_viewport_array to be accessed without requiring a geometry shader to be present.</p>
			*
			* <p>Requires {@link GL41 OpenGL 4.1} or {@link #ARB_viewport_array ARB_viewport_array}.</p>
			*/
			const bool AMD_vertex_shader_viewport_index;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/arrays_of_arrays.txt">ARB_arrays_of_arrays</a> extension is supported.
			*
			* <p>This extension removes the restriction that arrays cannot be formed into arrays, allowing arrays of arrays to be declared.</p>
			*
			* <p>Requires GLSL 1.2. Promoted to core in {@link GL43 OpenGL 4.3}.</p>
			*/
			const bool ARB_arrays_of_arrays;
			/** When true, {@link ARBBaseInstance} is supported. */
			const bool ARB_base_instance;
			/** When true, {@link ARBBindlessTexture} is supported. */
			const bool ARB_bindless_texture;
			/** When true, {@link ARBBlendFuncExtended} is supported. */
			const bool ARB_blend_func_extended;
			/** When true, {@link ARBBufferStorage} is supported. */
			const bool ARB_buffer_storage;
			/** When true, {@link ARBCLEvent} is supported. */
			const bool ARB_cl_event;
			/** When true, {@link ARBClearBufferObject} is supported. */
			const bool ARB_clear_buffer_object;
			/** When true, {@link ARBClearTexture} is supported. */
			const bool ARB_clear_texture;
			/** When true, {@link ARBClipControl} is supported. */
			const bool ARB_clip_control;
			/** When true, {@link ARBColorBufferFloat} is supported. */
			const bool ARB_color_buffer_float;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/compatibility.txt">ARB_compatibility</a> extension is supported.
			*
			* <p>This extension restores features deprecated by {@link GL30 OpenGL 3.0}.</p>
			*/
			const bool ARB_compatibility;
			/** When true, {@link ARBCompressedTexturePixelStorage} is supported. */
			const bool ARB_compressed_texture_pixel_storage;
			/** When true, {@link ARBComputeShader} is supported. */
			const bool ARB_compute_shader;
			/** When true, {@link ARBComputeVariableGroupSize} is supported. */
			const bool ARB_compute_variable_group_size;
			/** When true, {@link ARBConditionalRenderInverted} is supported. */
			const bool ARB_conditional_render_inverted;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/conservative_depth.txt">ARB_conservative_depth</a> extension is supported.
			*
			* <p>There is a common optimization for hardware accelerated implementation of OpenGL which relies on an early depth test to be run before the fragment
			* shader so that the shader evaluation can be skipped if the fragment ends up being discarded because it is occluded.</p>
			*
			* <p>This optimization does not affect the final rendering, and is typically possible when the fragment does not change the depth programmatically. (i.e.: it
			* does not write to the built-in gl_FragDepth output). There are, however a class of operations on the depth in the shader which could still be performed
			* while allowing the early depth test to operate.</p>
			*
			* <p>This extension allows the application to pass enough information to the GL implementation to activate such optimizations safely.</p>
			*
			* <p>Requires {@link GL30 OpenGL 3.0}. Promoted to core in {@link GL42 OpenGL 4.2}.</p>
			*/
			const bool ARB_conservative_depth;
			/** When true, {@link ARBCopyBuffer} is supported. */
			const bool ARB_copy_buffer;
			/** When true, {@link ARBCopyImage} is supported. */
			const bool ARB_copy_image;
			/** When true, {@link ARBCullDistance} is supported. */
			const bool ARB_cull_distance;
			/** When true, {@link ARBDebugOutput} is supported. */
			const bool ARB_debug_output;
			/** When true, {@link ARBDepthBufferFloat} is supported. */
			const bool ARB_depth_buffer_float;
			/** When true, {@link ARBDepthClamp} is supported. */
			const bool ARB_depth_clamp;
			/** When true, {@link ARBDepthTexture} is supported. */
			const bool ARB_depth_texture;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/derivative_control.txt">ARB_derivative_control</a> extension is supported.
			*
			* <p>This extension provides control over the spacial granularity at which the underlying implementation computes derivatives.</p>
			*
			* <p>For example, for the coarse-granularity derivative, a single x derivative could be computed for each 2x2 group of pixels, using that same derivative
			* value for all 4 pixels. For the fine-granularity derivative, two derivatives could be computed for each 2x2 group of pixels; one for the top row and one
			* for the bottom row. Implementations vary somewhat on how this is done.</p>
			*
			* <p>To select the coarse derivative, use:</p>
			*
			* <pre><code>dFdxCoarse(p)
			dFdyCoarse(p)
			fwidthCoarse(p)</code></pre>
			*
			* <p>To select the fine derivative, use:</p>
			*
			* <pre><code>dFdxFine(p)
			dFdyFine(p)
			fwidthFine(p)</code></pre>
			*
			* <p>To select which ever is "better" (based on performance, API hints, or other factors), use:</p>
			*
			* <pre><code>dFdx(p)
			dFdy(p)
			fwidth(p)</code></pre>
			*
			* <p>This last set is the set of previously existing built-ins for derivatives, and continues to work in a backward compatible way.</p>
			*
			* <p>Requires {@link GL40 OpenGL 4.0} and GLSL 4.00. Promoted to core in {@link GL45 OpenGL 4.5}.</p>
			*/
			const bool ARB_derivative_control;
			/** When true, {@link ARBDirectStateAccess} is supported. */
			const bool ARB_direct_state_access;
			/** When true, {@link ARBDrawBuffers} is supported. */
			const bool ARB_draw_buffers;
			/** When true, {@link ARBDrawBuffersBlend} is supported. */
			const bool ARB_draw_buffers_blend;
			/** When true, {@link ARBDrawElementsBaseVertex} is supported. */
			const bool ARB_draw_elements_base_vertex;
			/** When true, {@link ARBDrawIndirect} is supported. */
			const bool ARB_draw_indirect;
			/** When true, {@link ARBDrawInstanced} is supported. */
			const bool ARB_draw_instanced;
			/** When true, {@link ARBEnhancedLayouts} is supported. */
			const bool ARB_enhanced_layouts;
			/** When true, {@link ARBES2Compatibility} is supported. */
			const bool ARB_ES2_compatibility;
			/** When true, {@link ARBES31Compatibility} is supported. */
			const bool ARB_ES3_1_compatibility;
			/** When true, {@link ARBES32Compatibility} is supported. */
			const bool ARB_ES3_2_compatibility;
			/** When true, {@link ARBES3Compatibility} is supported. */
			const bool ARB_ES3_compatibility;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/explicit_attrib_location.txt">ARB_explicit_attrib_location</a> extension is supported.
			*
			* <p>This extension provides a method to pre-assign attribute locations to named vertex shader inputs and color numbers to named fragment shader outputs.
			* This allows applications to globally assign a particular semantic meaning, such as diffuse color or vertex normal, to a particular attribute location
			* without knowing how that attribute will be named in any particular shader.</p>
			*
			* <p>Requires {@link GL20 OpenGL 2.0} or {@link #ARB_vertex_shader ARB_vertex_shader}. Promoted to core in {@link GL33 OpenGL 3.3}.</p>
			*/
			const bool ARB_explicit_attrib_location;
			/** When true, {@link ARBExplicitUniformLocation} is supported. */
			const bool ARB_explicit_uniform_location;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/fragment_coord_conventions.txt">ARB_fragment_coord_conventions</a> extension is supported.
			*
			* <p>This extension provides alternative conventions for the fragment coordinate XY location available for programmable fragment processing.</p>
			*
			* <p>The scope of this extension deals *only* with how the fragment coordinate XY location appears during programming fragment processing. Beyond the scope
			* of this extension are coordinate conventions used for rasterization or transformation.</p>
			*
			* <p>In the case of the coordinate conventions for rasterization and transformation, some combination of the viewport, depth range, culling state, and
			* projection matrix state can be reconfigured to adopt other arbitrary clip-space and window-space coordinate space conventions. Adopting other clip-space
			* and window-space conventions involves adjusting existing OpenGL state. However it is non-trivial to massage an arbitrary fragment shader or program to
			* adopt a different window-space coordinate system because such shaders are encoded in various textual representations.</p>
			*
			* <p>The dominant 2D and 3D rendering APIs make two basic choices of convention when locating fragments in window space. The two choices are:</p>
			*
			* <ol>
			* <li>Is the origin nearest the lower-left- or upper-left-most pixel of the window?</li>
			* <li>Is the (x,y) location of the pixel nearest the origin at (0,0) or (0.5,0.5)?</li>
			* </ol>
			*
			* <p>OpenGL assumes a lower-left origin for window coordinates and assumes pixel centers are located at half-pixel coordinates. This means the XY location
			* (0.5,0.5) corresponds to the lower-left-most pixel in a window.</p>
			*
			* <p>Other window coordinate conventions exist for other rendering APIs. X11, GDI, and Direct3D version through DirectX 9 assume an upper-left window origin
			* and locate pixel centers at integer XY values. By this alternative convention, the XY location (0,0) corresponds to the upper-left-most pixel in a window.</p>
			*
			* <p>Direct3D for DirectX 10 assumes an upper-left origin (as do prior DirectX versions) yet assumes half-pixel coordinates (unlike prior DirectX versions).
			* By the DirectX 10 convention, the XY location (0.5,0.5) corresponds to the upper-left-most pixel in a window.</p>
			*
			* <p>Fragment shaders can directly access the location of a given processed fragment in window space. We call this location the "fragment coordinate".</p>
			*
			* <p>This extension provides a means for fragment shaders written in GLSL or OpenGL assembly extensions to specify alternative conventions for determining
			* the fragment coordinate value accessed during programmable fragment processing.</p>
			*
			* <p>The motivation for this extension is to provide an easy, efficient means for fragment shaders accessing a fragment's window-space location to adopt the
			* fragment coordinate convention for which the shader was originally written.</p>
			*
			* <p>Promoted to core in {@link GL32 OpenGL 3.2}.</p>
			*/
			const bool ARB_fragment_coord_conventions;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/fragment_layer_viewport.txt">ARB_fragment_layer_viewport</a> extension is supported.
			*
			* <p>The geometry shader has the special built-in variables gl_Layer and gl_ViewportIndex that specify which layer and viewport primitives are rendered to.
			* Currently the fragment shader does not know which layer or viewport the fragments are being written to without the application implementing their own
			* interface variables between the geometry and fragment shaders.</p>
			*
			* <p>This extension specifies that the gl_Layer and gl_ViewportIndex built-in variables are also available to the fragment shader so the application doesn't
			* need to implement these manually.</p>
			*
			* <p>Requires {@link GL30 OpenGL 3.0} and {@link #ARB_geometry_shader4 ARB_geometry_shader4}, or {@link GL32 OpenGL 3.2}. Promoted to core in {@link GL43 OpenGL 4.3}.</p>
			*/
			const bool ARB_fragment_layer_viewport;
			/** When true, {@link ARBFragmentProgram} is supported. */
			const bool ARB_fragment_program;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/fragment_program_shadow.txt">ARB_fragment_program_shadow</a> extension is supported.
			*
			* <p>This extension extends ARB_fragment_program to remove the interaction with ARB_shadow and defines the program option "ARB_fragment_program_shadow".</p>
			*
			* <p>Requires {@link #ARB_fragment_program ARB_fragment_program} and {@link #ARB_shadow ARB_shadow}.</p>
			*/
			const bool ARB_fragment_program_shadow;
			/** When true, {@link ARBFragmentShader} is supported. */
			const bool ARB_fragment_shader;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/fragment_shader_interlock.txt">ARB_fragment_shader_interlock</a> extension is supported.
			*
			* <p>In unextended OpenGL 4.5, applications may produce a large number of fragment shader invocations that perform loads and stores to memory using image
			* uniforms, atomic counter uniforms, buffer variables, or pointers. The order in which loads and stores to common addresses are performed by different
			* fragment shader invocations is largely undefined. For algorithms that use shader writes and touch the same pixels more than once, one or more of the
			* following techniques may be required to ensure proper execution ordering:</p>
			*
			* <ul>
			* <li>inserting Finish or WaitSync commands to drain the pipeline between different "passes" or "layers";</li>
			* <li>using only atomic memory operations to write to shader memory (which may be relatively slow and limits how memory may be updated); or</li>
			* <li>injecting spin loops into shaders to prevent multiple shader invocations from touching the same memory concurrently.</li>
			* </ul>
			*
			* <p>This extension provides new GLSL built-in functions beginInvocationInterlockARB() and endInvocationInterlockARB() that delimit a critical section of
			* fragment shader code. For pairs of shader invocations with "overlapping" coverage in a given pixel, the OpenGL implementation will guarantee that the
			* critical section of the fragment shader will be executed for only one fragment at a time.</p>
			*
			* <p>There are four different interlock modes supported by this extension, which are identified by layout qualifiers. The qualifiers
			* "pixel_interlock_ordered" and "pixel_interlock_unordered" provides mutual exclusion in the critical section for any pair of fragments corresponding to
			* the same pixel. When using multisampling, the qualifiers "sample_interlock_ordered" and "sample_interlock_unordered" only provide mutual exclusion for
			* pairs of fragments that both cover at least one common sample in the same pixel; these are recommended for performance if shaders use per-sample data
			* structures.</p>
			*
			* <p>Additionally, when the "pixel_interlock_ordered" or "sample_interlock_ordered" layout qualifier is used, the interlock also guarantees that the
			* critical section for multiple shader invocations with "overlapping" coverage will be executed in the order in which the primitives were processed by
			* the GL. Such a guarantee is useful for applications like blending in the fragment shader, where an application requires that fragment values to be
			* composited in the framebuffer in primitive order.</p>
			*
			* <p>This extension can be useful for algorithms that need to access per-pixel data structures via shader loads and stores. Such algorithms using this
			* extension can access such data structures in the critical section without worrying about other invocations for the same pixel accessing the data
			* structures concurrently. Additionally, the ordering guarantees are useful for cases where the API ordering of fragments is meaningful. For example,
			* applications may be able to execute programmable blending operations in the fragment shader, where the destination buffer is read via image loads and
			* the final value is written via image stores.</p>
			*
			* <p>Requires {@link GL42 OpenGL 4.2} or {@link ARBShaderImageLoadStore ARB_shader_image_load_store}.</p>
			*/
			const bool ARB_fragment_shader_interlock;
			/** When true, {@link ARBFramebufferNoAttachments} is supported. */
			const bool ARB_framebuffer_no_attachments;
			/** When true, {@link ARBFramebufferObject} is supported. */
			const bool ARB_framebuffer_object;
			/** When true, {@link ARBFramebufferSRGB} is supported. */
			const bool ARB_framebuffer_sRGB;
			/** When true, {@link ARBGeometryShader4} is supported. */
			const bool ARB_geometry_shader4;
			/** When true, {@link ARBGetProgramBinary} is supported. */
			const bool ARB_get_program_binary;
			/** When true, {@link ARBGetTextureSubImage} is supported. */
			const bool ARB_get_texture_sub_image;
			/** When true, {@link ARBGLSPIRV} is supported. */
			const bool ARB_gl_spirv;
			/** When true, {@link ARBGPUShader5} is supported. */
			const bool ARB_gpu_shader5;
			/** When true, {@link ARBGPUShaderFP64} is supported. */
			const bool ARB_gpu_shader_fp64;
			/** When true, {@link ARBGPUShaderInt64} is supported. */
			const bool ARB_gpu_shader_int64;
			/** When true, {@link ARBHalfFloatPixel} is supported. */
			const bool ARB_half_float_pixel;
			/** When true, {@link ARBHalfFloatVertex} is supported. */
			const bool ARB_half_float_vertex;
			/** When true, {@link ARBImaging} is supported. */
			const bool ARB_imaging;
			/** When true, {@link ARBIndirectParameters} is supported. */
			const bool ARB_indirect_parameters;
			/** When true, {@link ARBInstancedArrays} is supported. */
			const bool ARB_instanced_arrays;
			/** When true, {@link ARBInternalformatQuery} is supported. */
			const bool ARB_internalformat_query;
			/** When true, {@link ARBInternalformatQuery2} is supported. */
			const bool ARB_internalformat_query2;
			/** When true, {@link ARBInvalidateSubdata} is supported. */
			const bool ARB_invalidate_subdata;
			/** When true, {@link ARBMapBufferAlignment} is supported. */
			const bool ARB_map_buffer_alignment;
			/** When true, {@link ARBMapBufferRange} is supported. */
			const bool ARB_map_buffer_range;
			/** When true, {@link ARBMatrixPalette} is supported. */
			const bool ARB_matrix_palette;
			/** When true, {@link ARBMultiBind} is supported. */
			const bool ARB_multi_bind;
			/** When true, {@link ARBMultiDrawIndirect} is supported. */
			const bool ARB_multi_draw_indirect;
			/** When true, {@link ARBMultisample} is supported. */
			const bool ARB_multisample;
			/** When true, {@link ARBMultitexture} is supported. */
			const bool ARB_multitexture;
			/** When true, {@link ARBOcclusionQuery} is supported. */
			const bool ARB_occlusion_query;
			/** When true, {@link ARBOcclusionQuery2} is supported. */
			const bool ARB_occlusion_query2;
			/** When true, {@link ARBParallelShaderCompile} is supported. */
			const bool ARB_parallel_shader_compile;
			/** When true, {@link ARBPipelineStatisticsQuery} is supported. */
			const bool ARB_pipeline_statistics_query;
			/** When true, {@link ARBPixelBufferObject} is supported. */
			const bool ARB_pixel_buffer_object;
			/** When true, {@link ARBPointParameters} is supported. */
			const bool ARB_point_parameters;
			/** When true, {@link ARBPointSprite} is supported. */
			const bool ARB_point_sprite;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/post_depth_coverage.txt">ARB_post_depth_coverage</a> extension is supported.
			*
			* <p>This extension allows the fragment shader to control whether values in {@code gl_SampleMaskIn[]} reflect the coverage after application of the early
			* depth and stencil tests. This feature can be enabled with the following layout qualifier in the fragment shader:</p>
			*
			* <pre><code>        layout(post_depth_coverage) in;</code></pre>
			*
			* <p>Use of this feature implicitly enables early fragment tests.</p>
			*/
			const bool ARB_post_depth_coverage;
			/** When true, {@link ARBProgramInterfaceQuery} is supported. */
			const bool ARB_program_interface_query;
			/** When true, {@link ARBProvokingVertex} is supported. */
			const bool ARB_provoking_vertex;
			/** When true, {@link ARBQueryBufferObject} is supported. */
			const bool ARB_query_buffer_object;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/robust_buffer_access_behavior.txt">ARB_robust_buffer_access_behavior</a> extension is supported.
			*
			* <p>This extension specifies the behavior of out-of-bounds buffer and array accesses. This is an improvement over the existing ARB_robustness extension
			* which stated that the application should not crash, but the behavior is otherwise undefined. This extension specifies the access protection provided by
			* the GL to ensure that out-of-bounds accesses cannot read from or write to data not owned by the application. All accesses are contained within the
			* buffer object and program area they reference. These additional robustness guarantees apply to contexts created with the
			* {@code CONTEXT_FLAG_ROBUST_ACCESS_BIT_ARB} feature enabled.</p>
			*
			* <p>Requires {@link ARBRobustness ARB_robustness}. Promoted to core in {@link GL43 OpenGL 4.3}.</p>
			*/
			const bool ARB_robust_buffer_access_behavior;
			/** When true, {@link ARBRobustness} is supported. */
			const bool ARB_robustness;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/robustness_isolation.txt">ARB_robustness_isolation</a> extension is supported.
			*
			* <p>{@link ARBRobustness ARB_robustness} and supporting window system extensions allow creating an OpenGL context supporting graphics reset notification behavior. This
			* extension provides stronger guarantees about the possible side-effects of a graphics reset.</p>
			*
			* <p>It is expected that there may be a performance cost associated with isolating an application or share group from other contexts on the GPU. For this
			* reason, ARB_robustness_isolation is phrased as an opt-in mechanism, with a new context creation bit defined in the window system bindings. It is
			* expected that implementations might only advertise the strings in this extension if both the implementation supports the desired isolation properties,
			* and the context was created with the appropriate reset isolation bit.</p>
			*
			* <p>Requires {@link ARBRobustness ARB_robustness}. Promoted to core in {@link GL43 OpenGL 4.3}.</p>
			*/
			const bool ARB_robustness_isolation;
			/** When true, {@link ARBSampleLocations} is supported. */
			const bool ARB_sample_locations;
			/** When true, {@link ARBSampleShading} is supported. */
			const bool ARB_sample_shading;
			/** When true, {@link ARBSamplerObjects} is supported. */
			const bool ARB_sampler_objects;
			/** When true, {@link ARBSeamlessCubeMap} is supported. */
			const bool ARB_seamless_cube_map;
			/** When true, {@link ARBSeamlessCubemapPerTexture} is supported. */
			const bool ARB_seamless_cubemap_per_texture;
			/** When true, {@link ARBSeparateShaderObjects} is supported. */
			const bool ARB_separate_shader_objects;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_atomic_counter_ops.txt">ARB_shader_atomic_counter_ops</a> extension is supported.
			*
			* <p>The {@link ARBShaderAtomicCounters ARB_shader_atomic_counters} extension introduced atomic counters, but it limits list of potential operations that can be performed on them
			* to increment, decrement, and query. This extension extends the list of GLSL built-in functions that can operate on atomic counters. The list of new
			* operations include:</p>
			*
			* <ul>
			* <li>Addition and subtraction</li>
			* <li>Minimum and maximum</li>
			* <li>Bitwise operators (AND, OR, XOR, etc.)</li>
			* <li>Exchange, and compare and exchange operators</li>
			* </ul>
			*
			* <p>Requires {@link GL42 OpenGL 4.2} or {@link ARBShaderAtomicCounters ARB_shader_atomic_counters}.</p>
			*/
			const bool ARB_shader_atomic_counter_ops;
			/** When true, {@link ARBShaderAtomicCounters} is supported. */
			const bool ARB_shader_atomic_counters;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_ballot.txt">ARB_shader_ballot</a> extension is supported.
			*
			* <p>This extension provides the ability for a group of invocations which execute in lockstep to do limited forms of cross-invocation communication via a
			* group broadcast of a invocation value, or broadcast of a bitarray representing a predicate value from each invocation in the group.</p>
			*
			* <p>Requires {@link ARBGPUShaderInt64 ARB_gpu_shader_int64}.</p>
			*/
			const bool ARB_shader_ballot;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_bit_encoding.txt">ARB_shader_bit_encoding</a> extension is supported.
			*
			* <p>This extension trivially adds built-in functions for getting/setting the bit encoding for floating-point values in the OpenGL Shading Language.</p>
			*
			* <p>Promoted to core in {@link GL33 OpenGL 3.3}.</p>
			*/
			const bool ARB_shader_bit_encoding;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_clock.txt">ARB_shader_clock</a> extension is supported.
			*
			* <p>This extension exposes a 64-bit monotonically incrementing shader counter which may be used to derive local timing information within a single shader
			* invocation.</p>
			*/
			const bool ARB_shader_clock;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_draw_parameters.txt">ARB_shader_draw_parameters</a> extension is supported.
			*
			* <p>In unextended GL, vertex shaders have inputs named {@code gl_VertexID} and {@code gl_InstanceID}, which contain, respectively the index of the vertex
			* and instance. The value of {@code gl_VertexID} is the implicitly passed index of the vertex being processed, which includes the value of baseVertex, for
			* those commands that accept it. Meanwhile, {@code gl_InstanceID} is the integer index of the current instance being processed, but, even for commands
			* that accept a baseInstance parameter, it does not include the value of this argument. Furthermore, the equivalents to these variables in other graphics
			* APIs do not necessarily follow these conventions. The reason for this inconsistency is that there are legitimate use cases for both inclusion and
			* exclusion of the baseVertex or baseInstance parameters in {@code gl_VertexID} and {@code gl_InstanceID}, respectively.</p>
			*
			* <p>Rather than change the semantics of either built-in variable, this extension adds two new built-in variables to the GL shading language,
			* {@code gl_BaseVertexARB} and {@code gl_BaseInstanceARB}, which contain the values passed in the baseVertex and baseInstance parameters, respectively.
			* Shaders provided by the application may use these variables to offset {@code gl_VertexID} or {@code gl_InstanceID} if desired, or use them for any other
			* purpose.</p>
			*
			* <p>Additionally, this extension adds a further built-in variable, {@code gl_DrawID} to the shading language. This variable contains the index of the draw
			* currently being processed by a Multi* variant of a drawing command (such as {@link GL14#glMultiDrawElements MultiDrawElements} or {@link GL43#glMultiDrawArraysIndirect MultiDrawArraysIndirect}).</p>
			*
			* <p>Requires {@link GL31 OpenGL 3.1}. Promoted to core in {@link GL33 OpenGL 3.3}.</p>
			*/
			const bool ARB_shader_draw_parameters;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_group_vote.txt">ARB_shader_group_vote</a> extension is supported.
			*
			* <p>This extension provides new built-in functions to compute the composite of a set of boolean conditions across a group of shader invocations. These
			* composite results may be used to execute shaders more efficiently on a single-instruction multiple-data (SIMD) processor. The set of shader invocations
			* across which boolean conditions are evaluated is implementation-dependent, and this extension provides no guarantee over how individual shader
			* invocations are assigned to such sets. In particular, the set of shader invocations has no necessary relationship with the compute shader local work
			* group -- a pair of shader invocations in a single compute shader work group may end up in different sets used by these built-ins.</p>
			*
			* <p>Compute shaders operate on an explicitly specified group of threads (a local work group), but many implementations of OpenGL 4.3 will even group
			* non-compute shader invocations and execute them in a SIMD fashion. When executing code like</p>
			*
			* <pre><code>if (condition) {
			result = do_fast_path();
			} else {
			result = do_general_path();
			}</code></pre>
			*
			* <p>where {@code condition} diverges between invocations, a SIMD implementation might first call do_fast_path() for the invocations where {@code condition}
			* is true and leave the other invocations dormant. Once do_fast_path() returns, it might call do_general_path() for invocations where {@code condition} is
			* false and leave the other invocations dormant. In this case, the shader executes *both* the fast and the general path and might be better off just using
			* the general path for all invocations.</p>
			*
			* <p>This extension provides the ability to avoid divergent execution by evaluting a condition across an entire SIMD invocation group using code like:</p>
			*
			* <pre><code>if (allInvocationsARB(condition)) {
			result = do_fast_path();
			} else {
			result = do_general_path();
			}</code></pre>
			*
			* <p>The built-in function allInvocationsARB() will return the same value for all invocations in the group, so the group will either execute do_fast_path()
			* or do_general_path(), but never both. For example, shader code might want to evaluate a complex function iteratively by starting with an approximation
			* of the result and then refining the approximation. Some input values may require a small number of iterations to generate an accurate result
			* (do_fast_path) while others require a larger number (do_general_path). In another example, shader code might want to evaluate a complex function
			* (do_general_path) that can be greatly simplified when assuming a specific value for one of its inputs (do_fast_path).</p>
			*
			* <p>Requires {@link GL43 OpenGL 4.3} or {@link ARBComputeShader ARB_compute_shader}.</p>
			*/
			const bool ARB_shader_group_vote;
			/** When true, {@link ARBShaderImageLoadStore} is supported. */
			const bool ARB_shader_image_load_store;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_image_size.txt">ARB_shader_image_size</a> extension is supported.
			*
			* <p>This extension provides GLSL built-in functions allowing shaders to query the size of an image.</p>
			*
			* <p>Requires {@link GL42 OpenGL 4.2} and GLSL 4.20. Promoted to core in {@link GL43 OpenGL 4.3}.</p>
			*/
			const bool ARB_shader_image_size;
			/** When true, {@link ARBShaderObjects} is supported. */
			const bool ARB_shader_objects;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_precision.txt">ARB_shader_precision</a> extension is supported.
			*
			* <p>This extension more clearly restricts the precision requirements of implementations of the GLSL specification. These include precision of arithmetic
			* operations (operators '+', '/', ...), transcendentals (log, exp, pow, reciprocal sqrt, ...), when NaNs (not a number) and INFs (infinities) will be
			* supported and generated, and denorm flushing behavior.  Trigonometric built-ins and some other categories of built-ins are not addressed.</p>
			*
			* <p>Requires {@link GL40 OpenGL 4.0}. Promoted to core in {@link GL41 OpenGL 4.1}.</p>
			*/
			const bool ARB_shader_precision;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_stencil_export.txt">ARB_shader_stencil_export</a> extension is supported.
			*
			* <p>In OpenGL, the stencil test is a powerful mechanism to selectively discard fragments based on the content of the stencil buffer. However, facilites to
			* update the content of the stencil buffer are limited to operations such as incrementing the existing value, or overwriting with a fixed reference value.</p>
			*
			* <p>This extension provides a mechanism whereby a shader may generate the stencil reference value per invocation. When stencil testing is enabled, this
			* allows the test to be performed against the value generated in the shader. When the stencil operation is set to {@link GL11#REPLACE REPLACE}, this allows a value generated
			* in the shader to be written to the stencil buffer directly.</p>
			*
			* <p>Requires {@link #ARB_fragment_shader ARB_fragment_shader}.</p>
			*/
			const bool ARB_shader_stencil_export;
			/** When true, {@link ARBShaderStorageBufferObject} is supported. */
			const bool ARB_shader_storage_buffer_object;
			/** When true, {@link ARBShaderSubroutine} is supported. */
			const bool ARB_shader_subroutine;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_texture_image_samples.txt">ARB_shader_texture_image_samples</a> extension is supported.
			*
			* <p>This extension provides GLSL built-in functions allowing shaders to query the number of samples of a texture.</p>
			*
			* <p>Requires GLSL 1.50 or {@link ARBTextureMultisample ARB_texture_multisample}.</p>
			*/
			const bool ARB_shader_texture_image_samples;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_texture_lod.txt">ARB_shader_texture_lod</a> extension is supported.
			*
			* <p>This extension adds additional texture functions to the OpenGL Shading Language which provide the shader writer with explicit control of LOD.</p>
			*
			* <p>Mipmap texture fetches and anisotropic texture fetches require an implicit derivatives to calculate rho, lambda and/or the line of anisotropy. These
			* implicit derivatives will be undefined for texture fetches occurring inside non-uniform control flow or for vertex shader texture fetches, resulting in
			* undefined texels.</p>
			*
			* <p>The additional texture functions introduced with this extension provide explict control of LOD (isotropic texture functions) or provide explicit
			* derivatives (anisotropic texture functions).</p>
			*
			* <p>Anisotropic texture functions return defined texels for mipmap texture fetches or anisotropic texture fetches, even inside non-uniform control flow.
			* Isotropic texture functions return defined texels for mipmap texture fetches, even inside non-uniform control flow. However, isotropic texture functions
			* return undefined texels for anisotropic texture fetches.</p>
			*
			* <p>The existing isotropic vertex texture functions:</p>
			*
			* <pre><code>texture1DLod,   texture1DProjLod,
			texture2DLod,   texture2DProjLod,
			texture3DLod,   texture3DProjLod,
			textureCubeLod,
			shadow1DLod,    shadow1DProjLod,
			shadow2DLod,    shadow2DProjLod</code></pre>
			*
			* <p>are added to the built-in functions for fragment shaders.</p>
			*
			* <p>New anisotropic texture functions, providing explicit derivatives:</p>
			*
			* <pre><code>texture1DGradARB(
			sampler1D sampler,
			float P, float dPdx, float dPdy);
			texture1DProjGradARB(
			sampler1D sampler,
			vec2 P, float dPdx, float dPdy);
			texture1DProjGradARB(
			sampler1D sampler,
			vec4 P, float dPdx, float dPdy);
			texture2DGradARB(
			sampler2D sampler,
			vec2 P, vec2 dPdx, vec2 dPdy);
			texture2DProjGradARB(
			sampler2D sampler,
			vec3 P, vec2 dPdx, vec2 dPdy);
			texture2DProjGradARB(
			sampler2D sampler,
			vec4 P, vec2 dPdx, vec2 dPdy);
			texture3DGradARB(
			sampler3D sampler,
			vec3 P, vec3 dPdx, vec3 dPdy);
			texture3DProjGradARB(
			sampler3D sampler,
			vec4 P, vec3 dPdx, vec3 dPdy);
			textureCubeGradARB(
			samplerCube sampler,
			vec3 P, vec3 dPdx, vec3 dPdy);

			shadow1DGradARB(
			sampler1DShadow sampler,
			vec3 P, float dPdx, float dPdy);
			shadow1DProjGradARB(
			sampler1DShadow sampler,
			vec4 P, float dPdx, float dPdy);
			shadow2DGradARB(
			sampler2DShadow sampler,
			vec3 P, vec2 dPdx, vec2 dPdy);
			shadow2DProjGradARB(
			sampler2DShadow sampler,
			vec4 P, vec2 dPdx, vec2 dPdy);

			texture2DRectGradARB(
			sampler2DRect sampler,
			vec2 P, vec2 dPdx, vec2 dPdy);
			texture2DRectProjGradARB(
			sampler2DRect sampler,
			vec3 P, vec2 dPdx, vec2 dPdy);
			texture2DRectProjGradARB(
			sampler2DRect sampler,
			vec4 P, vec2 dPdx, vec2 dPdy);

			shadow2DRectGradARB(
			sampler2DRectShadow sampler,
			vec3 P, vec2 dPdx, vec2 dPdy);
			shadow2DRectProjGradARB(
			sampler2DRectShadow sampler,
			vec4 P, vec2 dPdx, vec2 dPdy);</code></pre>
			*
			* <p>are added to the built-in functions for vertex shaders and fragment shaders.</p>
			*
			* <p>Requires {@link #ARB_shader_objects ARB_shader_objects}. Promoted to core in {@link GL30 OpenGL 3.0}.</p>
			*/
			const bool ARB_shader_texture_lod;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shader_viewport_layer_array.txt">ARB_shader_viewport_layer_array</a> extension is supported.
			*
			* <p>The gl_ViewportIndex and gl_Layer built-in variables were introduced by the in OpenGL 4.1. These variables are available in un-extended OpenGL only to
			* the geometry shader. When written in the geometry shader, they cause geometry to be directed to one of an array of several independent viewport
			* rectangles or framebuffer attachment layers, respectively.</p>
			*
			* <p>In order to use any viewport or attachment layer other than zero, a geometry shader must be present. Geometry shaders introduce processing overhead and
			* potential performance issues. The AMD_vertex_shader_layer and AMD_vertex_shader_viewport_index extensions allowed the gl_Layer and gl_ViewportIndex
			* outputs to be written directly from the vertex shader with no geometry shader present.</p>
			*
			* <p>This extension effectively merges the AMD_vertex_shader_layer and AMD_vertex_shader_viewport_index extensions together and extends them further to
			* allow both outputs to be written from tessellation evaluation shaders.</p>
			*
			* <p>Requires {@link GL41 OpenGL 4.1}.</p>
			*/
			const bool ARB_shader_viewport_layer_array;
			/** When true, {@link ARBShadingLanguage100} is supported. */
			const bool ARB_shading_language_100;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shading_language_420pack.txt">ARB_shading_language_420pack</a> extension is supported.
			*
			* <p>This is a language feature only extension formed from changes made to version 4.20 of GLSL. It includes:</p>
			*
			* <ul>
			* <li>Add line-continuation using '', as in C++.</li>
			* <li>Change from ASCII to UTF-8 for the language character set and also allow any characters inside comments.</li>
			* <li>Allow implicit conversions of return values to the declared type of the function.</li>
			* <li>The *const* keyword can be used to declare variables within a function body with initializer expressions that are not constant expressions.</li>
			* <li>Qualifiers on variable declarations no longer have to follow a strict order. The layout qualifier can be used multiple times, and multiple parameter
			* qualifiers can be used. However, this is not as straightforward as saying declarations have arbitrary lists of initializers. Typically, one
			* qualifier from each class of qualifiers is allowed, so care is now taken to classify them and say so. Then, of these, order restrictions are removed.</li>
			* <li>Add layout qualifier identifier "binding" to bind the location of a uniform block. This requires version 1.4 of GLSL. If this extension is used with
			* an earlier version than 1.4, this feature is not present.</li>
			* <li>Add layout qualifier identifier "binding" to bind units to sampler and image variable declarations.</li>
			* <li>Add C-style curly brace initializer lists syntax for initializers. Full initialization of aggregates is required when these are used.</li>
			* <li>Allow ".length()" to be applied to vectors and matrices, returning the number of components or columns.</li>
			* <li>Allow swizzle operations on scalars.</li>
			* <li>Built-in constants for {@code gl_MinProgramTexelOffset} and {@code gl_MaxProgramTexelOffset}.</li>
			* </ul>
			*
			* <p>Requires GLSL 1.30. Requires GLSL 1.40 for uniform block bindings. Promoted to core in {@link GL42 OpenGL 4.2}.</p>
			*/
			const bool ARB_shading_language_420pack;
			/** When true, {@link ARBShadingLanguageInclude} is supported. */
			const bool ARB_shading_language_include;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/shading_language_packing.txt">ARB_shading_language_packing</a> extension is supported.
			*
			* <p>This extension provides the GLSL built-in functions to convert a 32-bit unsigned integer holding a pair of 16-bit floating-point values to or from a
			* two-component floating-point vector (vec2).</p>
			*
			* <p>This mechanism allows GLSL shaders to read and write 16-bit floating-point encodings (via 32-bit unsigned integers) without introducing a full set of
			* 16-bit floating-point data types.</p>
			*
			* <p>This extension also adds the GLSL built-in packing functions included in GLSL version 4.00 and the ARB_gpu_shader5 extension which pack and unpack
			* vectors of small fixed-point data types into a larger scalar. By putting these packing functions in this separate extension it allows implementations to
			* provide these functions in hardware that supports them independent of the other {@link #ARB_gpu_shader5 ARB_gpu_shader5} features.</p>
			*
			* <p>In addition to the packing functions from ARB_gpu_shader5 this extension also adds the missing {@code [un]packSnorm2x16} for completeness.</p>
			*
			* <p>Promoted to core in {@link GL42 OpenGL 4.2}.</p>
			*/
			const bool ARB_shading_language_packing;
			/** When true, {@link ARBShadow} is supported. */
			const bool ARB_shadow;
			/** When true, {@link ARBShadowAmbient} is supported. */
			const bool ARB_shadow_ambient;
			/** When true, {@link ARBSparseBuffer} is supported. */
			const bool ARB_sparse_buffer;
			/** When true, {@link ARBSparseTexture} is supported. */
			const bool ARB_sparse_texture;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/sparse_texture2.txt">ARB_sparse_texture2</a> extension is supported.
			*
			* <p>This extension builds on the {@link ARBSparseTexture ARB_sparse_texture} extension, providing the following new functionality:</p>
			*
			* <ul>
			* <li>New built-in GLSL texture lookup and image load functions are provided that return information on whether the texels accessed for the texture
			* lookup accessed uncommitted texture memory.</li>
			* <li>New built-in GLSL texture lookup functions are provided that specify a minimum level of detail to use for lookups where the level of detail is
			* computed automatically. This allows shaders to avoid accessing unpopulated portions of high-resolution levels of detail when it knows that the
			* memory accessed is unpopulated, either from a priori knowledge or from feedback provided by the return value of previously executed "sparse"
			* texture lookup functions.</li>
			* <li>Reads of uncommitted texture memory will act as though such memory were filled with zeroes; previously, the values returned by reads were
			* undefined.</li>
			* <li>Standard implementation-independent virtual page sizes for internal formats required to be supported with sparse textures. These standard sizes can
			* be requested by leaving {@link ARBSparseTexture#VIRTUAL_PAGE_SIZE_INDEX_ARB VIRTUAL_PAGE_SIZE_INDEX_ARB} at its initial value (0).</li>
			* <li>Support for creating sparse multisample and multisample array textures is added. However, the virtual page sizes for such textures remain fully
			* implementation-dependent.</li>
			* </ul>
			*
			* <p>Requires {@link ARBSparseTexture ARB_sparse_texture}</p>
			*/
			const bool ARB_sparse_texture2;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/sparse_texture_clamp.txt">ARB_sparse_texture_clamp</a> extension is supported.
			*
			* <p>This extension builds on the {@link #ARB_sparse_texture2 ARB_sparse_texture2} extension, providing the following new functionality:</p>
			*
			* <p>New built-in GLSL texture lookup functions are provided that specify a minimum level of detail to use for lookups where the level of detail is
			* computed automatically. This allows shaders to avoid accessing unpopulated portions of high-resolution levels of detail when it knows that the memory
			* accessed is unpopulated, either from a priori knowledge or from feedback provided by the return value of previously executed "sparse" texture lookup
			* functions.</p>
			*
			* <p>Requires {@link #ARB_sparse_texture2 ARB_sparse_texture2}</p>
			*/
			const bool ARB_sparse_texture_clamp;
			/** When true, {@link ARBStencilTexturing} is supported. */
			const bool ARB_stencil_texturing;
			/** When true, {@link ARBSync} is supported. */
			const bool ARB_sync;
			/** When true, {@link ARBTessellationShader} is supported. */
			const bool ARB_tessellation_shader;
			/** When true, {@link ARBTextureBarrier} is supported. */
			const bool ARB_texture_barrier;
			/** When true, {@link ARBTextureBorderClamp} is supported. */
			const bool ARB_texture_border_clamp;
			/** When true, {@link ARBTextureBufferObject} is supported. */
			const bool ARB_texture_buffer_object;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/texture_buffer_object_rgb32.txt">ARB_texture_buffer_object_rgb32</a> extension is supported.
			*
			* <p>This extension adds three new buffer texture formats - RGB32F, RGB32I, and RGB32UI. This partially addresses one of the limitations of buffer textures
			* in the original {@link #EXT_texture_buffer_object EXT_texture_buffer_object} extension and in {@link GL31 OpenGL 3.1}, which provide no support for three-component formats.</p>
			*
			* <p>Promoted to core in {@link GL40 OpenGL 4.0}.</p>
			*/
			const bool ARB_texture_buffer_object_rgb32;
			/** When true, {@link ARBTextureBufferRange} is supported. */
			const bool ARB_texture_buffer_range;
			/** When true, {@link ARBTextureCompression} is supported. */
			const bool ARB_texture_compression;
			/** When true, {@link ARBTextureCompressionBPTC} is supported. */
			const bool ARB_texture_compression_bptc;
			/** When true, {@link ARBTextureCompressionRGTC} is supported. */
			const bool ARB_texture_compression_rgtc;
			/** When true, {@link ARBTextureCubeMap} is supported. */
			const bool ARB_texture_cube_map;
			/** When true, {@link ARBTextureCubeMapArray} is supported. */
			const bool ARB_texture_cube_map_array;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/texture_env_add.txt">ARB_texture_env_add</a> extension is supported.
			*
			* <p>This extension adds a new texture environment function: ADD.</p>
			*
			* <p>Promoted to core in {@link GL13 OpenGL 1.3}.</p>
			*/
			const bool ARB_texture_env_add;
			/** When true, {@link ARBTextureEnvCombine} is supported. */
			const bool ARB_texture_env_combine;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/texture_env_crossbar.txt">ARB_texture_env_crossbar</a> extension is supported.
			*
			* <p>This extension adds the capability to use the texture color from other texture units as sources to the {@link ARBTextureEnvCombine#COMBINE_ARB COMBINE_ARB} environment
			* function. The {@link ARBTextureEnvCombine ARB_texture_env_combine} extension defined texture environment functions which could use the color from the current texture unit
			* as a source. This extension adds the ability to use the color from any texture unit as a source.</p>
			*
			* <p>Requires {@link #ARB_multitexture ARB_multitexture} and {@link ARBTextureEnvCombine ARB_texture_env_combine}. Promoted to core in {@link GL14 OpenGL 1.4}.</p>
			*/
			const bool ARB_texture_env_crossbar;
			/** When true, {@link ARBTextureEnvDot3} is supported. */
			const bool ARB_texture_env_dot3;
			/** When true, {@link ARBTextureFilterMinmax} is supported. */
			const bool ARB_texture_filter_minmax;
			/** When true, {@link ARBTextureFloat} is supported. */
			const bool ARB_texture_float;
			/** When true, {@link ARBTextureGather} is supported. */
			const bool ARB_texture_gather;
			/** When true, {@link ARBTextureMirrorClampToEdge} is supported. */
			const bool ARB_texture_mirror_clamp_to_edge;
			/** When true, {@link ARBTextureMirroredRepeat} is supported. */
			const bool ARB_texture_mirrored_repeat;
			/** When true, {@link ARBTextureMultisample} is supported. */
			const bool ARB_texture_multisample;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/texture_non_power_of_two.txt">ARB_texture_non_power_of_two</a> extension is supported.
			*
			* <p>Conventional OpenGL texturing is limited to images with power-of-two dimensions and an optional 1-texel border. This extension relaxes the size
			* restrictions for the 1D, 2D, cube map, and 3D texture targets.</p>
			*
			* <p>Promoted to core in {@link GL20 OpenGL 2.0}.</p>
			*/
			const bool ARB_texture_non_power_of_two;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/texture_query_levels.txt">ARB_texture_query_levels</a> extension is supported.
			*
			* <p>This extension provides a new set of texture functions ({@code textureQueryLevels}) in the OpenGL Shading Language that exposes the number of accessible
			* mipmap levels in the texture associated with a GLSL sampler variable. The set of accessible levels includes all the levels of the texture defined either
			* through TexImage*, TexStorage*, or TextureView* ({@link ARBTextureView ARB_texture_view}) APIs that are not below the {@link GL12#TEXTURE_BASE_LEVEL TEXTURE_BASE_LEVEL} or above the
			* {@link GL12#TEXTURE_MAX_LEVEL TEXTURE_MAX_LEVEL} parameters. For textures defined with TexImage*, the set of resident levels is somewhat implementation-dependent. For fully
			* defined results, applications should use TexStorage*&#47;TextureView unless the texture has a full mipmap chain and is used with a mipmapped minification
			* filter.</p>
			*
			* <p>These functions means that shaders are not required to manually recompute, approximate, or maintain a uniform holding a pre-computed level count, since
			* the true level count is already available to the implementation. This value can be used to avoid black or leaking pixel artifacts for rendering methods
			* which are using texture images as memory pages (eg: virtual textures); methods that can't only rely on the fixed pipeline texture functions which take
			* advantage of {@link GL12#TEXTURE_MAX_LEVEL TEXTURE_MAX_LEVEL} for their sampling.</p>
			*
			* <p>Requires {@link GL30 OpenGL 3.0} and GLSL 1.30. Promoted to core in {@link GL43 OpenGL 4.3}.</p>
			*/
			const bool ARB_texture_query_levels;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/texture_query_lod.txt">ARB_texture_query_lod</a> extension is supported.
			*
			* <p>This extension provides a new set of fragment shader texture functions ({@code textureLOD}) that return the results of automatic level-of-detail
			* computations that would be performed if a texture lookup were performed.</p>
			*
			* <p>Requires {@link GL20 OpenGL 2.0}, {@link #EXT_gpu_shader4 EXT_gpu_shader4}, {@link #EXT_texture_array EXT_texture_array} and GLSL 1.30. Promoted to core in {@link GL40 OpenGL 4.0}.</p>
			*/
			const bool ARB_texture_query_lod;
			/** When true, {@link ARBTextureRectangle} is supported. */
			const bool ARB_texture_rectangle;
			/** When true, {@link ARBTextureRG} is supported. */
			const bool ARB_texture_rg;
			/** When true, {@link ARBTextureRGB10_A2UI} is supported. */
			const bool ARB_texture_rgb10_a2ui;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/texture_stencil8.txt">ARB_texture_stencil8</a> extension is supported.
			*
			* <p>This extension accepts {@link GL30#STENCIL_INDEX8 STENCIL_INDEX8} as a texture internal format, and adds STENCIL_INDEX8 to the required internal format list. This removes the
			* need to use renderbuffers if a stencil-only format is desired.</p>
			*
			* <p>Promoted to core in {@link GL44 OpenGL 4.4}.</p>
			*/
			const bool ARB_texture_stencil8;
			/** When true, {@link ARBTextureStorage} is supported. */
			const bool ARB_texture_storage;
			/** When true, {@link ARBTextureStorageMultisample} is supported. */
			const bool ARB_texture_storage_multisample;
			/** When true, {@link ARBTextureSwizzle} is supported. */
			const bool ARB_texture_swizzle;
			/** When true, {@link ARBTextureView} is supported. */
			const bool ARB_texture_view;
			/** When true, {@link ARBTimerQuery} is supported. */
			const bool ARB_timer_query;
			/** When true, {@link ARBTransformFeedback2} is supported. */
			const bool ARB_transform_feedback2;
			/** When true, {@link ARBTransformFeedback3} is supported. */
			const bool ARB_transform_feedback3;
			/** When true, {@link ARBTransformFeedbackInstanced} is supported. */
			const bool ARB_transform_feedback_instanced;
			/** When true, {@link ARBTransformFeedbackOverflowQuery} is supported. */
			const bool ARB_transform_feedback_overflow_query;
			/** When true, {@link ARBTransposeMatrix} is supported. */
			const bool ARB_transpose_matrix;
			/** When true, {@link ARBUniformBufferObject} is supported. */
			const bool ARB_uniform_buffer_object;
			/** When true, {@link ARBVertexArrayBGRA} is supported. */
			const bool ARB_vertex_array_bgra;
			/** When true, {@link ARBVertexArrayObject} is supported. */
			const bool ARB_vertex_array_object;
			/** When true, {@link ARBVertexAttrib64Bit} is supported. */
			const bool ARB_vertex_attrib_64bit;
			/** When true, {@link ARBVertexAttribBinding} is supported. */
			const bool ARB_vertex_attrib_binding;
			/** When true, {@link ARBVertexBlend} is supported. */
			const bool ARB_vertex_blend;
			/** When true, {@link ARBVertexBufferObject} is supported. */
			const bool ARB_vertex_buffer_object;
			/** When true, {@link ARBVertexProgram} is supported. */
			const bool ARB_vertex_program;
			/** When true, {@link ARBVertexShader} is supported. */
			const bool ARB_vertex_shader;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/ARB/vertex_type_10f_11f_11f_rev.txt">ARB_vertex_type_10f_11f_11f_rev</a> extension is supported.
			*
			* <p>This extension a new vertex attribute data format: a packed 11.11.10 unsigned float vertex data format. This vertex data format can be used to describe
			* a compressed 3 component stream of values that can be represented by 10- or 11-bit unsigned floating point values.</p>
			*
			* <p>The {@link GL30#UNSIGNED_INT_10F_11F_11F_REV UNSIGNED_INT_10F_11F_11F_REV} vertex attribute type is equivalent to the {@link GL30#R11F_G11F_B10F R11F_G11F_B10F} texture internal format.</p>
			*
			* <p>Requires {@link GL30 OpenGL 3.0} and {@link ARBVertexType2_10_10_10_REV ARB_vertex_type_2_10_10_10_rev}. Promoted to core in {@link GL44 OpenGL 4.4}.</p>
			*/
			const bool ARB_vertex_type_10f_11f_11f_rev;
			/** When true, {@link ARBVertexType2_10_10_10_REV} is supported. */
			const bool ARB_vertex_type_2_10_10_10_rev;
			/** When true, {@link ARBViewportArray} is supported. */
			const bool ARB_viewport_array;
			/** When true, {@link ARBWindowPos} is supported. */
			const bool ARB_window_pos;
			/** When true, {@link ATIMeminfo} is supported. */
			const bool ATI_meminfo;
			/** When true, the <a href="http://www.opengl.org/registry/specs/ATI/shader_texture_lod.txt">ATI_shader_texture_lod</a> extension is supported. */
			const bool ATI_shader_texture_lod;
			/** When true, {@link ATITextureCompression3DC} is supported. */
			const bool ATI_texture_compression_3dc;
			/** When true, {@link EXT422Pixels} is supported. */
			const bool EXT_422_pixels;
			/** When true, {@link EXTABGR} is supported. */
			const bool EXT_abgr;
			/** When true, {@link EXTBGRA} is supported. */
			const bool EXT_bgra;
			/** When true, {@link EXTBindableUniform} is supported. */
			const bool EXT_bindable_uniform;
			/** When true, {@link EXTBlendColor} is supported. */
			const bool EXT_blend_color;
			/** When true, {@link EXTBlendEquationSeparate} is supported. */
			const bool EXT_blend_equation_separate;
			/** When true, {@link EXTBlendFuncSeparate} is supported. */
			const bool EXT_blend_func_separate;
			/** When true, {@link EXTBlendMinmax} is supported. */
			const bool EXT_blend_minmax;
			/** When true, {@link EXTBlendSubtract} is supported. */
			const bool EXT_blend_subtract;
			/** When true, {@link EXTClipVolumeHint} is supported. */
			const bool EXT_clip_volume_hint;
			/** When true, {@link EXTCompiledVertexArray} is supported. */
			const bool EXT_compiled_vertex_array;
			/** When true, {@link EXTDebugLabel} is supported. */
			const bool EXT_debug_label;
			/** When true, {@link EXTDebugMarker} is supported. */
			const bool EXT_debug_marker;
			/** When true, {@link EXTDepthBoundsTest} is supported. */
			const bool EXT_depth_bounds_test;
			/** When true, {@link EXTDirectStateAccess} is supported. */
			const bool EXT_direct_state_access;
			/** When true, {@link EXTDrawBuffers2} is supported. */
			const bool EXT_draw_buffers2;
			/** When true, {@link EXTDrawInstanced} is supported. */
			const bool EXT_draw_instanced;
			/** When true, {@link EXTFramebufferBlit} is supported. */
			const bool EXT_framebuffer_blit;
			/** When true, {@link EXTFramebufferMultisample} is supported. */
			const bool EXT_framebuffer_multisample;
			/** When true, {@link EXTFramebufferMultisampleBlitScaled} is supported. */
			const bool EXT_framebuffer_multisample_blit_scaled;
			/** When true, {@link EXTFramebufferObject} is supported. */
			const bool EXT_framebuffer_object;
			/** When true, {@link EXTFramebufferSRGB} is supported. */
			const bool EXT_framebuffer_sRGB;
			/** When true, {@link EXTGeometryShader4} is supported. */
			const bool EXT_geometry_shader4;
			/** When true, {@link EXTGPUProgramParameters} is supported. */
			const bool EXT_gpu_program_parameters;
			/** When true, {@link EXTGPUShader4} is supported. */
			const bool EXT_gpu_shader4;
			/** When true, {@link EXTPackedDepthStencil} is supported. */
			const bool EXT_packed_depth_stencil;
			/** When true, {@link EXTPackedFloat} is supported. */
			const bool EXT_packed_float;
			/** When true, {@link EXTPixelBufferObject} is supported. */
			const bool EXT_pixel_buffer_object;
			/** When true, {@link EXTPointParameters} is supported. */
			const bool EXT_point_parameters;
			/** When true, {@link EXTPolygonOffsetClamp} is supported. */
			const bool EXT_polygon_offset_clamp;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/EXT/post_depth_coverage.txt">EXT_post_depth_coverage</a> extension is supported.
			*
			* <p>This extension allows the fragment shader to control whether values in {@code gl_SampleMaskIn[]} reflect the coverage after application of the early
			* depth and stencil tests.  This feature can be enabled with the following layout qualifier in the fragment shader:</p>
			*
			* <pre><code>layout(post_depth_coverage) in;</code></pre>
			*
			* <p>To use this feature, early fragment tests must also be enabled in the fragment shader via:</p>
			*
			* <pre><code>layout(early_fragment_tests) in;</code></pre>
			*/
			const bool EXT_post_depth_coverage;
			/** When true, {@link EXTProvokingVertex} is supported. */
			const bool EXT_provoking_vertex;
			/** When true, {@link EXTRasterMultisample} is supported. */
			const bool EXT_raster_multisample;
			/** When true, {@link EXTSecondaryColor} is supported. */
			const bool EXT_secondary_color;
			/** When true, {@link EXTSeparateShaderObjects} is supported. */
			const bool EXT_separate_shader_objects;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/EXT/shader_image_load_formatted.txt">EXT_shader_image_load_formatted</a> extension is supported.
			*
			* <p>{@link ARBShaderImageLoadStore ARB_shader_image_load_store} (and OpenGL 4.2) added support for random access load and store from/to texture images, but due to hardware
			* limitations, loads were required to declare the image format in the shader source. This extension relaxes that requirement, and the return values from
			* {@code imageLoad} can be format-converted based on the format of the image binding.</p>
			*/
			const bool EXT_shader_image_load_formatted;
			/** When true, {@link EXTShaderImageLoadStore} is supported. */
			const bool EXT_shader_image_load_store;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/EXT/shader_integer_mix.txt">EXT_shader_integer_mix</a> extension is supported.
			*
			* <p>GLSL 1.30 (and GLSL ES 3.00) expanded the mix() built-in function to operate on a boolean third argument that does not interpolate but selects. This
			* extension extends mix() to select between int, uint, and bool components.</p>
			*
			* <p>Requires {@link GL30 OpenGL 3.0}.</p>
			*/
			const bool EXT_shader_integer_mix;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/EXT/shadow_funcs.txt">EXT_shadow_funcs</a> extension is supported.
			*
			* <p>This extension generalizes the {@link #ARB_shadow ARB_shadow} extension to support all eight binary texture comparison functions rather than just {@link GL11#LEQUAL LEQUAL} and
			* {@link GL11#GEQUAL GEQUAL}.</p>
			*
			* <p>Requires {@link #ARB_depth_texture ARB_depth_texture} and {@link #ARB_shadow ARB_shadow}.</p>
			*/
			const bool EXT_shadow_funcs;
			/** When true, {@link EXTSharedTexturePalette} is supported. */
			const bool EXT_shared_texture_palette;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/EXT/sparse_texture2.txt">EXT_sparse_texture2</a> extension is supported.
			*
			* <p>This extension builds on the {@link ARBSparseTexture ARB_sparse_texture} extension, providing the following new functionality:</p>
			*
			* <ul>
			* <li>New built-in GLSL texture lookup and image load functions are provided that return information on whether the texels accessed for the texture
			* lookup accessed uncommitted texture memory.
			*
			* <p>New built-in GLSL texture lookup functions are provided that specify a minimum level of detail to use for lookups where the level of detail is
			* computed automatically. This allows shaders to avoid accessing unpopulated portions of high-resolution levels of detail when it knows that the
			* memory accessed is unpopulated, either from a priori knowledge or from feedback provided by the return value of previously executed "sparse"
			* texture lookup functions.</p>
			*
			* <p>Reads of uncommitted texture memory will act as though such memory were filled with zeroes; previously, the values returned by reads were undefined.</p>
			*
			* <p>Standard implementation-independent virtual page sizes for internal formats required to be supported with sparse textures. These standard sizes can
			* be requested by leaving {@link ARBSparseTexture#VIRTUAL_PAGE_SIZE_INDEX_ARB VIRTUAL_PAGE_SIZE_INDEX_ARB} at its initial value (0).</p>
			*
			* <p>Support for creating sparse multisample and multisample array textures is added. However, the virtual page sizes for such textures remain fully
			* implementation-dependent.</p></li>
			* </ul>
			*
			* <p>Requires {@link ARBSparseTexture ARB_sparse_texture}.</p>
			*/
			const bool EXT_sparse_texture2;
			/** When true, {@link EXTStencilClearTag} is supported. */
			const bool EXT_stencil_clear_tag;
			/** When true, {@link EXTStencilTwoSide} is supported. */
			const bool EXT_stencil_two_side;
			/** When true, {@link EXTStencilWrap} is supported. */
			const bool EXT_stencil_wrap;
			/** When true, {@link EXTTextureArray} is supported. */
			const bool EXT_texture_array;
			/** When true, {@link EXTTextureBufferObject} is supported. */
			const bool EXT_texture_buffer_object;
			/** When true, {@link EXTTextureCompressionLATC} is supported. */
			const bool EXT_texture_compression_latc;
			/** When true, {@link EXTTextureCompressionRGTC} is supported. */
			const bool EXT_texture_compression_rgtc;
			/** When true, {@link EXTTextureCompressionS3TC} is supported. */
			const bool EXT_texture_compression_s3tc;
			/** When true, {@link EXTTextureFilterAnisotropic} is supported. */
			const bool EXT_texture_filter_anisotropic;
			/** When true, {@link EXTTextureFilterMinmax} is supported. */
			const bool EXT_texture_filter_minmax;
			/** When true, {@link EXTTextureInteger} is supported. */
			const bool EXT_texture_integer;
			/** When true, {@link EXTTextureMirrorClamp} is supported. */
			const bool EXT_texture_mirror_clamp;
			/** When true, {@link EXTTextureSharedExponent} is supported. */
			const bool EXT_texture_shared_exponent;
			/** When true, {@link EXTTextureSnorm} is supported. */
			const bool EXT_texture_snorm;
			/** When true, {@link EXTTextureSRGB} is supported. */
			const bool EXT_texture_sRGB;
			/** When true, {@link EXTTextureSRGBDecode} is supported. */
			const bool EXT_texture_sRGB_decode;
			/** When true, {@link EXTTextureSwizzle} is supported. */
			const bool EXT_texture_swizzle;
			/** When true, {@link EXTTimerQuery} is supported. */
			const bool EXT_timer_query;
			/** When true, {@link EXTTransformFeedback} is supported. */
			const bool EXT_transform_feedback;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/EXT/vertex_array_bgra.txt">EXT_vertex_array_bgra</a> extension is supported.
			*
			* <p>This extension provides a single new component format for vertex arrays to read 4-component unsigned byte vertex attributes with a BGRA component
			* ordering.</p>
			*
			* <p>OpenGL expects vertex arrays containing 4 unsigned bytes per element to be in the RGBA, STRQ, or XYZW order (reading components left-to-right in their
			* lower address to higher address order). Essentially the order the components appear in memory is the order the components appear in the resulting
			* vertex attribute vector.</p>
			*
			* <p>However Direct3D has color (diffuse and specular) vertex arrays containing 4 unsigned bytes per element that are in a BGRA order (again reading
			* components left-to-right in their lower address to higher address order). Direct3D calls this "ARGB" reading the components in the opposite order
			* (reading components left-to-right in their higher address to lower address order). This ordering is generalized in the DirectX 10 by the
			* DXGI_FORMAT_B8G8R8A8_UNORM format.</p>
			*
			* <p>For an OpenGL application to source color data from a vertex buffer formatted for Direct3D's color array format conventions, the application is forced
			* to either:</p>
			*
			* <ol>
			* <li>Rely on a vertex program or shader to swizzle the color components from the BGRA to conventional RGBA order.</li>
			* <li>Re-order the color data components in the vertex buffer from Direct3D's native BGRA order to OpenGL's native RGBA order.</li>
			* </ol>
			*
			* <p>Neither option is entirely satisfactory.</p>
			*
			* <p>Option 1 means vertex shaders have to be re-written to source colors differently. If the same vertex shader is used with vertex arrays configured to
			* source the color as 4 floating-point color components, the swizzle for BGRA colors stored as 4 unsigned bytes is no longer appropriate. The shader's
			* swizzling of colors becomes dependent on the type and number of color components. Ideally the vertex shader should be independent from the format and
			* component ordering of the data it sources.</p>
			*
			* <p>Option 2 is expensive because vertex buffers may have to be reformatted prior to use. OpenGL treats the memory for vertex arrays (whether client-side
			* memory or buffer objects) as essentially untyped memory and vertex arrays can be stored separately, interleaved, or even interwoven (where multiple
			* arrays overlap with differing strides and formats).</p>
			*
			* <p>Rather than force a re-ordering of either vertex array components in memory or a vertex array format-dependent re-ordering of vertex shader inputs,
			* OpenGL can simply provide a vertex array format that matches the Direct3D color component ordering.</p>
			*
			* <p>This approach mimics that of the EXT_bgra extension for pixel and texel formats except for vertex instead of image data.</p>
			*/
			const bool EXT_vertex_array_bgra;
			/** When true, {@link EXTVertexAttrib64bit} is supported. */
			const bool EXT_vertex_attrib_64bit;
			/** When true, {@link EXTWindowRectangles} is supported. */
			const bool EXT_window_rectangles;
			/** When true, {@link EXTX11SyncObject} is supported. */
			const bool EXT_x11_sync_object;
			/** When true, {@link INTELConservativeRasterization} is supported. */
			const bool INTEL_conservative_rasterization;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/INTEL/fragment_shader_ordering.txt">INTEL_fragment_shader_ordering</a> extension is supported.
			*
			* <p>Graphics devices may execute in parallel fragment shaders referring to the same window xy coordinates. Framebuffer writes are guaranteed to be
			* processed in primitive rasterization order, but there is no order guarantee for other instructions and image or buffer object accesses in particular.</p>
			*
			* <p>The extension introduces a new GLSL built-in function, beginFragmentShaderOrderingINTEL(), which blocks execution of a fragment shader invocation until
			* invocations from previous primitives that map to the same xy window coordinates (and same sample when per-sample shading is active) complete their
			* execution. All memory transactions from previous fragment shader invocations are made visible to the fragment shader invocation that called
			* beginFragmentShaderOrderingINTEL() when the function returns.</p>
			*/
			const bool INTEL_fragment_shader_ordering;
			/** When true, {@link INTELFramebufferCMAA} is supported. */
			const bool INTEL_framebuffer_CMAA;
			/** When true, {@link INTELMapTexture} is supported. */
			const bool INTEL_map_texture;
			/** When true, {@link INTELPerformanceQuery} is supported. */
			const bool INTEL_performance_query;
			/** When true, {@link KHRBlendEquationAdvanced} is supported. */
			const bool KHR_blend_equation_advanced;
			/** When true, {@link KHRBlendEquationAdvancedCoherent} is supported. */
			const bool KHR_blend_equation_advanced_coherent;
			/** When true, {@link KHRContextFlushControl} is supported. */
			const bool KHR_context_flush_control;
			/** When true, {@link KHRDebug} is supported. */
			const bool KHR_debug;
			/** When true, {@link KHRNoError} is supported. */
			const bool KHR_no_error;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/KHR/robust_buffer_access_behavior.txt">KHR_robust_buffer_access_behavior</a> extension is supported.
			*
			* <p>This extension specifies the behavior of out-of-bounds buffer and array accesses. This is an improvement over the existing {@link #KHR_robustness KHR_robustness}
			* extension which states that the application should not crash, but that behavior is otherwise undefined. This extension specifies the access protection
			* provided by the GL to ensure that out-of-bounds accesses cannot read from or write to data not owned by the application. All accesses are contained
			* within the buffer object and program area they reference. These additional robustness guarantees apply to contexts created with the robust access flag
			* set.</p>
			*
			* <p>Requires {@link GL32 OpenGL 3.2} and {@link #KHR_robustness KHR_robustness}.</p>
			*/
			const bool KHR_robust_buffer_access_behavior;
			/** When true, {@link KHRRobustness} is supported. */
			const bool KHR_robustness;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/KHR/texture_compression_astc_hdr.txt">KHR_texture_compression_astc_hdr</a> extension is supported.
			*
			* <p>This extension corresponds to the ASTC HDR Profile, see {@link KHRTextureCompressionASTCLDR KHR_texture_compression_astc_ldr} for details.</p>
			*/
			const bool KHR_texture_compression_astc_hdr;
			/** When true, {@link KHRTextureCompressionASTCLDR} is supported. */
			const bool KHR_texture_compression_astc_ldr;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/KHR/texture_compression_astc_sliced_3d.txt">KHR_texture_compression_astc_sliced_3d</a> extension is supported.
			*
			* <p>Adaptive Scalable Texture Compression (ASTC) is a new texture compression technology that offers unprecendented flexibility, while producing better or
			* comparable results than existing texture compressions at all bit rates. It includes support for 2D and slice-based 3D textures, with low and high
			* dynamic range, at bitrates from below 1 bit/pixel up to 8 bits/pixel in fine steps.</p>
			*
			* <p>This extension extends the functionality of {@link KHRTextureCompressionASTCLDR KHR_texture_compression_astc_ldr} to include slice-based 3D textures for textures using the LDR
			* profile in the same way as the HDR profile allows slice-based 3D textures.</p>
			*
			* <p>Requires {@link KHRTextureCompressionASTCLDR KHR_texture_compression_astc_ldr}.</p>
			*/
			const bool KHR_texture_compression_astc_sliced_3d;
			/** When true, {@link NVBindlessMultiDrawIndirect} is supported. */
			const bool NV_bindless_multi_draw_indirect;
			/** When true, {@link NVBindlessMultiDrawIndirectCount} is supported. */
			const bool NV_bindless_multi_draw_indirect_count;
			/** When true, {@link NVBindlessTexture} is supported. */
			const bool NV_bindless_texture;
			/** When true, {@link NVBlendEquationAdvanced} is supported. */
			const bool NV_blend_equation_advanced;
			/** When true, {@link NVBlendEquationAdvancedCoherent} is supported. */
			const bool NV_blend_equation_advanced_coherent;
			/** When true, the <a href="http://www.opengl.org/registry/specs/NV/blend_square.txt">NV_blend_square</a> extension is supported. */
			const bool NV_blend_square;
			/** When true, {@link NVClipSpaceWScaling} is supported. */
			const bool NV_clip_space_w_scaling;
			/** When true, {@link NVCommandList} is supported. */
			const bool NV_command_list;
			/** When true, {@link NVConditionalRender} is supported. */
			const bool NV_conditional_render;
			/** When true, {@link NVConservativeRaster} is supported. */
			const bool NV_conservative_raster;
			/** When true, {@link NVConservativeRasterDilate} is supported. */
			const bool NV_conservative_raster_dilate;
			/** When true, {@link NVConservativeRasterPreSnapTriangles} is supported. */
			const bool NV_conservative_raster_pre_snap_triangles;
			/** When true, {@link NVCopyDepthToColor} is supported. */
			const bool NV_copy_depth_to_color;
			/** When true, {@link NVCopyImage} is supported. */
			const bool NV_copy_image;
			/** When true, {@link NVDeepTexture3D} is supported. */
			const bool NV_deep_texture3D;
			/** When true, {@link NVDepthBufferFloat} is supported. */
			const bool NV_depth_buffer_float;
			/** When true, {@link NVDepthClamp} is supported. */
			const bool NV_depth_clamp;
			/** When true, {@link NVDrawTexture} is supported. */
			const bool NV_draw_texture;
			/** When true, {@link NVDrawVulkanImage} is supported. */
			const bool NV_draw_vulkan_image;
			/** When true, the <a href="http://www.opengl.org/registry/specs/NV/ES3_1_compatibility.txt">NV_ES3_1_compatibility</a> extension is supported. */
			const bool NV_ES3_1_compatibility;
			/** When true, {@link NVExplicitMultisample} is supported. */
			const bool NV_explicit_multisample;
			/** When true, {@link NVFence} is supported. */
			const bool NV_fence;
			/** When true, {@link NVFillRectangle} is supported. */
			const bool NV_fill_rectangle;
			/** When true, {@link NVFloatBuffer} is supported. */
			const bool NV_float_buffer;
			/** When true, {@link NVFogDistance} is supported. */
			const bool NV_fog_distance;
			/** When true, {@link NVFragmentCoverageToColor} is supported. */
			const bool NV_fragment_coverage_to_color;
			/** When true, the <a href="http://www.opengl.org/registry/specs/NV/fragment_program4.txt">NV_fragment_program4</a> extension is supported. */
			const bool NV_fragment_program4;
			/** When true, the <a href="http://www.opengl.org/registry/specs/NV/fragment_program_option.txt">NV_fragment_program_option</a> extension is supported. */
			const bool NV_fragment_program_option;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/fragment_shader_interlock.txt">NV_fragment_shader_interlock</a> extension is supported.
			*
			* <p>In unextended OpenGL 4.3, applications may produce a large number of fragment shader invocations that perform loads and stores to memory using image
			* uniforms, atomic counter uniforms, buffer variables, or pointers. The order in which loads and stores to common addresses are performed by different
			* fragment shader invocations is largely undefined. For algorithms that use shader writes and touch the same pixels more than once, one or more of the
			* following techniques may be required to ensure proper execution ordering:</p>
			*
			* <ul>
			* <li>inserting Finish or WaitSync commands to drain the pipeline between different "passes" or "layers";</li>
			* <li>using only atomic memory operations to write to shader memory (which may be relatively slow and limits how memory may be updated); or</li>
			* <li>injecting spin loops into shaders to prevent multiple shader invocations from touching the same memory concurrently.</li>
			* </ul>
			*
			* <p>This extension provides new GLSL built-in functions beginInvocationInterlockNV() and endInvocationInterlockNV() that delimit a critical section of
			* fragment shader code. For pairs of shader invocations with "overlapping" coverage in a given pixel, the OpenGL implementation will guarantee that the
			* critical section of the fragment shader will be executed for only one fragment at a time.</p>
			*
			* <p>There are four different interlock modes supported by this extension, which are identified by layout qualifiers. The qualifiers
			* "pixel_interlock_ordered" and "pixel_interlock_unordered" provides mutual exclusion in the critical section for any pair of fragments corresponding to
			* the same pixel. When using multisampling, the qualifiers "sample_interlock_ordered" and "sample_interlock_unordered" only provide mutual exclusion for
			* pairs of fragments that both cover at least one common sample in the same pixel; these are recommended for performance if shaders use per-sample data
			* structures.</p>
			*
			* <p>Additionally, when the "pixel_interlock_ordered" or "sample_interlock_ordered" layout qualifier is used, the interlock also guarantees that the
			* critical section for multiple shader invocations with "overlapping" coverage will be executed in the order in which the primitives were processed by
			* the GL. Such a guarantee is useful for applications like blending in the fragment shader, where an application requires that fragment values to be
			* composited in the framebuffer in primitive order.</p>
			*
			* <p>This extension can be useful for algorithms that need to access per-pixel data structures via shader loads and stores. Such algorithms using this
			* extension can access such data structures in the critical section without worrying about other invocations for the same pixel accessing the data
			* structures concurrently. Additionally, the ordering guarantees are useful for cases where the API ordering of fragments is meaningful. For example,
			* applications may be able to execute programmable blending operations in the fragment shader, where the destination buffer is read via image loads and
			* the final value is written via image stores.</p>
			*
			* <p>Requires {@link GL43 OpenGL 4.3} and GLSL 4.30.</p>
			*/
			const bool NV_fragment_shader_interlock;
			/** When true, {@link NVFramebufferMixedSamples} is supported. */
			const bool NV_framebuffer_mixed_samples;
			/** When true, {@link NVFramebufferMultisampleCoverage} is supported. */
			const bool NV_framebuffer_multisample_coverage;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/geometry_shader4.txt">NV_geometry_shader4</a> extension is supported.
			*
			* <p>This extension builds upon the {@link #EXT_geometry_shader4 EXT_geometry_shader4} specification to provide two additional capabilities:</p>
			*
			* <ul>
			* <li>Support for QUADS, QUAD_STRIP, and POLYGON primitive types when geometry shaders are enabled.  Such primitives will be tessellated into individual
			* triangles.</li>
			* <li>Setting the value of GEOMETRY_VERTICES_OUT_EXT will take effect immediately. It is not necessary to link the program object in order for this change
			* to take effect, as is the case in the EXT version of this extension.</li>
			* </ul>
			*
			* <p>Requires {@link #EXT_geometry_shader4 EXT_geometry_shader4}.</p>
			*/
			const bool NV_geometry_shader4;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/geometry_shader_passthrough.txt">NV_geometry_shader_passthrough</a> extension is supported.
			*
			* <p>This extension provides a shading language abstraction to express such shaders without requiring explicit logic to manually copy attributes from input
			* vertices to output vertices.</p>
			*/
			const bool NV_geometry_shader_passthrough;
			/** When true, {@link NVGPUMulticast} is supported. */
			const bool NV_gpu_multicast;
			/** When true, {@link NVGPUShader5} is supported. */
			const bool NV_gpu_shader5;
			/** When true, {@link NVHalfFloat} is supported. */
			const bool NV_half_float;
			/** When true, {@link NVInternalformatSampleQuery} is supported. */
			const bool NV_internalformat_sample_query;
			/** When true, {@link NVLightMaxExponent} is supported. */
			const bool NV_light_max_exponent;
			/** When true, {@link NVMultisampleCoverage} is supported. */
			const bool NV_multisample_coverage;
			/** When true, {@link NVMultisampleFilterHint} is supported. */
			const bool NV_multisample_filter_hint;
			/** When true, {@link NVPackedDepthStencil} is supported. */
			const bool NV_packed_depth_stencil;
			/** When true, {@link NVPathRendering} is supported. */
			const bool NV_path_rendering;
			/** When true, {@link NVPathRenderingSharedEdge} is supported. */
			const bool NV_path_rendering_shared_edge;
			/** When true, {@link NVPixelDataRange} is supported. */
			const bool NV_pixel_data_range;
			/** When true, {@link NVPointSprite} is supported. */
			const bool NV_point_sprite;
			/** When true, {@link NVPrimitiveRestart} is supported. */
			const bool NV_primitive_restart;
			/** When true, {@link NVRobustnessVideoMemoryPurge} is supported. */
			const bool NV_robustness_video_memory_purge;
			/** When true, {@link NVSampleLocations} is supported. */
			const bool NV_sample_locations;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/sample_mask_override_coverage.txt">NV_sample_mask_override_coverage</a> extension is supported.
			*
			* <p>This extension allows the fragment shader to control whether the gl_SampleMask output can enable samples that were not covered by the original
			* primitive, or that failed the early depth/stencil tests.</p>
			*/
			const bool NV_sample_mask_override_coverage;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/shader_atomic_float.txt">NV_shader_atomic_float</a> extension is supported.
			*
			* <p>This extension provides GLSL built-in functions and assembly opcodes allowing shaders to perform atomic read-modify-write operations to buffer or
			* texture memory with floating-point components.  The set of atomic operations provided by this extension is limited to adds and exchanges. Providing
			* atomic add support allows shaders to atomically accumulate the sum of floating-point values into buffer or texture memory across multiple (possibly
			* concurrent) shader invocations.</p>
			*
			* <p>This extension provides GLSL support for atomics targeting image uniforms (if GLSL 4.20, {@link #ARB_shader_image_load_store ARB_shader_image_load_store}, or
			* {@link #EXT_shader_image_load_store EXT_shader_image_load_store} is supported) or floating-point pointers (if {@link #NV_gpu_shader5 NV_gpu_shader5} is supported). Additionally, assembly opcodes
			* for these operations is also provided if <a href="http://www.opengl.org/registry/specs/NV/gpu_program5.txt">NV_shader_atomic_float</a> is supported.</p>
			*/
			const bool NV_shader_atomic_float;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/shader_atomic_float64.txt">NV_shader_atomic_float64</a> extension is supported.
			*
			* <p>This extension provides GLSL built-in functions and assembly opcodes allowing shaders to perform atomic read-modify-write operations to buffer or
			* shared memory with double-precision floating-point components.  The set of atomic operations provided by this extension is limited to adds and
			* exchanges. Providing atomic add support allows shaders to atomically accumulate the sum of double-precision floating-point values into buffer memory
			* across multiple (possibly concurrent) shader invocations.</p>
			*
			* <p>This extension provides GLSL support for atomics targeting double-precision floating-point pointers (if {@link NVGPUShader5 NV_gpu_shader5} is supported).
			* Additionally, assembly opcodes for these operations are also provided if {@code NV_gpu_program5} is supported.</p>
			*
			* <p>Requires {@link ARBGPUShaderFP64 ARB_gpu_shader_fp64} or {@code NV_gpu_program_fp64}.</p>
			*/
			const bool NV_shader_atomic_float64;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/shader_atomic_fp16_vector.txt">NV_shader_atomic_fp16_vector</a> extension is supported.
			*
			* <p>This extension provides GLSL built-in functions and assembly opcodes allowing shaders to perform a limited set of atomic read-modify-write operations
			* to buffer or texture memory with 16-bit floating point vector surface formats.</p>
			*
			* <p>Requires {@link #NV_gpu_shader5 NV_gpu_shader5}.</p>
			*/
			const bool NV_shader_atomic_fp16_vector;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/shader_atomic_int64.txt">NV_shader_atomic_int64</a> extension is supported.
			*
			* <p>This extension provides additional GLSL built-in functions and assembly opcodes allowing shaders to perform additional atomic read-modify-write
			* operations on 64-bit signed and unsigned integers stored in buffer object memory.</p>
			*/
			const bool NV_shader_atomic_int64;
			/** When true, {@link NVShaderBufferLoad} is supported. */
			const bool NV_shader_buffer_load;
			/** When true, {@link NVShaderBufferStore} is supported. */
			const bool NV_shader_buffer_store;
			/** When true, {@link NVShaderThreadGroup} is supported. */
			const bool NV_shader_thread_group;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/shader_thread_shuffle.txt">NV_shader_thread_shuffle</a> extension is supported.
			*
			* <p>Implementations of the OpenGL Shading Language may, but are not required, to run multiple shader threads for a single stage as a SIMD thread group,
			* where individual execution threads are assigned to thread groups in an undefined, implementation-dependent order.  This extension provides a set of
			* new features to the OpenGL Shading Language to share data between multiple threads within a thread group.</p>
			*
			* <p>Requires {@link GL43 OpenGL 4.3} and GLSL 4.3.</p>
			*/
			const bool NV_shader_thread_shuffle;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/stereo_view_rendering.txt">NV_stereo_view_rendering</a> extension is supported.
			*
			* <p>Virtual reality (VR) applications often render a single logical scene from multiple views corresponding to a pair of eyes. The views (eyes) are
			* separated by a fixed offset in the X direction.</p>
			*
			* <p>Traditionally, multiple views are rendered via multiple rendering passes. This is expensive for the GPU because the objects in the scene must be
			* transformed, rasterized, shaded, and fragment processed redundantly. This is expensive for the CPU because the scene graph needs to be visited multiple
			* times and driver validation happens for each view. Rendering N passes tends to take N times longer than a single pass.</p>
			*
			* <p>This extension provides a mechanism to render binocular (stereo) views from a single stream of OpenGL rendering commands. Vertex, tessellation, and
			* geometry (VTG) shaders can output two positions for each vertex corresponding to the two eye views. A built-in "gl_SecondaryPositionNV" is added to
			* specify the second position. The positions from each view may be sent to different viewports and/or layers. A built-in "gl_SecondaryViewportMaskNV[]"
			* is also added to specify the viewport mask for the second view. A new layout-qualifier "secondary_view_offset" is added for built-in output "gl_Layer"
			* which allows for the geometry from each view to be sent to different layers for rendering.</p>
			*
			* <p>Requires {@link #NV_viewport_array2 NV_viewport_array2}.</p>
			*/
			const bool NV_stereo_view_rendering;
			/** When true, {@link NVTexgenReflection} is supported. */
			const bool NV_texgen_reflection;
			/** When true, {@link NVTextureBarrier} is supported. */
			const bool NV_texture_barrier;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/texture_compression_vtc.txt">NV_texture_compression_vtc</a> extension is supported.
			*
			* <p>This extension adds support for the VTC 3D texture compression formats, which are analogous to the S3TC texture compression formats, with the addition
			* of some retiling in the Z direction. VTC has the same compression ratio as S3TC and uses 4x4x1, 4x4x2, (4x4x3 when non-power-of-two textures are
			* supported), or 4x4x4 blocks.</p>
			*/
			const bool NV_texture_compression_vtc;
			/** When true, {@link NVTextureMultisample} is supported. */
			const bool NV_texture_multisample;
			/** When true, {@link NVTransformFeedback} is supported. */
			const bool NV_transform_feedback;
			/** When true, {@link NVTransformFeedback2} is supported. */
			const bool NV_transform_feedback2;
			/** When true, {@link NVUniformBufferUnifiedMemory} is supported. */
			const bool NV_uniform_buffer_unified_memory;
			/** When true, {@link NVVertexArrayRange} is supported. */
			const bool NV_vertex_array_range;
			/** When true, {@link NVVertexArrayRange2} is supported. */
			const bool NV_vertex_array_range2;
			/** When true, {@link NVVertexAttribInteger64bit} is supported. */
			const bool NV_vertex_attrib_integer_64bit;
			/** When true, {@link NVVertexBufferUnifiedMemory} is supported. */
			const bool NV_vertex_buffer_unified_memory;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NV/viewport_array2.txt">NV_viewport_array2</a> extension is supported.
			*
			* <p>This extension provides new support allowing a single primitive to be broadcast to multiple viewports and/or multiple layers. A shader output
			* gl_ViewportMask[] is provided, allowing a single primitive to be output to multiple viewports simultaneously. Also, a new shader option is provided to
			* control whether the effective viewport index is added into gl_Layer. These capabilities allow a single primitive to be output to multiple layers
			* simultaneously.</p>
			*
			* <p>The gl_ViewportMask[] output is available in vertex, tessellation control, tessellation evaluation, and geometry shaders. gl_ViewportIndex and gl_Layer
			* are also made available in all these shader stages. The actual viewport index or mask and render target layer values are taken from the last active
			* shader stage from this set of stages.</p>
			*
			* <p>This extension is a superset of the AMD_vertex_shader_layer and AMD_vertex_shader_viewport_index extensions, and thus those extension strings are
			* expected to be exported if NV_viewport_array2 is supported.</p>
			*/
			const bool NV_viewport_array2;
			/** When true, {@link NVViewportSwizzle} is supported. */
			const bool NV_viewport_swizzle;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/NVX/nvx_blend_equation_advanced_multi_draw_buffers.txt">NVX_blend_equation_advanced_multi_draw_buffers</a> extension is supported.
			*
			* <p>This extension adds support for using advanced blend equations introduced with {@link NVBlendEquationAdvanced NV_blend_equation_advanced} (and standardized by
			* {@link KHRBlendEquationAdvanced KHR_blend_equation_advanced}) in conjunction with multiple draw buffers. The NV_blend_equation_advanced extension supports advanced blending
			* equations only when rending to a single color buffer using fragment color zero and throws and {@link GL11#INVALID_OPERATION INVALID_OPERATION} error when multiple draw buffers are
			* used. This extension removes this restriction.</p>
			*
			* <p>Requires either {@link NVBlendEquationAdvanced NV_blend_equation_advanced} or {@link KHRBlendEquationAdvanced KHR_blend_equation_advanced}.</p>
			*/
			const bool NVX_blend_equation_advanced_multi_draw_buffers;
			/** When true, {@link NVXConditionalRender} is supported. */
			const bool NVX_conditional_render;
			/** When true, {@link NVXGPUMemoryInfo} is supported. */
			const bool NVX_gpu_memory_info;
			/** When true, {@link OVRMultiview} is supported. */
			const bool OVR_multiview;
			/**
			* When true, the <a href="http://www.opengl.org/registry/specs/OVR/multiview2.txt">OVR_multiview2</a> extension is supported.
			*
			* <p>This extension relaxes the restriction in OVR_multiview that only {@code gl_Position} can depend on {@code ViewID} in the vertex shader.  With this
			* change, view-dependent outputs like reflection vectors and similar are allowed.</p>
			*
			* <p>Requires {@link GL30 OpenGL 3.0} and {@link OVRMultiview OVR_multiview}.</p>
			*/
			const bool OVR_multiview2;

			/** When true, deprecated functions are not available. */
			const bool forwardCompatible;
		};
	}
}

