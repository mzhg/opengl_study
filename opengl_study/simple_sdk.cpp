#include "simple_sdk_common.h"

#ifdef __cplusplus
extern "C"{
#endif

	static const char* g_pVertStr =
		"uniform mat4 g_Mat;\n"
		"attribute vec4  g_Position;\n"
		"attribute vec2  g_TextureCoord;\n"
		"varying   vec2  vTextureCoord;\n"

		"void main(){\n"
		"vTextureCoord = g_TextureCoord;\n"
		"gl_Position = g_Mat*g_Position;\n"
		"}\n";

	static const char* g_pFragStr =
		"#if GL_ES\n"
		"precision mediump float;\n"
		"#endif\n"

		"varying vec2  vTextureCoord;\n"
		"uniform sampler2D sparrow;\n"

		"void main(){\n"
		"	gl_FragColor = texture2D(sparrow, vTextureCoord);\n"
		"}\n";

	static GLuint g_program = 0;
	static TextureGL g_background_left_tex_id;
	static TextureGL g_background_right_tex_id;
	static TextureGL g_rect_texture_id;

	static GLuint g_sphere_vbo = 0;  // sphere vertex buffer
	static GLuint g_sphere_ibo = 0;  // sphere indices buffer. 
	static GLuint g_sphere_indices_count = 0;
	static GLuint g_rect_vbo = 0;
	static GLuint g_rect_ibo = 0;
	static GLuint g_billboard_vbo = 0;

	static GLint g_mat_loc = -1;

	static Rect g_rect_bound;
	static Billboard g_billboard_info;
	static Size g_window_size;
	static Projection g_projection;
	static SimpleFBO* g_fbo_left = NULL;
	static SimpleFBO* g_fbo_right = NULL;
	static TextureGL  g_fbo_left_tex;
	static TextureGL  g_fbo_right_tex;
	static TextureGL  g_tag_normal_tex;
	static TextureGL  g_tag_focus_tex;

	static Matrix4 g_background_rotate2;
	static RenderParams g_render_params;

	static void create_sphere_vertexs(int, int);
	static void create_rect_vertices(GLuint& buffer, GLenum usage);
	static void create_program();
	static Vertex3 setVertex(float, float, float);

	extern "C" void update_billboard_buffer(Vertex* pData, Matrix4& rotate, const Billboard& info, int width, int height);
	extern "C" void create_tag_image(TextureGL& tex, const Vertex3& color, unsigned size, float t);

	void ogl_init()
	{
		create_sphere_vertexs(50,20);
		create_rect_vertices(g_rect_vbo, GL_STATIC_DRAW);
		create_rect_vertices(g_billboard_vbo, GL_DYNAMIC_DRAW);
		create_program();

		g_background_rotate = setVertex(0, 0, 0);
		g_projection.far = 100.0f;
		g_projection.near = 0.1f;
		g_projection.fov = 60.0f;

		g_background_left_tex_id = empty_texture();
		g_background_right_tex_id = empty_texture();
		g_rect_texture_id = empty_texture();

		g_tag_normal_tex = empty_texture();
		g_tag_focus_tex = empty_texture();

		create_tag_image(g_tag_normal_tex, setVertex(1.0f, 1.0f, 1.0f), TAG_TEX_SIZE, 0.5f);
		create_tag_image(g_tag_focus_tex,  setVertex(1.0f, 0.2f, 0.1f), TAG_TEX_SIZE, 0.1f);

		g_background_rotate2 = identity();

		g_billboard_info.depth = 50.0f;
		g_billboard_info.pitch = 0.0f;
		g_billboard_info.roll = 0.0f;
		g_billboard_info.scale = 0.03f;
		g_billboard_info.yaw = 0.0f;
	}

	void ogl_resize(int width, int height)
	{
		g_window_size.width = width;
		g_window_size.height = height;
	}

	void ogl_set_projection(float fov, float near, float far)
	{
		g_projection.far = far;
		g_projection.near = near;
		g_projection.fov = fov;
	}

	void ogl_read_texels_from_renderbuffer()
	{
		if (g_rect_texture_id.textureId == 0 || g_rect_texture_id.width != g_window_size.width || g_rect_texture_id.height != g_window_size.height)
		{
			release_texture(g_rect_texture_id);
			create_texture_internal(g_rect_texture_id, g_window_size.width, g_window_size.height, GL_RGBA, (const char*)0);
			AND_LOG("Create rect texture from renderbuffer.");
		}

		GL(glBindTexture(GL_TEXTURE_2D, g_rect_texture_id.textureId));
		GL(glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, g_window_size.width, g_window_size.height));
		GL(glBindTexture(GL_TEXTURE_2D, 0));
	}

	void ogl_set_background_texture(int width, int height, int format, const char* pData, bool left)
	{
		TextureGL& g_background_tex_id = left ? g_background_left_tex_id : g_background_right_tex_id;
		create_texture_internal(g_background_tex_id, width, height, format, pData);
		AND_LOG("Create background texture from pixel data.");
	}

	void ogl_set_background_rotation(float yaw, float pitch, float roll)
	{
		g_background_rotate = setVertex(pitch, yaw, roll);
	}

	void ogl_update(ScreenResult& out, float* mat)
	{
		memcpy(&g_background_rotate2, mat, sizeof(Matrix4));
		Matrix4 rotation = rotationYawPitchRoll(g_background_rotate.y, g_background_rotate.x, g_background_rotate.z);
		rotation = mulMat(rotation, g_background_rotate2);

		g_billboard_info.depth = ogl_max(g_billboard_info.depth, g_projection.near + 1.0f);
		g_billboard_info.depth = ogl_min(g_billboard_info.depth, g_projection.far - 1.0f);

		Size window_size;
		if (g_window_size.height > g_window_size.width)
		{
			window_size.width = g_window_size.width;
			window_size.height = g_window_size.height / 2;
		}
		else
		{
			window_size.width = g_window_size.width/2;
			window_size.height = g_window_size.height ;
		}
		update_billboard(g_render_params, out, window_size, (float)g_window_size.width / (float)g_window_size.height,
			g_projection, rotation, g_billboard_info);
	}

	void ogl_set_rect_texture(int width, int height, int format, const char* pData)
	{
		create_texture_internal(g_rect_texture_id, width, height, format, pData);
		AND_LOG("Create rect texture from pixel data.");
	}

	void ogl_set_background_texture_id(unsigned int id, bool left)
	{
		TextureGL& g_background_tex_id = left ? g_background_left_tex_id : g_background_right_tex_id;
		release_texture(g_background_tex_id);
		g_background_tex_id.textureId = id;
		g_background_tex_id.owend = 0;

		AND_LOG("Create background texture with the given texture id.");
	}

	void ogl_set_rect_texture_id(unsigned int id)
	{
		release_texture(g_rect_texture_id);
		g_rect_texture_id.textureId = id;
		g_rect_texture_id.owend = 0;

		bool isValid = glIsTexture(id);
		if (!isValid)
		{
			AND_LOG("ogl_set_rect_texture_id::Invalid texture id. \n");
		}
		AND_LOG("Create rect texture with the given texture id.\n");
	}

	static Vertex3 bilinear(const Vertex3& x, const Vertex3& y, const Vertex3& z, const Vertex3& w, float a, float b){
		const float _a = 1 - a;
		const float _b = 1 - b;
		Vertex3 dest;

		dest.x = _a * _b * x.x + a * _b * y.x + a * b * z.x + _a * b * w.x;
		dest.y = _a * _b * x.y + a * _b * y.y + a * b * z.y + _a * b * w.y;
		dest.z = _a * _b * x.z + a * _b * y.z + a * b * z.z + _a * b * w.z;

		return dest;
	}

	void ogl_create_background_default_texture(int width, int height, bool left)
	{
		struct RGB
		{
			unsigned char r, g, b;
		};

		TextureGL& g_background_tex_id = left ? g_background_left_tex_id : g_background_right_tex_id;
		release_texture(g_background_tex_id);
		RGB* pData = (RGB*)malloc(sizeof(RGB) * width * height);

		const Vertex3 left_colors[] =
		{
			{1,0,0},
			{0,1,0},
			{0,0,1},
			{1,1,1}
		};

		const Vertex3 right_colors[] =
		{
			{ 1, 1, 0 },
			{ 0, 1, 1 },
			{ 1, 0, 1 },
			{ 1, 1, 1 }
		};

		Vertex3 red, green, blue, white;
		if (left)
		{
			red = left_colors[0];
			green = left_colors[1];
			blue = left_colors[2];
			white = left_colors[3];
		}
		else
		{
			red = right_colors[0];
			green = right_colors[1];
			blue = right_colors[2];
			white = right_colors[3];
		}
		
		for (int i = 0; i < width; i++)
		{
			for (int j = 0; j < height; j++)
			{
				float a = (float)i / (float)(width - 1);
				float b = (float)j / (float)(height - 1);

				int idx = j * width + i;
				Vertex3 color =  bilinear(red, green, blue, white, a, b);
				RGB& rgbColor = pData[idx];
				rgbColor.r = (unsigned char)(color.x * 255);
				rgbColor.g = (unsigned char)(color.y * 255);
				rgbColor.b = (unsigned char)(color.z * 255);
			}
		}

		ogl_set_background_texture(width, height, GL_RGB, (const char*)pData, left);
		free(pData);
	}

	void ogl_set_rect_size(int x, int y, int width, int height)
	{
		g_rect_bound.x = x;
		g_rect_bound.y = y;
		g_rect_bound.width = width;
		g_rect_bound.height = height;

		AND_LOG("set rect bound[x = %d, y = %d, width = %d, height = %d].\n", x,y,width, height);
	}

	void ogl_set_rect_location(float scale, float depth, float yaw, float pitch, float roll)
	{
		g_billboard_info.depth = depth;
		g_billboard_info.scale = scale;
		g_billboard_info.yaw = yaw;
		g_billboard_info.pitch = pitch;
		g_billboard_info.roll = roll;
	}

	typedef struct AttribDesc
	{
		GLint enabled;
		GLint size;
		GLint stride;
		GLint type;
		GLint normalized;
		GLvoid* pointer;
	}AttribDesc;

