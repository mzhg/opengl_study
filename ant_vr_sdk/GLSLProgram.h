

#ifndef _JET_UTIL_GLSLPROGRAM_H
#define _JET_UTIL_GLSLPROGRAM_H

#include <stdint.h>
#include <string>
#include <GL\glew.h>
#include "Disposeable.h"

namespace jet
{
	namespace util
	{

		class OpenGLProgram : public Disposeable
		{
			virtual GLuint getProgram() = 0;
			virtual void enable() = 0;
			virtual void disable() = 0;
		};

		template<GLenum Target>
		class ShaderProgram : public OpenGLProgram
		{
		public:

			ShaderProgram() :m_Program(0){}
			void enable();
			void disable();

			/// Returns the index containing the named vertex attribute
			/// \param[in] uniform the null-terminated string name of the attribute
			/// \param[in] isOptional if true, the function logs an error if the attribute is not found
			/// \return the non-negative index of the attribute if found.  -1 if not found
			GLint getAttribLocation(const char* attribute, bool isOptional = false);

			/// Returns the index containing the named uniform
			/// \param[in] uniform the null-terminated string name of the uniform
			/// \param[in] isOptional if true, the function logs an error if the uniform is not found
			/// \return the non-negative index of the uniform if found.  -1 if not found
			GLint getUniformLocation(const char* uniform, bool isOptional = false);

			/// Returns the GL program object for the shader
			/// \return the GL shader object ID
			GLuint getProgram() { return m_Program; }
			GLenum getTarget()  { return Target; }

			/// Relinks an existing shader program to update based on external changes
			bool relink();

			void dispose();
			
			~ShaderProgram();

			friend class GLSLProgram;
		protected:
			// Only created from internal.
//			ShaderProgram(int program, int target) : m_Program(program), m_Target(target){}
		protected:
			GLuint m_Program;
		};

#if 0
		class VertexShader : public ShaderProgram<GL_VERTEX_SHADER>{};

		class PixelShader : public ShaderProgram<GL_FRAGMENT_SHADER>{};

		class TessControlShader : public ShaderProgram<GL_TESS_CONTROL_SHADER>{};

		class TessEvaluteShader : public ShaderProgram<GL_TESS_EVALUATION_SHADER>{};

		class GeometryShader : public ShaderProgram<GL_GEOMETRY_SHADER>{};

		class ComputeShader : public ShaderProgram<GL_COMPUTE_SHADER>{};
#else
		typedef ShaderProgram<GL_VERTEX_SHADER> VertexShader;
		typedef ShaderProgram<GL_FRAGMENT_SHADER> PixelShader;

		typedef ShaderProgram<GL_TESS_CONTROL_SHADER> TessControlShader;
		typedef ShaderProgram<GL_TESS_EVALUATION_SHADER> TessEvaluteShader;
		typedef ShaderProgram<GL_GEOMETRY_SHADER> GeometryShader;
		typedef ShaderProgram<GL_COMPUTE_SHADER> ComputeShader;
#endif

		/// Represents a pre-defined macro for the shader source building.
		struct Macro
		{
			std::string key;
			std::string value;
		};

		struct AttribBinding
		{
			std::string name;
			GLuint location;
		};

		/// Represents a piece of shader source and the shader type
		/// Used with creation functions to pass in arrays of multiple
		/// shader source types.
		struct ShaderSourceItem
		{
			const char* src; ///< Null-terminated string containing the shader source code
			GLenum type; ///< The GL_*_SHADER enum representing the shader type

			uint32_t length;  ///< Indices the length of the pre-defined macros.
			const Macro* macros;  ///< The pre-defined macros. Can be NULL.

			int32_t compileVersion;

			uint32_t attribLength;
			const AttribBinding* attribs;


			ShaderSourceItem() : src(NULL), type(GL_NONE), length(0), macros(NULL), compileVersion(0), attribLength(0), attribs(NULL){}
			ShaderSourceItem(const char* _src, GLenum _type, uint32_t _length = 0, const Macro* _macros = nullptr, int32_t _compileVersion = 0) :
				src(_src), type(_type), length(_length), macros(_macros), compileVersion(_compileVersion)
			{}
		};

		/// \file
		/// GLSL shader program wrapper

