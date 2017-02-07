#include "GLCapabilities.h"
#include <algorithm>
#include <ctype.h>

#if defined(_DEBUG)
#include <iostream>
#endif

namespace jet
{
	namespace util
	{
		static bool contains(std::set<std::string>& ext, std::string str)
		{
			return ext.find(str) != ext.end();
		}
	
		GLCapabilities::GLCapabilities(std::set<std::string>& ext, bool fc):forwardCompatible(fc),
			OpenGL11(contains(ext, "OpenGL11")),
			OpenGL12(contains(ext, "OpenGL12")),
			OpenGL13(contains(ext, "OpenGL13")),
			OpenGL14(contains(ext, "OpenGL14")),
			OpenGL15(contains(ext, "OpenGL15")),
			OpenGL20(contains(ext, "OpenGL20")),
			OpenGL21(contains(ext, "OpenGL21")),
			OpenGL30(contains(ext, "OpenGL30")),
			OpenGL31(contains(ext, "OpenGL31")),
			OpenGL32(contains(ext, "OpenGL32")),
			OpenGL33(contains(ext, "OpenGL33")),
			OpenGL40(contains(ext, "OpenGL40")),
			OpenGL41(contains(ext, "OpenGL41")),
			OpenGL42(contains(ext, "OpenGL42")),
			OpenGL43(contains(ext, "OpenGL43")),
			OpenGL44(contains(ext, "OpenGL44")),
			OpenGL45(contains(ext, "OpenGL45")),
			AMD_blend_minmax_factor(contains(ext, "GL_AMD_blend_minmax_factor")),
			AMD_conservative_depth(contains(ext, "GL_AMD_conservative_depth")),
			AMD_debug_output(contains(ext, "GL_AMD_debug_output")),
			AMD_depth_clamp_separate(contains(ext, "GL_AMD_depth_clamp_separate")),
			AMD_draw_buffers_blend(contains(ext, "GL_AMD_draw_buffers_blend")),
			AMD_gcn_shader(contains(ext, "GL_AMD_gcn_shader")),
			AMD_gpu_shader_half_float(contains(ext, "GL_AMD_gpu_shader_half_float")),
			AMD_gpu_shader_int64(contains(ext, "GL_AMD_gpu_shader_int64")),
			AMD_interleaved_elements(contains(ext, "GL_AMD_interleaved_elements")),
			AMD_occlusion_query_event(contains(ext, "GL_AMD_occlusion_query_event")),
			AMD_performance_monitor(contains(ext, "GL_AMD_performance_monitor")),
			AMD_pinned_memory(contains(ext, "GL_AMD_pinned_memory")),
			AMD_query_buffer_object(contains(ext, "GL_AMD_query_buffer_object")),
			AMD_sample_positions(contains(ext, "GL_AMD_sample_positions")),
			AMD_seamless_cubemap_per_texture(contains(ext, "GL_AMD_seamless_cubemap_per_texture")),
			AMD_shader_atomic_counter_ops(contains(ext, "GL_AMD_shader_atomic_counter_ops")),
			AMD_shader_ballot(contains(ext, "GL_AMD_shader_ballot")),
			AMD_shader_explicit_vertex_parameter(contains(ext, "GL_AMD_shader_explicit_vertex_parameter")),
			AMD_shader_stencil_export(contains(ext, "GL_AMD_shader_stencil_export")),
			AMD_shader_trinary_minmax(contains(ext, "GL_AMD_shader_trinary_minmax")),
			AMD_sparse_texture(contains(ext, "GL_AMD_sparse_texture")),
			AMD_stencil_operation_extended(contains(ext, "GL_AMD_stencil_operation_extended")),
			AMD_texture_texture4(contains(ext, "GL_AMD_texture_texture4")),
			AMD_transform_feedback3_lines_triangles(contains(ext, "GL_AMD_transform_feedback3_lines_triangles")),
			AMD_transform_feedback4(contains(ext, "GL_AMD_transform_feedback4")),
			AMD_vertex_shader_layer(contains(ext, "GL_AMD_vertex_shader_layer")),
			AMD_vertex_shader_tessellator(contains(ext, "GL_AMD_vertex_shader_tessellator")),
			AMD_vertex_shader_viewport_index(contains(ext, "GL_AMD_vertex_shader_viewport_index")),
			ARB_arrays_of_arrays(contains(ext, "GL_ARB_arrays_of_arrays")),
			ARB_base_instance(contains(ext, "GL_ARB_base_instance")),
			ARB_bindless_texture(contains(ext, "GL_ARB_bindless_texture")),
			ARB_blend_func_extended(contains(ext, "GL_ARB_blend_func_extended")),
			ARB_buffer_storage(contains(ext, "GL_ARB_buffer_storage")),
			ARB_cl_event(contains(ext, "GL_ARB_cl_event")),
			ARB_clear_buffer_object(contains(ext, "GL_ARB_clear_buffer_object")),
			ARB_clear_texture(contains(ext, "GL_ARB_clear_texture")),
			ARB_clip_control(contains(ext, "GL_ARB_clip_control")),
			ARB_color_buffer_float(contains(ext, "GL_ARB_color_buffer_float")),
			ARB_compatibility(contains(ext, "GL_ARB_compatibility")),
			ARB_compressed_texture_pixel_storage(contains(ext, "GL_ARB_compressed_texture_pixel_storage")),
			ARB_compute_shader(contains(ext, "GL_ARB_compute_shader")),
			ARB_compute_variable_group_size(contains(ext, "GL_ARB_compute_variable_group_size")),
			ARB_conditional_render_inverted(contains(ext, "GL_ARB_conditional_render_inverted")),
			ARB_conservative_depth(contains(ext, "GL_ARB_conservative_depth")),
			ARB_copy_buffer(contains(ext, "GL_ARB_copy_buffer")),
			ARB_copy_image(contains(ext, "GL_ARB_copy_image")),
			ARB_cull_distance(contains(ext, "GL_ARB_cull_distance")),
			ARB_debug_output(contains(ext, "GL_ARB_debug_output")),
			ARB_depth_buffer_float(contains(ext, "GL_ARB_depth_buffer_float")),
			ARB_depth_clamp(contains(ext, "GL_ARB_depth_clamp")),
			ARB_depth_texture(contains(ext, "GL_ARB_depth_texture")),
			ARB_derivative_control(contains(ext, "GL_ARB_derivative_control")),
			ARB_direct_state_access(contains(ext, "GL_ARB_direct_state_access")),
			ARB_draw_buffers(contains(ext, "GL_ARB_draw_buffers")),
			ARB_draw_buffers_blend(contains(ext, "GL_ARB_draw_buffers_blend")),
			ARB_draw_elements_base_vertex(contains(ext, "GL_ARB_draw_elements_base_vertex")),
			ARB_draw_indirect(contains(ext, "GL_ARB_draw_indirect")),
			ARB_draw_instanced(contains(ext, "GL_ARB_draw_instanced")),
			ARB_enhanced_layouts(contains(ext, "GL_ARB_enhanced_layouts")),
			ARB_ES2_compatibility(contains(ext, "GL_ARB_ES2_compatibility")),
			ARB_ES3_1_compatibility(contains(ext, "GL_ARB_ES3_1_compatibility")),
			ARB_ES3_2_compatibility(contains(ext, "GL_ARB_ES3_2_compatibility")),
			ARB_ES3_compatibility(contains(ext, "GL_ARB_ES3_compatibility")),
			ARB_explicit_attrib_location(contains(ext, "GL_ARB_explicit_attrib_location")),
			ARB_explicit_uniform_location(contains(ext, "GL_ARB_explicit_uniform_location")),
			ARB_fragment_coord_conventions(contains(ext, "GL_ARB_fragment_coord_conventions")),
			ARB_fragment_layer_viewport(contains(ext, "GL_ARB_fragment_layer_viewport")),
			ARB_fragment_program(contains(ext, "GL_ARB_fragment_program")),
			ARB_fragment_program_shadow(contains(ext, "GL_ARB_fragment_program_shadow")),
			ARB_fragment_shader(contains(ext, "GL_ARB_fragment_shader")),
			ARB_fragment_shader_interlock(contains(ext, "GL_ARB_fragment_shader_interlock")),
			ARB_framebuffer_no_attachments(contains(ext, "GL_ARB_framebuffer_no_attachments")),
			ARB_framebuffer_object(contains(ext, "GL_ARB_framebuffer_object")),
			ARB_framebuffer_sRGB(contains(ext, "GL_ARB_framebuffer_sRGB")),
			ARB_geometry_shader4(contains(ext, "GL_ARB_geometry_shader4")),
			ARB_get_program_binary(contains(ext, "GL_ARB_get_program_binary")),
			ARB_get_texture_sub_image(contains(ext, "GL_ARB_get_texture_sub_image")),
			ARB_gl_spirv(contains(ext, "GL_ARB_gl_spirv")),
			ARB_gpu_shader5(contains(ext, "GL_ARB_gpu_shader5")),
			ARB_gpu_shader_fp64(contains(ext, "GL_ARB_gpu_shader_fp64")),
			ARB_gpu_shader_int64(contains(ext, "GL_ARB_gpu_shader_int64")),
			ARB_half_float_pixel(contains(ext, "GL_ARB_half_float_pixel")),
			ARB_half_float_vertex(contains(ext, "GL_ARB_half_float_vertex")),
			ARB_imaging(contains(ext, "GL_ARB_imaging")),
			ARB_indirect_parameters(contains(ext, "GL_ARB_indirect_parameters")),
			ARB_instanced_arrays(contains(ext, "GL_ARB_instanced_arrays")),
			ARB_internalformat_query(contains(ext, "GL_ARB_internalformat_query")),
			ARB_internalformat_query2(contains(ext, "GL_ARB_internalformat_query2")),
			ARB_invalidate_subdata(contains(ext, "GL_ARB_invalidate_subdata")),
			ARB_map_buffer_alignment(contains(ext, "GL_ARB_map_buffer_alignment")),
			ARB_map_buffer_range(contains(ext, "GL_ARB_map_buffer_range")),
			ARB_matrix_palette(contains(ext, "GL_ARB_matrix_palette")),
			ARB_multi_bind(contains(ext, "GL_ARB_multi_bind")),
			ARB_multi_draw_indirect(contains(ext, "GL_ARB_multi_draw_indirect")),
			ARB_multisample(contains(ext, "GL_ARB_multisample")),
			ARB_multitexture(contains(ext, "GL_ARB_multitexture")),
			ARB_occlusion_query(contains(ext, "GL_ARB_occlusion_query")),
			ARB_occlusion_query2(contains(ext, "GL_ARB_occlusion_query2")),
			ARB_parallel_shader_compile(contains(ext, "GL_ARB_parallel_shader_compile")),
			ARB_pipeline_statistics_query(contains(ext, "GL_ARB_pipeline_statistics_query")),
			ARB_pixel_buffer_object(contains(ext, "GL_ARB_pixel_buffer_object")),
			ARB_point_parameters(contains(ext, "GL_ARB_point_parameters")),
			ARB_point_sprite(contains(ext, "GL_ARB_point_sprite")),
			ARB_post_depth_coverage(contains(ext, "GL_ARB_post_depth_coverage")),
			ARB_program_interface_query(contains(ext, "GL_ARB_program_interface_query")),
			ARB_provoking_vertex(contains(ext, "GL_ARB_provoking_vertex")),
			ARB_query_buffer_object(contains(ext, "GL_ARB_query_buffer_object")),
			ARB_robust_buffer_access_behavior(contains(ext, "GL_ARB_robust_buffer_access_behavior")),
			ARB_robustness(contains(ext, "GL_ARB_robustness")),
			ARB_robustness_isolation(contains(ext, "GL_ARB_robustness_isolation")),
			ARB_sample_locations(contains(ext, "GL_ARB_sample_locations")),
			ARB_sample_shading(contains(ext, "GL_ARB_sample_shading")),
			ARB_sampler_objects(contains(ext, "GL_ARB_sampler_objects")),
			ARB_seamless_cube_map(contains(ext, "GL_ARB_seamless_cube_map")),
			ARB_seamless_cubemap_per_texture(contains(ext, "GL_ARB_seamless_cubemap_per_texture")),
			ARB_separate_shader_objects(contains(ext, "GL_ARB_separate_shader_objects")),
			ARB_shader_atomic_counter_ops(contains(ext, "GL_ARB_shader_atomic_counter_ops")),
			ARB_shader_atomic_counters(contains(ext, "GL_ARB_shader_atomic_counters")),
			ARB_shader_ballot(contains(ext, "GL_ARB_shader_ballot")),
			ARB_shader_bit_encoding(contains(ext, "GL_ARB_shader_bit_encoding")),
			ARB_shader_clock(contains(ext, "GL_ARB_shader_clock")),
			ARB_shader_draw_parameters(contains(ext, "GL_ARB_shader_draw_parameters")),
			ARB_shader_group_vote(contains(ext, "GL_ARB_shader_group_vote")),
			ARB_shader_image_load_store(contains(ext, "GL_ARB_shader_image_load_store")),
			ARB_shader_image_size(contains(ext, "GL_ARB_shader_image_size")),
			ARB_shader_objects(contains(ext, "GL_ARB_shader_objects")),
			ARB_shader_precision(contains(ext, "GL_ARB_shader_precision")),
			ARB_shader_stencil_export(contains(ext, "GL_ARB_shader_stencil_export")),
			ARB_shader_storage_buffer_object(contains(ext, "GL_ARB_shader_storage_buffer_object")),
			ARB_shader_subroutine(contains(ext, "GL_ARB_shader_subroutine")),
			ARB_shader_texture_image_samples(contains(ext, "GL_ARB_shader_texture_image_samples")),
			ARB_shader_texture_lod(contains(ext, "GL_ARB_shader_texture_lod")),
			ARB_shader_viewport_layer_array(contains(ext, "GL_ARB_shader_viewport_layer_array")),
			ARB_shading_language_100(contains(ext, "GL_ARB_shading_language_100")),
			ARB_shading_language_420pack(contains(ext, "GL_ARB_shading_language_420pack")),
			ARB_shading_language_include(contains(ext, "GL_ARB_shading_language_include")),
			ARB_shading_language_packing(contains(ext, "GL_ARB_shading_language_packing")),
			ARB_shadow(contains(ext, "GL_ARB_shadow")),
			ARB_shadow_ambient(contains(ext, "GL_ARB_shadow_ambient")),
			ARB_sparse_buffer(contains(ext, "GL_ARB_sparse_buffer")),
			ARB_sparse_texture(contains(ext, "GL_ARB_sparse_texture")),
			ARB_sparse_texture2(contains(ext, "GL_ARB_sparse_texture2")),
			ARB_sparse_texture_clamp(contains(ext, "GL_ARB_sparse_texture_clamp")),
			ARB_stencil_texturing(contains(ext, "GL_ARB_stencil_texturing")),
			ARB_sync(contains(ext, "GL_ARB_sync")),
			ARB_tessellation_shader(contains(ext, "GL_ARB_tessellation_shader")),
			ARB_texture_barrier(contains(ext, "GL_ARB_texture_barrier")),
			ARB_texture_border_clamp(contains(ext, "GL_ARB_texture_border_clamp")),
			ARB_texture_buffer_object(contains(ext, "GL_ARB_texture_buffer_object")),
			ARB_texture_buffer_object_rgb32(contains(ext, "GL_ARB_texture_buffer_object_rgb32")),
			ARB_texture_buffer_range(contains(ext, "GL_ARB_texture_buffer_range")),
			ARB_texture_compression(contains(ext, "GL_ARB_texture_compression")),
			ARB_texture_compression_bptc(contains(ext, "GL_ARB_texture_compression_bptc")),
			ARB_texture_compression_rgtc(contains(ext, "GL_ARB_texture_compression_rgtc")),
			ARB_texture_cube_map(contains(ext, "GL_ARB_texture_cube_map")),
			ARB_texture_cube_map_array(contains(ext, "GL_ARB_texture_cube_map_array")),
			ARB_texture_env_add(contains(ext, "GL_ARB_texture_env_add")),
			ARB_texture_env_combine(contains(ext, "GL_ARB_texture_env_combine")),
			ARB_texture_env_crossbar(contains(ext, "GL_ARB_texture_env_crossbar")),
			ARB_texture_env_dot3(contains(ext, "GL_ARB_texture_env_dot3")),
			ARB_texture_filter_minmax(contains(ext, "GL_ARB_texture_filter_minmax")),
			ARB_texture_float(contains(ext, "GL_ARB_texture_float")),
			ARB_texture_gather(contains(ext, "GL_ARB_texture_gather")),
			ARB_texture_mirror_clamp_to_edge(contains(ext, "GL_ARB_texture_mirror_clamp_to_edge")),
			ARB_texture_mirrored_repeat(contains(ext, "GL_ARB_texture_mirrored_repeat")),
			ARB_texture_multisample(contains(ext, "GL_ARB_texture_multisample")),
			ARB_texture_non_power_of_two(contains(ext, "GL_ARB_texture_non_power_of_two")),
			ARB_texture_query_levels(contains(ext, "GL_ARB_texture_query_levels")),
			ARB_texture_query_lod(contains(ext, "GL_ARB_texture_query_lod")),
			ARB_texture_rectangle(contains(ext, "GL_ARB_texture_rectangle")),
			ARB_texture_rg(contains(ext, "GL_ARB_texture_rg")),
			ARB_texture_rgb10_a2ui(contains(ext, "GL_ARB_texture_rgb10_a2ui")),
			ARB_texture_stencil8(contains(ext, "GL_ARB_texture_stencil8")),
			ARB_texture_storage(contains(ext, "GL_ARB_texture_storage")),
			ARB_texture_storage_multisample(contains(ext, "GL_ARB_texture_storage_multisample")),
			ARB_texture_swizzle(contains(ext, "GL_ARB_texture_swizzle")),
			ARB_texture_view(contains(ext, "GL_ARB_texture_view")),
			ARB_timer_query(contains(ext, "GL_ARB_timer_query")),
			ARB_transform_feedback2(contains(ext, "GL_ARB_transform_feedback2")),
			ARB_transform_feedback3(contains(ext, "GL_ARB_transform_feedback3")),
			ARB_transform_feedback_instanced(contains(ext, "GL_ARB_transform_feedback_instanced")),
			ARB_transform_feedback_overflow_query(contains(ext, "GL_ARB_transform_feedback_overflow_query")),
			ARB_transpose_matrix(contains(ext, "GL_ARB_transpose_matrix")),
			ARB_uniform_buffer_object(contains(ext, "GL_ARB_uniform_buffer_object")),
			ARB_vertex_array_bgra(contains(ext, "GL_ARB_vertex_array_bgra")),
			ARB_vertex_array_object(contains(ext, "GL_ARB_vertex_array_object")),
			ARB_vertex_attrib_64bit(contains(ext, "GL_ARB_vertex_attrib_64bit")),
			ARB_vertex_attrib_binding(contains(ext, "GL_ARB_vertex_attrib_binding")),
			ARB_vertex_blend(contains(ext, "GL_ARB_vertex_blend")),
			ARB_vertex_buffer_object(contains(ext, "GL_ARB_vertex_buffer_object")),
			ARB_vertex_program(contains(ext, "GL_ARB_vertex_program")),
			ARB_vertex_shader(contains(ext, "GL_ARB_vertex_shader")),
			ARB_vertex_type_10f_11f_11f_rev(contains(ext, "GL_ARB_vertex_type_10f_11f_11f_rev")),
			ARB_vertex_type_2_10_10_10_rev(contains(ext, "GL_ARB_vertex_type_2_10_10_10_rev")),
			ARB_viewport_array(contains(ext, "GL_ARB_viewport_array")),
			ARB_window_pos(contains(ext, "GL_ARB_window_pos")),
			ATI_meminfo(contains(ext, "GL_ATI_meminfo")),
			ATI_shader_texture_lod(contains(ext, "GL_ATI_shader_texture_lod")),
			ATI_texture_compression_3dc(contains(ext, "GL_ATI_texture_compression_3dc")),
			EXT_422_pixels(contains(ext, "GL_EXT_422_pixels")),
			EXT_abgr(contains(ext, "GL_EXT_abgr")),
			EXT_bgra(contains(ext, "GL_EXT_bgra")),
			EXT_bindable_uniform(contains(ext, "GL_EXT_bindable_uniform")),
			EXT_blend_color(contains(ext, "GL_EXT_blend_color")),
			EXT_blend_equation_separate(contains(ext, "GL_EXT_blend_equation_separate")),
			EXT_blend_func_separate(contains(ext, "GL_EXT_blend_func_separate")),
			EXT_blend_minmax(contains(ext, "GL_EXT_blend_minmax")),
			EXT_blend_subtract(contains(ext, "GL_EXT_blend_subtract")),
			EXT_clip_volume_hint(contains(ext, "GL_EXT_clip_volume_hint")),
			EXT_compiled_vertex_array(contains(ext, "GL_EXT_compiled_vertex_array")),
			EXT_debug_label(contains(ext, "GL_EXT_debug_label")),
			EXT_debug_marker(contains(ext, "GL_EXT_debug_marker")),
			EXT_depth_bounds_test(contains(ext, "GL_EXT_depth_bounds_test")),
			EXT_direct_state_access(contains(ext, "GL_EXT_direct_state_access")),
			EXT_draw_buffers2(contains(ext, "GL_EXT_draw_buffers2")),
			EXT_draw_instanced(contains(ext, "GL_EXT_draw_instanced")),
			EXT_framebuffer_blit(contains(ext, "GL_EXT_framebuffer_blit")),
			EXT_framebuffer_multisample(contains(ext, "GL_EXT_framebuffer_multisample")),
			EXT_framebuffer_multisample_blit_scaled(contains(ext, "GL_EXT_framebuffer_multisample_blit_scaled")),
			EXT_framebuffer_object(contains(ext, "GL_EXT_framebuffer_object")),
			EXT_framebuffer_sRGB(contains(ext, "GL_EXT_framebuffer_sRGB")),
			EXT_geometry_shader4(contains(ext, "GL_EXT_geometry_shader4")),
			EXT_gpu_program_parameters(contains(ext, "GL_EXT_gpu_program_parameters")),
			EXT_gpu_shader4(contains(ext, "GL_EXT_gpu_shader4")),
			EXT_packed_depth_stencil(contains(ext, "GL_EXT_packed_depth_stencil")),
			EXT_packed_float(contains(ext, "GL_EXT_packed_float")),
			EXT_pixel_buffer_object(contains(ext, "GL_EXT_pixel_buffer_object")),
			EXT_point_parameters(contains(ext, "GL_EXT_point_parameters")),
			EXT_polygon_offset_clamp(contains(ext, "GL_EXT_polygon_offset_clamp")),
			EXT_post_depth_coverage(contains(ext, "GL_EXT_post_depth_coverage")),
			EXT_provoking_vertex(contains(ext, "GL_EXT_provoking_vertex")),
			EXT_raster_multisample(contains(ext, "GL_EXT_raster_multisample")),
			EXT_secondary_color(contains(ext, "GL_EXT_secondary_color")),
			EXT_separate_shader_objects(contains(ext, "GL_EXT_separate_shader_objects")),
			EXT_shader_image_load_formatted(contains(ext, "GL_EXT_shader_image_load_formatted")),
			EXT_shader_image_load_store(contains(ext, "GL_EXT_shader_image_load_store")),
			EXT_shader_integer_mix(contains(ext, "GL_EXT_shader_integer_mix")),
			EXT_shadow_funcs(contains(ext, "GL_EXT_shadow_funcs")),
			EXT_shared_texture_palette(contains(ext, "GL_EXT_shared_texture_palette")),
			EXT_sparse_texture2(contains(ext, "GL_EXT_sparse_texture2")),
			EXT_stencil_clear_tag(contains(ext, "GL_EXT_stencil_clear_tag")),
			EXT_stencil_two_side(contains(ext, "GL_EXT_stencil_two_side")),
			EXT_stencil_wrap(contains(ext, "GL_EXT_stencil_wrap")),
			EXT_texture_array(contains(ext, "GL_EXT_texture_array")),
			EXT_texture_buffer_object(contains(ext, "GL_EXT_texture_buffer_object")),
			EXT_texture_compression_latc(contains(ext, "GL_EXT_texture_compression_latc")),
			EXT_texture_compression_rgtc(contains(ext, "GL_EXT_texture_compression_rgtc")),
			EXT_texture_compression_s3tc(contains(ext, "GL_EXT_texture_compression_s3tc")),
			EXT_texture_filter_anisotropic(contains(ext, "GL_EXT_texture_filter_anisotropic")),
			EXT_texture_filter_minmax(contains(ext, "GL_EXT_texture_filter_minmax")),
			EXT_texture_integer(contains(ext, "GL_EXT_texture_integer")),
			EXT_texture_mirror_clamp(contains(ext, "GL_EXT_texture_mirror_clamp")),
			EXT_texture_shared_exponent(contains(ext, "GL_EXT_texture_shared_exponent")),
			EXT_texture_snorm(contains(ext, "GL_EXT_texture_snorm")),
			EXT_texture_sRGB(contains(ext, "GL_EXT_texture_sRGB")),
			EXT_texture_sRGB_decode(contains(ext, "GL_EXT_texture_sRGB_decode")),
			EXT_texture_swizzle(contains(ext, "GL_EXT_texture_swizzle")),
			EXT_timer_query(contains(ext, "GL_EXT_timer_query")),
			EXT_transform_feedback(contains(ext, "GL_EXT_transform_feedback")),
			EXT_vertex_array_bgra(contains(ext, "GL_EXT_vertex_array_bgra")),
			EXT_vertex_attrib_64bit(contains(ext, "GL_EXT_vertex_attrib_64bit")),
			EXT_window_rectangles(contains(ext, "GL_EXT_window_rectangles")),
			EXT_x11_sync_object(contains(ext, "GL_EXT_x11_sync_object")),
			INTEL_conservative_rasterization(contains(ext, "GL_INTEL_conservative_rasterization")),
			INTEL_fragment_shader_ordering(contains(ext, "GL_INTEL_fragment_shader_ordering")),
			INTEL_framebuffer_CMAA(contains(ext, "GL_INTEL_framebuffer_CMAA")),
			INTEL_map_texture(contains(ext, "GL_INTEL_map_texture")),
			INTEL_performance_query(contains(ext, "GL_INTEL_performance_query")),
			KHR_blend_equation_advanced(contains(ext, "GL_KHR_blend_equation_advanced")),
			KHR_blend_equation_advanced_coherent(contains(ext, "GL_KHR_blend_equation_advanced_coherent")),
			KHR_context_flush_control(contains(ext, "GL_KHR_context_flush_control")),
			KHR_debug(contains(ext, "GL_KHR_debug")),
			KHR_no_error(contains(ext, "GL_KHR_no_error")),
			KHR_robust_buffer_access_behavior(contains(ext, "GL_KHR_robust_buffer_access_behavior")),
			KHR_robustness(contains(ext, "GL_KHR_robustness")),
			KHR_texture_compression_astc_hdr(contains(ext, "GL_KHR_texture_compression_astc_hdr")),
			KHR_texture_compression_astc_ldr(contains(ext, "GL_KHR_texture_compression_astc_ldr")),
			KHR_texture_compression_astc_sliced_3d(contains(ext, "GL_KHR_texture_compression_astc_sliced_3d")),
			NV_bindless_multi_draw_indirect(contains(ext, "GL_NV_bindless_multi_draw_indirect")),
			NV_bindless_multi_draw_indirect_count(contains(ext, "GL_NV_bindless_multi_draw_indirect_count")),
			NV_bindless_texture(contains(ext, "GL_NV_bindless_texture")),
			NV_blend_equation_advanced(contains(ext, "GL_NV_blend_equation_advanced")),
			NV_blend_equation_advanced_coherent(contains(ext, "GL_NV_blend_equation_advanced_coherent")),
			NV_blend_square(contains(ext, "GL_NV_blend_square")),
			NV_clip_space_w_scaling(contains(ext, "GL_NV_clip_space_w_scaling")),
			NV_command_list(contains(ext, "GL_NV_command_list")),
			NV_conditional_render(contains(ext, "GL_NV_conditional_render")),
			NV_conservative_raster(contains(ext, "GL_NV_conservative_raster")),
			NV_conservative_raster_dilate(contains(ext, "GL_NV_conservative_raster_dilate")),
			NV_conservative_raster_pre_snap_triangles(contains(ext, "GL_NV_conservative_raster_pre_snap_triangles")),
			NV_copy_depth_to_color(contains(ext, "GL_NV_copy_depth_to_color")),
			NV_copy_image(contains(ext, "GL_NV_copy_image")),
			NV_deep_texture3D(contains(ext, "GL_NV_deep_texture3D")),
			NV_depth_buffer_float(contains(ext, "GL_NV_depth_buffer_float")),
			NV_depth_clamp(contains(ext, "GL_NV_depth_clamp")),
			NV_draw_texture(contains(ext, "GL_NV_draw_texture")),
			NV_draw_vulkan_image(contains(ext, "GL_NV_draw_vulkan_image")),
			NV_ES3_1_compatibility(contains(ext, "GL_NV_ES3_1_compatibility")),
			NV_explicit_multisample(contains(ext, "GL_NV_explicit_multisample")),
			NV_fence(contains(ext, "GL_NV_fence")),
			NV_fill_rectangle(contains(ext, "GL_NV_fill_rectangle")),
			NV_float_buffer(contains(ext, "GL_NV_float_buffer")),
			NV_fog_distance(contains(ext, "GL_NV_fog_distance")),
			NV_fragment_coverage_to_color(contains(ext, "GL_NV_fragment_coverage_to_color")),
			NV_fragment_program4(contains(ext, "GL_NV_fragment_program4")),
			NV_fragment_program_option(contains(ext, "GL_NV_fragment_program_option")),
			NV_fragment_shader_interlock(contains(ext, "GL_NV_fragment_shader_interlock")),
			NV_framebuffer_mixed_samples(contains(ext, "GL_NV_framebuffer_mixed_samples")),
			NV_framebuffer_multisample_coverage(contains(ext, "GL_NV_framebuffer_multisample_coverage")),
			NV_geometry_shader4(contains(ext, "GL_NV_geometry_shader4")),
			NV_geometry_shader_passthrough(contains(ext, "GL_NV_geometry_shader_passthrough")),
			NV_gpu_multicast(contains(ext, "GL_NV_gpu_multicast")),
			NV_gpu_shader5(contains(ext, "GL_NV_gpu_shader5")),
			NV_half_float(contains(ext, "GL_NV_half_float")),
			NV_internalformat_sample_query(contains(ext, "GL_NV_internalformat_sample_query")),
			NV_light_max_exponent(contains(ext, "GL_NV_light_max_exponent")),
			NV_multisample_coverage(contains(ext, "GL_NV_multisample_coverage")),
			NV_multisample_filter_hint(contains(ext, "GL_NV_multisample_filter_hint")),
			NV_packed_depth_stencil(contains(ext, "GL_NV_packed_depth_stencil")),
			NV_path_rendering(contains(ext, "GL_NV_path_rendering")),
			NV_path_rendering_shared_edge(contains(ext, "GL_NV_path_rendering_shared_edge")),
			NV_pixel_data_range(contains(ext, "GL_NV_pixel_data_range")),
			NV_point_sprite(contains(ext, "GL_NV_point_sprite")),
			NV_primitive_restart(contains(ext, "GL_NV_primitive_restart")),
			NV_robustness_video_memory_purge(contains(ext, "GL_NV_robustness_video_memory_purge")),
			NV_sample_locations(contains(ext, "GL_NV_sample_locations")),
			NV_sample_mask_override_coverage(contains(ext, "GL_NV_sample_mask_override_coverage")),
			NV_shader_atomic_float(contains(ext, "GL_NV_shader_atomic_float")),
			NV_shader_atomic_float64(contains(ext, "GL_NV_shader_atomic_float64")),
			NV_shader_atomic_fp16_vector(contains(ext, "GL_NV_shader_atomic_fp16_vector")),
			NV_shader_atomic_int64(contains(ext, "GL_NV_shader_atomic_int64")),
			NV_shader_buffer_load(contains(ext, "GL_NV_shader_buffer_load")),
			NV_shader_buffer_store(contains(ext, "GL_NV_shader_buffer_store")),
			NV_shader_thread_group(contains(ext, "GL_NV_shader_thread_group")),
			NV_shader_thread_shuffle(contains(ext, "GL_NV_shader_thread_shuffle")),
			NV_stereo_view_rendering(contains(ext, "GL_NV_stereo_view_rendering")),
			NV_texgen_reflection(contains(ext, "GL_NV_texgen_reflection")),
			NV_texture_barrier(contains(ext, "GL_NV_texture_barrier")),
			NV_texture_compression_vtc(contains(ext, "GL_NV_texture_compression_vtc")),
			NV_texture_multisample(contains(ext, "GL_NV_texture_multisample")),
			NV_transform_feedback(contains(ext, "GL_NV_transform_feedback")),
			NV_transform_feedback2(contains(ext, "GL_NV_transform_feedback2")),
			NV_uniform_buffer_unified_memory(contains(ext, "GL_NV_uniform_buffer_unified_memory")),
			NV_vertex_array_range(contains(ext, "GL_NV_vertex_array_range")),
			NV_vertex_array_range2(contains(ext, "GL_NV_vertex_array_range2")),
			NV_vertex_attrib_integer_64bit(contains(ext, "GL_NV_vertex_attrib_integer_64bit")),
			NV_vertex_buffer_unified_memory(contains(ext, "GL_NV_vertex_buffer_unified_memory")),
			NV_viewport_array2(contains(ext, "GL_NV_viewport_array2")),
			NV_viewport_swizzle(contains(ext, "GL_NV_viewport_swizzle")),
			NVX_blend_equation_advanced_multi_draw_buffers(contains(ext, "GL_NVX_blend_equation_advanced_multi_draw_buffers")),
			NVX_conditional_render(contains(ext, "GL_NVX_conditional_render")),
			NVX_gpu_memory_info(contains(ext, "GL_NVX_gpu_memory_info")),
			OVR_multiview(contains(ext, "GL_OVR_multiview")),
			OVR_multiview2(contains(ext, "GL_OVR_multiview2"))
		{
		
#if defined(_DEBUG)
			std::cout << "GLCapabilities init!" << std::endl;
#endif
		}