#if OGL_DEBUG
	static void ogl_print_attrib_info(int index, const AttribDesc& info)
	{
		char* type_str = "Unkown";
		switch (info.type)
		{
		case GL_BYTE: 
			type_str = "GL_BYTE";
			break;
		case GL_UNSIGNED_BYTE:
			type_str = "GL_UNSIGNED_BYTE";
			break;
		case GL_SHORT:
			type_str = "GL_SHORT";
			break;
		case GL_UNSIGNED_SHORT:
			type_str = "GL_UNSIGNED_SHORT";
			break;
		case GL_FIXED:
			type_str = "GL_FIXED";
			break;
		case GL_FLOAT:
			type_str = "GL_FLOAT";
			break;
		}

		AND_LOG("Attrib[%d]: (enabled, size, stride, type, normalized, pointer) = [%s, %d, %d, %s, %s, %d].\n",
			index,
			(info.enabled ? "true":"false"), info.size, info.stride, type_str, 
			(info.normalized ? "true" : "false"), (GLint)(info.pointer)
			);
	}
#endif
	static void ogl_get_attrib_info(int index, AttribDesc& out)
	{
		glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_ENABLED, &out.enabled);
		glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_SIZE, &out.size);
		glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_STRIDE, &out.stride);
		glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_TYPE, &out.type);
		glGetVertexAttribiv(index, GL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &out.normalized);
		glGetVertexAttribPointerv(index, GL_VERTEX_ATTRIB_ARRAY_POINTER, &out.pointer);
	}

	static void render_to_framebuffer_impl(bool left)
	{
		SimpleFBO* g_fbo;
		TextureGL  g_background_tex_id;

		if (left)
		{
			g_fbo = g_fbo_left;
			g_background_tex_id = g_background_left_tex_id;
		}
		else
		{
			g_fbo = g_fbo_right;
			g_background_tex_id = g_background_right_tex_id;
		}

#if 0
		Matrix4 proj = perspective(g_projection.fov, (float)window_width / (float)window_height, g_projection.near, g_projection.far);
		Matrix4 rotation = rotationYawPitchRoll(g_background_rotate.y, g_background_rotate.x, g_background_rotate.z);
		rotation = mulMat(rotation, g_background_rotate2);
		Matrix4 mvp = mulMat(proj, rotation);
#endif
		g_fbo->Begin();
		if (left)
		{
			glClearColor(0.7f, 0.1f, 0.12f, 1.0f);
		}
		else
		{
			glClearColor(0.1f, 0.7f, 0.12f, 1.0f);
		}
		glClear(GL_COLOR_BUFFER_BIT);

		GL(glUseProgram(g_program));
		{
			// render the sphere
			GL(glUniformMatrix4fv(g_mat_loc, 1, GL_FALSE, (const GLfloat*)&g_render_params.sceneMVP));

			GL(glActiveTexture(GL_TEXTURE0));
			GL(glBindTexture(GL_TEXTURE_2D, g_background_tex_id.textureId));

			GL(glBindBuffer(GL_ARRAY_BUFFER, g_sphere_vbo));
			GL(glEnableVertexAttribArray(0));
			GL(glEnableVertexAttribArray(1));

			GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
			GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)sizeof(Vertex3)));

			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_sphere_ibo));
			GL(glDrawElements(GL_TRIANGLES, g_sphere_indices_count, GL_UNSIGNED_SHORT, 0));
		}

		{
#if 0
			Matrix4 billboard_rot_mat;
			Vertex billboard_data[4];


			update_billboard_buffer(billboard_data, billboard_rot_mat, g_billboard_info, window_width * 0.3f, window_height * 0.3f);
			Matrix4 _mvp = mulMat(mvp, billboard_rot_mat);
#endif
			GL(glUniformMatrix4fv(g_mat_loc, 1, GL_FALSE, (const GLfloat*)&g_render_params.billboardMVP));

			GL(glActiveTexture(GL_TEXTURE0));
			GL(glBindTexture(GL_TEXTURE_2D, g_rect_texture_id.textureId));
			GL(glBindBuffer(GL_ARRAY_BUFFER, g_billboard_vbo));
			GL(glBufferSubData(GL_ARRAY_BUFFER, 0, 4 * sizeof(Vertex), g_render_params.billBoardData));
			GL(glEnableVertexAttribArray(0));
			GL(glEnableVertexAttribArray(1));

			GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
			GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)sizeof(Vertex3)));

			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
			GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

			// render the tag image to screen center
			// render the rect
			GL(glEnable(GL_BLEND));
			GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