		/// Convenience wrapper for GLSL shader programs.
		/// Wraps shader programs and simplifies creation, setting uniforms and setting
		/// vertex attributes.  Supports all forms of shaders, but has simple paths for
		/// the common case of shader programs consisting of only vertex and fragment
		/// shaders.
		class GLSLProgram :public OpenGLProgram
		{
		public:

			/// Default constructor.
			/// Creates an empty object with no shader program, because we cannot set the source
			/// in the constructor.  Doing so would require the possibility of failure, and we cannot
			/// fail a constructor.
			GLSLProgram();
			~GLSLProgram();

//			template<class T>
//				static Macro newMacro(const char* name, T value);

			/// Creates and returns a shader object from a pair of filenames/paths
			/// Uses #NvAssetLoaderRead to load the files.  Convenience function.
			/// \param[in] vertFilename the filename and partial path to the text file containing the vertex shader source
			/// \param[in] fragFilename the filename and partial path to the text file containing the fragment shader source
			/// \param[in] strict if set to true, then later calls to retrieve the locations of non-
			/// existent uniforms and vertex attributes will log a warning to the output
			/// \return a pointer to an #NvGLSLProgram on success and NULL on failure
			static GLSLProgram* createFromFiles(const char* vertFilename, const char* fragFilename, bool strict = false);

			/// Creates and returns a shader object from a pair of source strings
			/// Convenience function.
			/// \param[in] vertSrc the null-terminated string containing the vertex shader source
			/// \param[in] fragSrc the null-terminated string containing the fragment shader source
			/// \param[in] strict if set to true, then later calls to retrieve the locations of non-
			/// existent uniforms and vertex attributes will log a warning to the output
			/// \return a pointer to an #NvGLSLProgram on success and NULL on failure
			static GLSLProgram* createFromStrings(const char* vertSrc, const char* fragSrc, bool strict = false);

			/// Creates and returns a shader object from a pair of arrays of source strings
			/// Convenience function.
			/// \param[in] vertSrcArray the array of null-terminated strings containing the vertex shader source
			/// \param[in] vertSrcCount the number of strings in vertSrcArray
			/// \param[in] fragSrcArray the array of null-terminated strings containing the fragment shader source
			/// \param[in] fragSrcCount the number of strings in fragSrcArray
			/// \param[in] strict if set to true, then later calls to retrieve the locations of non-
			/// existent uniforms and vertex attributes will log a warning to the output
			/// \return a pointer to an #NvGLSLProgram on success and NULL on failure
			static GLSLProgram* createFromStrings(const char** vertSrcArray, int32_t vertSrcCount,
				const char** fragSrcArray, int32_t fragSrcCount, bool strict = false);

			template<GLenum Target>
			static void createShaderFromStrings(const ShaderSourceItem& item, ShaderProgram<Target>*, bool strict = false);
			template<GLenum Target>
			static void createShaderFromFile(const char* filename, ShaderProgram<Target>*, GLuint attribLength = 0, const AttribBinding* attribs = 0, bool strict = false);

			/// Initializes an existing shader object from a pair of filenames/paths
			/// Uses #NvAssetLoaderRead to load the files.  Convenience function.
			/// \param[in] vertFilename the filename and partial path to the text file containing the vertex shader source
			/// \param[in] fragFilename the filename and partial path to the text file containing the fragment shader source
			/// \param[in] strict if set to true, then later calls to retrieve the locations of non-
			/// existent uniforms and vertex attributes will log a warning to the output
			/// \return true on success and false on failure
			bool setSourceFromFiles(const char* vertFilename, const char* fragFilename, bool strict = false);

			/// Creates and returns a shader object from a pair of source strings
			/// \param[in] vertSrc the null-terminated string containing the vertex shader source
			/// \param[in] fragSrc the null-terminated string containing the fragment shader source
			/// \param[in] strict if set to true, then later calls to retrieve the locations of non-
			/// existent uniforms and vertex attributes will log a warning to the output
			/// \return true on success and false on failure
			bool setSourceFromStrings(const char* vertSrc, const char* fragSrc, bool strict = false);