		static int split(std::set<std::string>& out, char* str, const char* spl)
		{
			int n = 0;
			char *result = NULL;
			if (str == NULL)
			{
				return n;
			}
			result = strtok(str, spl);
			while (result != NULL)
			{
//				strcpy(dst[n++], result);
				out.insert(result);
				result = strtok(NULL, spl);
			}
			return n;
		}

#include <GL\glew.h>

		typedef struct _GLVersion
		{
			int mojar;
			int minor;
			// Indicate the Open ES platform If true.
			bool es;

			_GLVersion() :mojar(0), minor(0), es(false){}
		}_GLVersion;

		static void ToUppercase(char* pDst, const char* pIn, int length = -1)
		{
			if (length < 0)
			{
				length = static_cast<int>(std::min(strlen(pIn), strlen(pDst)));
			}

			const char* const pEnd = pIn + length;

			while (pIn < pEnd)
			{
				if (*pIn >= 'a' && *pIn <= 'z')
				{
					*pDst = *pIn - 32;
				}
				else
				{
					*pDst = *pIn;
				}

				pIn++;
				pDst++;
			}
		}

		static void ParseGLVersionString(_GLVersion* pOut, const char* versionString)
		{
			size_t length = strlen(versionString);
			char *pUpperVersionString = (char *)malloc(length + 1);
			ToUppercase(pUpperVersionString, versionString);
			pUpperVersionString[length] = '\0';

			char *p = (char *)pUpperVersionString;
			char *end = p + length;

			const char* opengl_tag = "OPENGL";
			size_t opengl_tag_length = strlen(opengl_tag);

			const char* es_tag = "ES";
			pOut->es = false;

			while (p < end)
			{
				size_t n = strcspn(p, " ");
				if ((opengl_tag_length == n) && strncmp(opengl_tag, p, n) == 0)
				{
					p += (n + 1);
					n = strcspn(p, " ");

					if (n == 2 && strncmp(es_tag, p, 2) == 0)
					{
						p += 3;
						pOut->es = true;
					}
				}
				else if (isdigit(*p))
				{
					size_t next_slot = strcspn(p, " ");
					int idx = 0;
					while (idx < 2 && (n = strcspn(p, ". ")) < next_slot)
					{
						((int*)pOut)[idx++] = atoi(p);
						p += (n + 1);
					}

					break;
				}
			}

			free(pUpperVersionString);
		}