#if 0
			Matrix4 orthMat = ortho(0, +window_width, 0, +window_height,
				-1.0f, 1.0f);

			float img_x = (window_width - TAG_TEX_SIZE) / 2;
			float img_y = (window_height - TAG_TEX_SIZE) / 2;

			Matrix4 viewMat = identity();
			viewMat.m00 = TAG_TEX_SIZE * 0.5f;
			viewMat.m11 = TAG_TEX_SIZE * 0.5f;
			viewMat.m30 = img_x + viewMat.m00;
			viewMat.m31 = img_y + viewMat.m11;

			bool intersect;
			Vertex3 camera_pos;
			Vertex3 xAxis;
			Vertex3 yAxis;
			Vertex3 zAxis;
			Matrix4 modelView = mulMat(rotation, billboard_rot_mat);
			decompseRigidMatrix(modelView, camera_pos, xAxis, yAxis, zAxis);
			zAxis.x *= -1.0f;
			zAxis.y *= -1.0f;
			zAxis.z *= -1.0f;

			float t, u, v;
			intersect = ogl_intersect_triangle(camera_pos, zAxis, billboard_data[0].position, billboard_data[1].position, billboard_data[2].position, &t, &u, &v);
			if (!intersect)
			{
				intersect = ogl_intersect_triangle(camera_pos, zAxis, billboard_data[1].position, billboard_data[2].position, billboard_data[3].position, &t, &u, &v);
			}

			intersect &= (t > 0.0f);
			if (intersect)
			{
				//				AND_LOG("camera_pos = (%f, %f, %f), dir = (%f, %f, %f), t = %f.\n", camera_pos.x, camera_pos.y, camera_pos.z, zAxis.x, zAxis.y, zAxis.z, t);
				Vertex3 point;
				point.x = camera_pos.x + zAxis.x * t;
				point.y = camera_pos.y + zAxis.y * t;
				point.z = camera_pos.z + zAxis.z * t;
				Vertex3 diff = point - billboard_data[0].position;
				float hw = window_width * 0.3f * g_billboard_info.scale;
				float hh = window_height * 0.3f * g_billboard_info.scale;
				float screenX = diff.x / hw *  window_width;
				float screenY = window_height - diff.y / hh * window_height;

				printf("ScreenPos = (%f, %f).\n", screenX, screenY);
			}

			mvp = mulMat(orthMat, viewMat);