			/// Creates and returns a shader object from a pair of source strings
			/// Convenience function.
			/// \param[in] vertSrc the null-terminated string containing the vertex shader source
			/// \param[in] fragSrc the null-terminated string containing the fragment shader source
			/// \param[in] strict if set to true, then later calls to retrieve the locations of non-
			/// existent uniforms and vertex attributes will log a warning to the output
			/// \return a pointer to an #NvGLSLProgram on success and NULL on failure
			bool setSourceFromStrings(const char** vertSrcArray, int32_t vertSrcCount, const char** fragSrcArray,
				int32_t fragSrcCount, bool strict = false);

			/// Creates and returns a shader object from an array of #ShaderSourceItem source objects
			/// \param[in] src an array of #ShaderSourceItem objects containing the shaders sources to
			/// be loaded.  Unlike the vert/frag-only creation functions, this version can accept additional
			/// shader types such as geometry and tessellation shaders (if supported)
			/// \param[in] count the number of elements in #src array
			/// \param[in] strict if set to true, then later calls to retrieve the locations of non-
			/// existent uniforms and vertex attributes will log a warning to the output
			/// \return true on success and false on failure
			bool setSourceFromStrings(const ShaderSourceItem* src, int32_t count, bool strict = false);

			/// Binds the given shader program as current in the GL context
			void enable();

			/// Unbinds the given shader program from the GL context (binds shader 0)
			void disable();

			void dispose();

			/// Returns the index containing the named vertex attribute
			/// \param[in] uniform the null-terminated string name of the attribute
			/// \param[in] isOptional if true, the function logs an error if the attribute is not found
			/// \return the non-negative index of the attribute if found.  -1 if not found
			GLint getAttribLocation(const char* attribute, bool isOptional = false);

			/// Returns the index containing the named uniform
			/// \param[in] uniform the null-terminated string name of the uniform
			/// \param[in] isOptional if true, the function logs an error if the uniform is not found
			/// \return the non-negative index of the uniform if found.  -1 if not found
			GLint getUniformLocation(const char* uniform, bool isOptional = false);

			/// Returns the GL program object for the shader
			/// \return the GL shader object ID
			GLuint getProgram() { return m_Program; }

			/// Relinks an existing shader program to update based on external changes
			bool relink();

			/// Enables logging of missing uniforms even for non-strict shaders
			/// \param[in] logMissing if set to true, missing uniforms are logged when set,
			/// even if the shader was not created with the strict flag
//			static void setLogAllMissing(bool logMissing) { ms_logAllMissing = logMissing; }

			static const char* getShaderName(GLenum shaderType);
		protected:
			GLint m_Program;

			static bool checkCompileError(GLuint object, int32_t target);
			GLuint compileProgram(const char *vsource, const char *fsource);
			GLuint compileProgram(const char** vertSrcArray, int32_t vertSrcCount,
				const char** fragSrcArray, int32_t fragSrcCount);

			static std::string constructSource(const ShaderSourceItem& item);
			static std::string toString(uint32_t length, const Macro* macros);
			static std::string genVersion(int32_t version);
			static GLuint compileProgram(const ShaderSourceItem* src, int32_t count);
		};

		class ProgramPipeline:public OpenGLProgram
		{
		public:

			virtual void setRenderShader(VertexShader* pVS, PixelShader* pPS, TessControlShader* pTC = NULL, TessEvaluteShader* pTE = NULL, GeometryShader* pGS = NULL) = 0;
			virtual void setComputeShader(ComputeShader* pCS) = 0;

			virtual void enable() = 0;
			virtual void disable() = 0;

			virtual void setActiveShader(GLenum type) = 0;
			virtual bool isSperateShader() = 0;
			virtual void dispose() = 0;

			/*
			* Create a instance of the ProgramPipeline object. You should delete the pointer yourself when the pointer is no longer use.
			*/
			static ProgramPipeline* create();

			// The Program pipeline doesn't need to implemente this because it had performance issue.
			virtual void setUniform2f(const char* name, float x, float y){};
			virtual void setUniform1i(const char* name, int v){}

			// Noto: Only worked for the non-sperate shader program.
			void setPreLoadAttribs(int count, const AttribBinding* attribs)
			{
				m_AttribCount = count;
				m_PreLoadAttribs = attribs;
			}

		protected:
			const AttribBinding* m_PreLoadAttribs;
			int m_AttribCount;
		};
	}
}


#endif