		static GLCapabilities* g_GLCaps = nullptr;
		GLCapabilities* GLCapabilities::getGLCapabilities()
		{
			if (g_GLCaps == nullptr)
			{
				std::set<std::string> ext;
				bool forwardCompatible;
				if (glGetStringi)
				{
					int extCount;
					glGetIntegerv(GL_NUM_EXTENSIONS, &extCount);
					for (int i = 0; i < extCount; i++)
					{
						char* _ext = (char*)glGetStringi(GL_EXTENSIONS, i);
						ext.insert(_ext);
					}
				}
				else
				{
					char* _ext = (char*)glGetString(GL_EXTENSIONS);
					split(ext, _ext, " ");
				}

				_GLVersion version;
				ParseGLVersionString(&version, (const char*)glGetString(GL_VERSION));

				char openglStr[12];
				for (int i = 1; i < version.mojar; i++)
				{
					for (int j = 1; j < version.minor; j++)
					{
						sprintf_s(openglStr, "OpenGL%d%d", i, j);
						ext.insert(openglStr);
					}
				}
				if (version.mojar < 3)  // less than opengl 3.0
				{
					forwardCompatible = false;
				}
				else
				{
					int context_flags;
					glGetIntegerv(GL_CONTEXT_FLAGS, &context_flags);
					if (context_flags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
					{
						forwardCompatible = true;
					}
					else
					{
						if (version.minor >= 1)  // OpenGL 3.1
						{
							if (version.minor >= 2)  // OpenGL 3.2
							{
								int context_mask;
								glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &context_mask);
								if (context_mask & GL_CONTEXT_CORE_PROFILE_BIT)
								{
									forwardCompatible = true;
								}
							}
						}
						else
						{
							forwardCompatible = !contains(ext, "GL_ARB_compatibility");
						}
					}
				}

				

				g_GLCaps = new GLCapabilities(ext, forwardCompatible);
			}

			return g_GLCaps;
		}

		void GLCapabilities::release()
		{
			if (g_GLCaps)
			{
				delete g_GLCaps;
				g_GLCaps = nullptr;
			}
		}
	}
}