#endif
			GL(glUniformMatrix4fv(g_mat_loc, 1, GL_FALSE, (const GLfloat*)&g_render_params.tagMVP));
			GL(glBindTexture(GL_TEXTURE_2D, g_render_params.result.intersected ? g_tag_focus_tex.textureId : g_tag_normal_tex.textureId));
			GL(glBindBuffer(GL_ARRAY_BUFFER, g_rect_vbo));
			GL(glEnableVertexAttribArray(0));
			GL(glEnableVertexAttribArray(1));

			GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
			GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)sizeof(Vertex3)));

			GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

			GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
			GL(glDisableVertexAttribArray(0));
			GL(glDisableVertexAttribArray(1));
			GL(glDisable(GL_BLEND));
		}

		g_fbo->End();
	}

	static void render_to_framebuffer()
	{
//		const int window_width = ogl_max(g_window_size.width, g_window_size.height)/2;
//		const int window_height = ogl_min(g_window_size.width, g_window_size.height);

		int window_width, window_height;
		if (g_window_size.height > g_window_size.width)
		{
			window_width = g_window_size.width;
			window_height = g_window_size.height / 2;
		}
		else
		{
			window_width = g_window_size.width / 2;
			window_height = g_window_size.height;
		}

		if (g_fbo_left == NULL)
		{
			g_fbo_left_tex = empty_texture();
			create_texture_internal(g_fbo_left_tex, window_width, window_height, GL_RGBA, NULL);
			g_fbo_left = new SimpleFBO(&g_fbo_left_tex);
		}

		if (g_fbo_right == NULL)
		{
			g_fbo_right_tex = empty_texture();
			create_texture_internal(g_fbo_right_tex, window_width, window_height, GL_RGBA, NULL);
			g_fbo_right = new SimpleFBO(&g_fbo_right_tex);
		}

		GL(glViewport(0, 0, window_width, window_height));
		render_to_framebuffer_impl(true);
		render_to_framebuffer_impl(false);
	}

	static void render_fbo_to_split_screen()
	{
//		const int window_width = ogl_max(g_window_size.width, g_window_size.height) / 2;
//		const int window_height = ogl_min(g_window_size.width, g_window_size.height);

		int window_width, window_height;
		if (g_window_size.height > g_window_size.width)
		{
			window_width = g_window_size.width;
			window_height = g_window_size.height / 2;
		}
		else
		{
			window_width = g_window_size.width / 2;
			window_height = g_window_size.height;
		}

		Matrix4 orthMat = ortho(0, +window_width, 0, +window_height, -1.0f, 1.0f);
		GL(glUseProgram(g_program));
		{
			// render the fbo to left screen
#if 0
			Rect left_region = { 0, window_height/4, window_width / 2, window_height/2 };
			Matrix4 viewMat = identity();
			viewMat.m00 = left_region.width * 0.5f;
			viewMat.m11 = left_region.height * 0.5f;
			viewMat.m30 = left_region.x + viewMat.m00;
			viewMat.m31 = left_region.y + viewMat.m11;

			Matrix4 mvp = mulMat(orthMat, viewMat);
#endif
			if (g_window_size.width > g_window_size.height)
			{
				GL(glViewport(0, 0, window_width, window_height));
			}
			else
			{
				GL(glViewport(0, 0, window_height, window_width));
			}
			GL(glViewport(0, 0, window_width, window_height));
			glClearColor(0.7f, 0.1f, 0.12f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);
			
			GL(glUniformMatrix4fv(g_mat_loc, 1, GL_FALSE, (const GLfloat*)&g_render_params.leftSplitMVP));

			GL(glActiveTexture(GL_TEXTURE0));
			GL(glBindTexture(GL_TEXTURE_2D, g_fbo_left_tex.textureId));
			GL(glBindBuffer(GL_ARRAY_BUFFER, g_rect_vbo));
			GL(glEnableVertexAttribArray(0));
			GL(glEnableVertexAttribArray(1));

			GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0));
			GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)sizeof(Vertex3)));

			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
			GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));		
		}

		{
			// render the fbo to right screen
#if 0
			Rect right_region = { window_width / 2, window_height / 4, window_width / 2, window_height/2 };
			Matrix4 viewMat = identity();
			viewMat.m00 = right_region.width * 0.5f;
			viewMat.m11 = right_region.height * 0.5f;
			viewMat.m30 = right_region.x + viewMat.m00;
			viewMat.m31 = right_region.y + viewMat.m11;

			Matrix4 mvp = mulMat(orthMat, viewMat);

#endif
			if (g_window_size.width > g_window_size.height)
			{
				GL(glViewport(window_width, 0, window_width, window_height));
			}
			else
			{
				GL(glViewport(0, window_height, window_width, window_height));
			}

//			glClearColor(0.1f, 0.7f, 0.12f, 1.0f);
//			glClear(GL_COLOR_BUFFER_BIT);
			GL(glBindTexture(GL_TEXTURE_2D, g_fbo_right_tex.textureId));
			GL(glUniformMatrix4fv(g_mat_loc, 1, GL_FALSE, (const GLfloat*)&g_render_params.rightSplitMVP));
			GL(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));
			GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
			GL(glDisableVertexAttribArray(0));
			GL(glDisableVertexAttribArray(1));
		}
	}

	void ogl_render(float elpsedTime)
	{
		/*
		GL(glEnable(GL_DEPTH_TEST));
		GL(glDepthFunc(GL_ALWAYS));
		GL((GL_FALSE));
		GL(glDisable(GL_CULL_FACE));
		GL((GL_BLEND));
		GL(glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE));
		*/

		GLint program;
		glGetIntegerv(GL_CURRENT_PROGRAM, &program);

		GLint activeTexture;
		glGetIntegerv(GL_ACTIVE_TEXTURE, &activeTexture);

		GLint vertex_buffer, element_buffer;
		glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vertex_buffer);
		glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &element_buffer);

		AttribDesc attrib_slot0, attrib_slot1;
		ogl_get_attrib_info(0, attrib_slot0);
		ogl_get_attrib_info(1, attrib_slot1);
		
#if OGL_DEBUG
		int viewports[4];
		glGetIntegerv(GL_VIEWPORT, viewports);

		GLint framebuffer;
		glGetIntegerv(0x8CA6, &framebuffer);

		GLboolean blend;
		glGetBooleanv(GL_BLEND, &blend);

		ogl_print_attrib_info(0, attrib_slot0);
		ogl_print_attrib_info(1, attrib_slot1);

		AND_LOG("The current Viewport = [%d, %d, %d, %d].\n", viewports[0], viewports[1], viewports[2], viewports[3]);
		AND_LOG("The current framebuffer name = %d.\n", framebuffer);
		AND_LOG("The current program name = %d.\n", program);
		AND_LOG("The current vertex_buffer name = %d.\n", vertex_buffer);
		AND_LOG("The current element_buffer name = %d.\n", element_buffer);
		AND_LOG("The current activeTexture name = GL_TEXTURE0 + (%d).\n", (activeTexture - GL_TEXTURE0));
		AND_LOG("The current   state is = %s.\n", (blend ? "true":"false"));

		OBJ_CHECK(g_program, glIsProgram);
		OBJ_CHECK(g_background_left_tex_id.textureId, glIsTexture);
		OBJ_CHECK(g_background_right_tex_id.textureId, glIsTexture);
		OBJ_CHECK(g_rect_texture_id.textureId, glIsTexture);
		OBJ_CHECK(g_sphere_vbo, glIsBuffer);
		OBJ_CHECK(g_sphere_ibo, glIsBuffer);
		OBJ_CHECK(g_rect_vbo,   glIsBuffer);

#endif

		// render to framebuffer
		render_to_framebuffer();
		render_fbo_to_split_screen();

//		GL(glDisable(GL_DEPTH_TEST));
//		GL(glDepthFunc(GL_LESS));
//		GL(glDepthMask(GL_TRUE));

		GL(glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer));
		
		if (attrib_slot0.enabled)
		{
			GL(glEnableVertexAttribArray(0));
			GL(glVertexAttribPointer(0, attrib_slot0.size, attrib_slot0.type, attrib_slot0.normalized, attrib_slot0.stride, attrib_slot0.pointer));
		}

		if (attrib_slot1.enabled)
		{
			GL(glEnableVertexAttribArray(1));
			GL(glVertexAttribPointer(1, attrib_slot1.size, attrib_slot1.type, attrib_slot1.normalized, attrib_slot1.stride, attrib_slot1.pointer));
		}

		GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer));


		GL(glUseProgram(program));
		GL(glActiveTexture(activeTexture));
		GL(glViewport(0, 0, g_window_size.width, g_window_size.height));
	}

	void ogl_destroy()
	{
		if (g_program)
		{
			glDeleteProgram(g_program);
			g_program = 0;
		}

		release_texture(g_background_left_tex_id);
		release_texture(g_background_right_tex_id);
		release_texture(g_rect_texture_id);
		release_texture(g_fbo_left_tex);
		release_texture(g_fbo_right_tex);
		release_texture(g_tag_focus_tex);
		release_texture(g_tag_normal_tex);

		if (g_fbo_left)
		{
			delete g_fbo_left;
			delete g_fbo_right;

			g_fbo_left = NULL;
			g_fbo_right = NULL;
		}
#define DELETE_BUF(x) if(x) {GL(glDeleteBuffers(1, &x)); x = 0;}
		DELETE_BUF(g_sphere_ibo);
		DELETE_BUF(g_sphere_vbo);
		DELETE_BUF(g_rect_vbo);
		DELETE_BUF(g_rect_ibo);
		DELETE_BUF(g_billboard_vbo);
#undef DELETE_BUF
	}

	static void create_sphere_vertexs(int mSegmentsW, int mSegmentsH)
	{
#if INIT_ONCE
		if (g_rect_vbo != 0 && glIsBuffer(g_sphere_vbo) && glIsBuffer(g_sphere_ibo))
			return;
#endif
		const int numVertices = (mSegmentsW+1)*(mSegmentsH+1);
        const int numIndices = 2*mSegmentsW*(mSegmentsH-1)*3;
        int i, j;
		float mRadius = 1.0f;
		Vertex* pVertices = (Vertex*)malloc(numVertices * sizeof(Vertex));
		unsigned short* pIndices = (unsigned short*)malloc(numIndices * sizeof(unsigned short));
		unsigned int vtx_idx = 0;
		unsigned int idi_idx = 0;

		for (j = 0; j <= mSegmentsH; ++j)
		{
			float phi = (float)(PI*j / mSegmentsH);
			float y = mRadius*cosf(phi);

			float ringRadius = mRadius*sinf(phi);

			for (i = 0; i <= mSegmentsW; ++i)
			{
				float theta = (float)(2.0f*PI*i / mSegmentsW);
				float x = ringRadius * cosf(theta);
				float z = ringRadius * sinf(theta);

				pVertices[vtx_idx].position.x = x;
				pVertices[vtx_idx].position.y = y;
				pVertices[vtx_idx].position.z = z;

				//TextureCoords
				float u = (float)i / mSegmentsW;
				float v = (float)j / mSegmentsH;

				pVertices[vtx_idx].texcoord.x = u;
				pVertices[vtx_idx].texcoord.y = v;

				//Indexes
				if (i > 0 && j > 0)
				{
					int a = (mSegmentsW + 1)*j + i; // right-down
					int b = (mSegmentsW + 1)*j + i - 1; // left-down
					int c = (mSegmentsW + 1)*(j - 1) + i - 1; // left-upper
					int d = (mSegmentsW + 1)*(j - 1) + i; // right-upper


					if (j == mSegmentsH)
					{
						pIndices[idi_idx++] = a;
						pIndices[idi_idx++] = c;
						pIndices[idi_idx++] = d;
					}
					else if (j == 1)
					{
						pIndices[idi_idx++] = a;
						pIndices[idi_idx++] = b;
						pIndices[idi_idx++] = c;
					}
					else
					{
						pIndices[idi_idx++] = a;
						pIndices[idi_idx++] = b;
						pIndices[idi_idx++] = c;
						pIndices[idi_idx++] = a;
						pIndices[idi_idx++] = c;
						pIndices[idi_idx++] = d;
					}
				}

				vtx_idx++;
			}
		}

		assert(vtx_idx == numVertices);
		assert(idi_idx == numIndices);

		g_sphere_indices_count = numIndices;

		// create ogl buffers
		{
			GLuint bufs[2];
			GL(glGenBuffers(2, bufs));
			GL(glBindBuffer(GL_ARRAY_BUFFER, bufs[0]));
			GL(glBufferData(GL_ARRAY_BUFFER, numVertices * sizeof(Vertex), pVertices, GL_STATIC_DRAW));
			GL(glBindBuffer(GL_ARRAY_BUFFER, 0));

			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufs[1]));
			GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, numIndices * sizeof(unsigned short), pIndices, GL_STATIC_DRAW));
			GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

			g_sphere_vbo = bufs[0];
			g_sphere_ibo = bufs[1];
		}

		free(pVertices);
		free(pIndices);
	}

	static Vertex3 setVertex(float x, float y, float z)
	{
		Vertex3 v;
		v.x = x;
		v.y = y;
		v.z = z;

		return v;
	}

	static void create_rect_vertices(GLuint& buffer, GLenum usage)
	{
#if INIT_ONCE
		if (buffer != 0 && glIsBuffer(buffer))
			return;
#endif
		Vertex pVertices[4];
		pVertices[0].position = setVertex(-1, -1, 0);
		pVertices[1].position = setVertex(+1, -1, 0);
		pVertices[2].position = setVertex(-1, +1, 0);
		pVertices[3].position = setVertex(+1, +1, 0);

		for (int i = 0; i < 4; i++)
		{
			pVertices[i].texcoord.x = 0.5f * pVertices[i].position.x + 0.5f;
			pVertices[i].texcoord.y = 0.5f * pVertices[i].position.y + 0.5f;
		}

		GL(glGenBuffers(1, &buffer));
		GL(glBindBuffer(GL_ARRAY_BUFFER, buffer));
		GL(glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex), pVertices, usage));
		GL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	}

	static void create_program()
	{
		GLuint vertexShader;
		GLuint fragShader;
#if INIT_ONCE
		if (g_program != 0 && glIsProgram(g_program))
			return;
#endif
		g_program = GL(glCreateProgram());

		{
			vertexShader = GL(glCreateShader(GL_VERTEX_SHADER));
			GL(glShaderSource(vertexShader, 1, &g_pVertStr, 0));
			GL(glCompileShader(vertexShader));
//			if (!checkCompileError(shader, src[i].type))
//				return 0;

			GL(glAttachShader(g_program, vertexShader));

			// can be deleted since the program will keep a reference
			GL(glDeleteShader(vertexShader));
		}

		{
			fragShader = GL(glCreateShader(GL_FRAGMENT_SHADER));
			GL(glShaderSource(fragShader, 1, &g_pFragStr, 0));
			GL(glCompileShader(fragShader));
			//			if (!checkCompileError(shader, src[i].type))
			//				return 0;

			GL(glAttachShader(g_program, fragShader));

			// can be deleted since the program will keep a reference
			GL(glDeleteShader(fragShader));
		}

		GL(glBindAttribLocation(g_program, POSITION, "g_Position"));
		GL(glBindAttribLocation(g_program, TEXCOORD, "g_TextureCoord"));
		GL(glLinkProgram(g_program));

		GLint texLoc = GL(glGetUniformLocation(g_program, "sparrow"));
		g_mat_loc = GL(glGetUniformLocation(g_program, "g_Mat"));
		assert(texLoc >= 0);
		assert(g_mat_loc >= 0);

		AND_LOG("texLoc: %d\n", texLoc);
		AND_LOG("g_mat_loc: %d\n", g_mat_loc);

		GL(glUseProgram(g_program));
		GL(glUniform1i(texLoc, 0));
		GL(glUseProgram(0));
	}

#ifdef __cplusplus
}
#endif