
#include <stdio.h>
#include "GLSLProgram.h"
#include "AssetLoader.h"
#include "GLUtil.h"
#include <sstream>
#include <vector>
#include <map>
#include "Util.h"

namespace jet
{
	namespace util
	{
		GLSLProgram::GLSLProgram() : m_Program(0) {}

		GLSLProgram::~GLSLProgram()
		{
			dispose();
		}

		GLSLProgram* GLSLProgram::createFromFiles(const char* vertFilename, const char* fragFilename, bool strict)
		{
			GLSLProgram* prog = new GLSLProgram;

			if (prog->setSourceFromFiles(vertFilename, fragFilename, strict)) {
				return prog;
			}
			else {
				delete prog;
				return NULL;
			}
		}
		GLSLProgram* GLSLProgram::createFromStrings(const char* vertSrc, const char* fragSrc, bool strict)
		{
			GLSLProgram* prog = new GLSLProgram;

			if (prog->setSourceFromStrings(vertSrc, fragSrc, strict)) {
				return prog;
			}
			else {
				delete prog;
				return NULL;
			}
		}

		const char* GLSLProgram::getShaderName(GLenum shaderType)
		{
			static const char* SHADER_NAMES[] = 
			{
				"Vertex", "Fragment", "Geometry", "Tess_Control", "Tess_Evaluation", "Compute", "Unkown"
			};

			switch (shaderType)
			{
				case GL_VERTEX_SHADER: return SHADER_NAMES[0];
				case GL_FRAGMENT_SHADER: return SHADER_NAMES[1];
				case GL_GEOMETRY_SHADER: return SHADER_NAMES[2];
				case GL_TESS_CONTROL_SHADER: return SHADER_NAMES[3];
				case GL_TESS_EVALUATION_SHADER: return SHADER_NAMES[4];
				case GL_COMPUTE_SHADER: return SHADER_NAMES[5];

				default:return SHADER_NAMES[6];
			}
		}

		GLSLProgram* GLSLProgram::createFromStrings(const char** vertSrcArray, int32_t vertSrcCount,
			const char** fragSrcArray, int32_t fragSrcCount, bool strict)
		{
			GLSLProgram* prog = new GLSLProgram;

			if (prog->setSourceFromStrings(vertSrcArray, vertSrcCount, fragSrcArray, fragSrcCount, strict)) {
				return prog;
			}
			else {
				delete prog;
				return NULL;
			}
		}

		bool GLSLProgram::setSourceFromFiles(const char* vertFilename, const char* fragFilename, bool strict)
		{
			int32_t len;

			AssetLoaderAddSearchPath("../ant_vr_sdk");
			char* vertSrc = AssetLoaderRead(vertFilename, len);
			char* fragSrc = AssetLoaderRead(fragFilename, len);

			debugString(vertSrc);
			debugString(fragSrc);

			if (!vertSrc || !fragSrc) {
				AssetLoaderFree(vertSrc);
				AssetLoaderFree(fragSrc);
				return false;
			}

			bool success = setSourceFromStrings(vertSrc, fragSrc, strict);

			AssetLoaderFree(vertSrc);
			AssetLoaderFree(fragSrc);

			return success;
		}

		bool GLSLProgram::setSourceFromStrings(const char* vertSrc, const char* fragSrc, bool strict)
		{
			const char* vertSrcArray[] = { vertSrc };
			const char* fragSrcArray[] = { fragSrc };
			return setSourceFromStrings(vertSrcArray, 1, fragSrcArray, 1, strict);
		}

		bool GLSLProgram::setSourceFromStrings(const char** vertSrcArray, int32_t vertSrcCount,
			const char** fragSrcArray, int32_t fragSrcCount, bool strict)
		{
			int glVersion = GetOpenGLVersion().toInt();
			std::string vertSource;
			std::string fragSource;
			ShaderSourceItem shader_items[2];
			if (vertSrcCount <= 1 && fragSrcCount <= 1)
			{
				shader_items[0] = ShaderSourceItem(vertSrcCount ? vertSrcArray[0] : NULL, GL_VERTEX_SHADER, 0, 0, glVersion);
				shader_items[1] = ShaderSourceItem(fragSrcCount ? fragSrcArray[0] : NULL, GL_FRAGMENT_SHADER, 0, 0, glVersion);
			}
			else
			{
				if (vertSrcCount)
				{
					for (int i = 0; i < vertSrcCount; i++)
					{
						vertSource += vertSrcArray[i];
						vertSource += "\n";
					}

					shader_items[0] = ShaderSourceItem(vertSource.c_str(), GL_VERTEX_SHADER, 0, 0, glVersion);
				}

				if (fragSrcCount)
				{
					for (int i = 0; i < fragSrcCount; i++)
					{
						fragSource += fragSrcArray[i];
						fragSource += "\n";
					}

					shader_items[1] = ShaderSourceItem(fragSource.c_str(), GL_FRAGMENT_SHADER, 0, 0, glVersion);
				}
			}

			debugString(shader_items[0].src);
			debugString(shader_items[1].src);

			bool result = setSourceFromStrings(shader_items, 2, strict);
			return result;
		}

		bool GLSLProgram::setSourceFromStrings(const ShaderSourceItem* src, int32_t count, bool strict)
		{
			if (m_Program) {
				glDeleteProgram(m_Program);
				m_Program = 0;
			}

//			m_strict = strict;

			m_Program = compileProgram(src, count);

			return m_Program != 0;
		}

		void GLSLProgram::enable()
		{
			glUseProgram(m_Program);
		}

		void GLSLProgram::disable()
		{
			glUseProgram(0);
		}

		void GLSLProgram::dispose()
		{
			if (glIsProgram(m_Program))
			{
				glDeleteProgram(m_Program);
				m_Program = 0;
			}
		}

		bool GLSLProgram::checkCompileError(GLuint shader, int32_t target)
		{
			// check if shader compiled
			GLint compiled = 0;
			bool m_strict = false;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

			if (!compiled || m_strict)
			{
				if (!compiled) {
					printf("Error compiling shader ");
				}
				GLint infoLen = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);
				if (infoLen) {
					char* buf = new char[infoLen];
					if (buf) {
						glGetShaderInfoLog(shader, infoLen, NULL, buf);
						printf("Shader log:\n%s\n", buf);
						delete[] buf;
					}
				}
				if (!compiled) {
					glDeleteShader(shader);
					shader = 0;
					return false;
				}
			}
			return true;
		}

		/*
		template<class T>
		Macro GLSLProgram::newMacro(const char* name, T value)
		{
			std::stringstream out;
			out << value;

			return Macro(std::string(name), out.str());
		}
		*/

		GLuint GLSLProgram::compileProgram(const char *vsource, const char *fsource)
		{
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

			const char* sourceItems[2];
			int sourceCount = 0;
//			if (ms_shaderHeader)
//				sourceItems[sourceCount++] = ms_shaderHeader;
			sourceItems[sourceCount++] = vsource;

			glShaderSource(vertexShader, sourceCount, sourceItems, 0);

			sourceCount = 0;
//			if (ms_shaderHeader)
//				sourceItems[sourceCount++] = ms_shaderHeader;
			sourceItems[sourceCount++] = fsource;

			glShaderSource(fragmentShader, sourceCount, sourceItems, 0);

			glCompileShader(vertexShader);
			if (!checkCompileError(vertexShader, GL_VERTEX_SHADER))
				return 0;

			glCompileShader(fragmentShader);
			if (!checkCompileError(fragmentShader, GL_FRAGMENT_SHADER))
				return 0;

			GLuint program = glCreateProgram();

			glAttachShader(program, vertexShader);
			glAttachShader(program, fragmentShader);

			// can be deleted since the program will keep a reference
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			glLinkProgram(program);

			// check if program linked
			GLint success = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &success);

			if (!success)
			{
				GLint bufLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
				if (bufLength) {
					char* buf = new char[bufLength];
					if (buf) {
						glGetProgramInfoLog(program, bufLength, NULL, buf);
						printf("Could not link program:\n%s\n", buf);
						delete[] buf;
					}
				}
				glDeleteProgram(program);
				program = 0;
			}

			return program;
		}

		GLuint GLSLProgram::compileProgram(
			const char** vertSrcArray, int32_t vertSrcCount,
			const char** fragSrcArray, int32_t fragSrcCount)
		{
			GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
			GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

			{
				const char** sourceItems = new const char*[vertSrcCount + 1];
				int sourceCount = 0;
//				if (ms_shaderHeader)
//					sourceItems[sourceCount++] = ms_shaderHeader;

				for (int i = 0; i < vertSrcCount; i++)
					sourceItems[sourceCount++] = vertSrcArray[i];

				glShaderSource(vertexShader, sourceCount, sourceItems, 0);

				delete[] sourceItems;
			}

	{
		const char** sourceItems = new const char*[fragSrcCount + 1];
		int sourceCount = 0;
//		if (ms_shaderHeader)
//			sourceItems[sourceCount++] = ms_shaderHeader;

		for (int i = 0; i < fragSrcCount; i++)
			sourceItems[sourceCount++] = fragSrcArray[i];

		glShaderSource(fragmentShader, sourceCount, sourceItems, 0);

		delete[] sourceItems;
	}

	glCompileShader(vertexShader);
	if (!checkCompileError(vertexShader, GL_VERTEX_SHADER))
		return 0;

	glCompileShader(fragmentShader);
	if (!checkCompileError(fragmentShader, GL_FRAGMENT_SHADER))
		return 0;

	GLuint program = glCreateProgram();

	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);

	// can be deleted since the program will keep a reference
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	glLinkProgram(program);

	// check if program linked
	GLint success = 0;
	glGetProgramiv(program, GL_LINK_STATUS, &success);

	if (!success)
	{
		GLint bufLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
		if (bufLength) {
			char* buf = new char[bufLength];
			if (buf) {
				glGetProgramInfoLog(program, bufLength, NULL, buf);
				printf("Could not link program:\n%s\n", buf);
				delete[] buf;
			}
		}
		glDeleteProgram(program);
		program = 0;
	}

	return program;
		}



		GLuint GLSLProgram::compileProgram(const ShaderSourceItem* src, int32_t count)
		{
			GLuint program = glCreateProgram();
			int32_t i;
			for (i = 0; i < count; i++) {
				GLuint shader = glCreateShader(src[i].type);

				std::string shaderSource = constructSource(src[i]);
				const char* pShaderSource[] = { shaderSource.c_str() };

				glShaderSource(shader, 1, pShaderSource, 0);
				glCompileShader(shader);
				if (!checkCompileError(shader, src[i].type))
					return 0;

				glAttachShader(program, shader);

				// can be deleted since the program will keep a reference
				glDeleteShader(shader);
			}

			glLinkProgram(program);

			// check if program linked
			GLint success = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &success);

			if (!success)
			{
				GLint bufLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
				if (bufLength) {
					char* buf = new char[bufLength];
					if (buf) {
						glGetProgramInfoLog(program, bufLength, NULL, buf);
						printf("Could not link program:\n%s\n", buf);
						delete[] buf;
					}
				}
				glDeleteProgram(program);
				program = 0;
			}

			return program;
		}

		static bool _relink(GLuint program)
		{
			glLinkProgram(program);

			// check if program linked
			GLint success = 0;
			glGetProgramiv(program, GL_LINK_STATUS, &success);

			if (!success)
			{
				GLint bufLength = 0;
				glGetProgramiv(program, GL_INFO_LOG_LENGTH, &bufLength);
				if (bufLength) {
					char* buf = new char[bufLength];
					if (buf) {
						glGetProgramInfoLog(program, bufLength, NULL, buf);
						printf("Could not link program:\n%s\n", buf);
						delete[] buf;
					}
				}
				return false;
			}
			return true;
		}

		static GLint _getAttribLocation(GLuint program, const char* attribute, bool isOptional)
		{
			GLint result = glGetAttribLocation(program, attribute);

			if (result == -1)
			{
				if (/*(ms_logAllMissing || m_strict) &&*/ !isOptional) {
					printf
						(
						"could not find attribute \"%s\" in program %d\n",
						attribute,
						program
						);
				}
			}

			return result;
		}

		static GLint _getUniformLocation(GLuint program, const char* uniform, bool isOptional)
		{
			GLint result = glGetUniformLocation(program, uniform);

			if (result == -1)
			{
				if (/*(ms_logAllMissing || m_strict) &&*/ !isOptional) {
					printf
						(
						"could not find uniform \"%s\" in program %d\n",
						uniform,
						program
						);
				}
			}

			return result;
		}

		bool GLSLProgram::relink()
		{
			return _relink(m_Program);
		}

		GLint GLSLProgram::getAttribLocation(const char* attribute, bool isOptional)
		{
			return _getAttribLocation(m_Program, attribute, isOptional);
		}

		GLint GLSLProgram::getUniformLocation(const char* uniform, bool isOptional)
		{
			return _getUniformLocation(m_Program, uniform, isOptional);
		}

		std::string GLSLProgram::toString(uint32_t length, const Macro* macros)
		{
			std::string output;

			for (uint32_t i = 0; i < length; i++)
			{
				output += macros->key + " " + macros->value;
				macros++;
			}

			return output;
		}

		std::string GLSLProgram::genVersion(int32_t version)
		{
			std::stringstream out;
			out << "#version " << version << std::endl;

			return out.str();
		}

		std::string GLSLProgram::constructSource(const ShaderSourceItem& item)
		{
			static const int GL_VERSIONS[] = {
				110, 120, 130, 140, 150, 300, 330, 400, 410, 420, 430, 440, 450
			};

			static const int GLES_VERSIONS[] = {
				100, 300, 310, 320
			};

			GLVersion version = GetOpenGLVersion();

			int length = 0;
//			std::string source = item.src;
			if (item.length == 0 && item.compileVersion == 0)
			{
				return item.src;
			}

			std::string source = item.src;
			std::stringstream macro_string_builder;
			if (item.compileVersion)
			{
				int idx = -1;
				if (version.es)
				{
					idx = Numeric::indexOf(_countof(GLES_VERSIONS), GLES_VERSIONS, item.compileVersion);
				}
				else
				{
					idx = Numeric::indexOf(_countof(GL_VERSIONS), GL_VERSIONS, item.compileVersion);
				}
				assert(idx >= 0);
				
				macro_string_builder << "#version " << item.compileVersion;
				if (GetOpenGLVersion().es)
				{
					macro_string_builder << " es";
				}
				macro_string_builder << std::endl;
			}


			if (item.length)
			{
				for (uint32_t i = 0; i < item.length; i++)
				{
					if (item.macros[i].key.empty())
					{
						continue;
					}

					macro_string_builder << "#define " << item.macros[i].key << ' ' << item.macros[i].value << std::endl;
				}
			}

			std::string macro_string = std::string(macro_string_builder.str());

			std::string::npos;
			auto versionIndex = source.find("#version");
			auto versionLineEnd = std::string::npos;
			if (versionIndex <source.length() && versionIndex != std::string::npos)
			{
				versionLineEnd = source.find_first_of('\n', versionIndex + 8);
				assert(versionLineEnd != std::string::npos);
			}

			// If the ShaderSourceItem string contain version tag.
			if (item.compileVersion)
			{
				// replace the old version.
				if (versionIndex <source.length() &&  versionIndex != std::string::npos)
				{
					source.replace(versionIndex, versionLineEnd, macro_string);
				}
				// insert the macroString to the head of old source.
				else
				{
//					source.reserve(source.length() + macro_string.length() + 1);
					source.insert(0, macro_string);
//					return macro_string + source;
				}
			}
			else
			{
				if (versionLineEnd == std::string::npos)
				{
					source.reserve(source.length() + macro_string.length() + 1);
					source.insert(0, macro_string);
				}
				else
				{
					source.reserve(source.length() + macro_string.length() + 1);
					source.insert(versionLineEnd + 1, macro_string);
				}
			}

			printf("Reconstruct Shader Source: \n%s\n", source.c_str());

			return source;
		}

#if ENABLE_PROGRAM_PIPELINE
		class GPUProgramPipeline : public ProgramPipeline
		{
		public:
			GPUProgramPipeline() : m_ProgramPipeline(0) {}

			bool isSperateShader(){ return true; }
			void checkInit()
			{
				if (m_ProgramPipeline == 0)
				{
					glGenProgramPipelines(1, &m_ProgramPipeline);
				}
			}

			void setRenderShader(VertexShader* pVS, PixelShader* pPS, TessControlShader* pTC, TessEvaluteShader* pTE, GeometryShader* pGS)
			{
				m_pVS = pVS;
				m_pPS = pPS;
				m_pTC = pTC;
				m_pTE = pTE;
				m_pGS = pGS;

				m_pCS = NULL;

				if (pVS){ glUseProgramStages(m_ProgramPipeline, GL_VERTEX_SHADER_BIT, pVS->getProgram()); }
				if (pPS){ glUseProgramStages(m_ProgramPipeline, GL_FRAGMENT_SHADER_BIT, pPS->getProgram()); }
				if (pTC){ glUseProgramStages(m_ProgramPipeline, GL_TESS_CONTROL_SHADER_BIT, pTC->getProgram()); }
				if (pTE){ glUseProgramStages(m_ProgramPipeline, GL_TESS_EVALUATION_SHADER_BIT, pTE->getProgram()); }
				if (pGS){ glUseProgramStages(m_ProgramPipeline, GL_GEOMETRY_SHADER_BIT, pGS->getProgram()); }
			}
			void setComputeShader(ComputeShader* pCS)
			{
				m_pCS = pCS;
				if (pCS){ glUseProgramStages(m_ProgramPipeline, GL_COMPUTE_SHADER_BIT, pCS->getProgram()); }

				m_pVS = NULL;
				m_pPS = NULL;
				m_pTC = NULL;
				m_pTE = NULL;
				m_pGS = NULL;
			}

			GLuint getProgram() { return 0; }
			void setActiveShader(GLenum type)
			{
				switch (type)
				{
				case GL_VERTEX_SHADER: 
					if (m_pVS){
						glActiveShaderProgram(m_ProgramPipeline, m_pVS->getProgram());
					}
					break;
				case GL_FRAGMENT_SHADER:
					if (m_pPS){
						glActiveShaderProgram(m_ProgramPipeline, m_pPS->getProgram());
					}
					break;
				case GL_TESS_CONTROL_SHADER:
					if (m_pTC){
						glActiveShaderProgram(m_ProgramPipeline, m_pTC->getProgram());
					}
					break;
				case GL_TESS_EVALUATION_SHADER:
					if (m_pTE){
						glActiveShaderProgram(m_ProgramPipeline, m_pTE->getProgram());
					}
					break;
				case GL_GEOMETRY_SHADER:
					if (m_pGS){
						glActiveShaderProgram(m_ProgramPipeline, m_pGS->getProgram());
					}
					break;
				case GL_COMPUTE_SHADER:
					if (m_pCS){
						glActiveShaderProgram(m_ProgramPipeline, m_pCS->getProgram());
					}
					break;
				default:
					break;
				}
			}

			void enable()
			{
				checkInit();
				glBindProgramPipeline(m_ProgramPipeline);
			}

			void disable()
			{
				glBindProgramPipeline(0);
			}

			void dispose()
			{
				if (m_ProgramPipeline == 0) return;
				if (glIsProgramPipeline(m_ProgramPipeline))
				{
					glDeleteProgramPipelines(1, &m_ProgramPipeline);
				}
			}

			template<GLenum Target>
			static void _setUniform2f(ShaderProgram<Target>* pShader, const char* name, float x, float y)
			{
				if (pShader != NULL)
				{
					GLint location = _getUniformLocation(pShader->getProgram(), name, true);
					if (location >= 0)
					{
						glProgramUniform2f(pShader->getProgram(), location, x, y);
					}
				}
			}

			void setUniform2f(const char* name, float x, float y) override
			{
				_setUniform2f(m_pVS, name, x, y);
				_setUniform2f(m_pPS, name, x, y);
				_setUniform2f(m_pTC, name, x, y);
				_setUniform2f(m_pTE, name, x, y);
				_setUniform2f(m_pGS, name, x, y);
				_setUniform2f(m_pCS, name, x, y);
			}

			template<GLenum Target>
			static void _setUniform4f(ShaderProgram<Target>* pShader, const char* name, float x, float y, float z, float w)
			{
				if (pShader != NULL)
				{
					GLint location = _getUniformLocation(pShader->getProgram(), name, true);
					if (location >= 0)
					{
						glProgramUniform4f(pShader->getProgram(), location, x, y, z, w);
					}
				}
			}

			void setUniform4f(const char* name, float x, float y, float z, float w) override
			{
				_setUniform4f(m_pVS, name, x, y,z,w);
				_setUniform4f(m_pPS, name, x, y,z,w);
				_setUniform4f(m_pTC, name, x, y,z,w);
				_setUniform4f(m_pTE, name, x, y,z,w);
				_setUniform4f(m_pGS, name, x, y,z,w);
				_setUniform4f(m_pCS, name, x, y,z,w);
			}

			template<GLenum Target>
			static void _setUniform1i(ShaderProgram<Target>* pShader, const char* name, int v)
			{
				if (pShader != NULL)
				{
					GLint location = _getUniformLocation(pShader->getProgram(), name, true);
					if (location >= 0)
					{
						glProgramUniform1i(pShader->getProgram(), location, v);
					}
				}
			}

			void setUniform1i(const char* name, int x) override
			{
				_setUniform1i(m_pVS, name, x);
				_setUniform1i(m_pPS, name, x);
				_setUniform1i(m_pTC, name, x);
				_setUniform1i(m_pTE, name, x);
				_setUniform1i(m_pGS, name, x);
				_setUniform1i(m_pCS, name, x);
			}

			~GPUProgramPipeline()
			{
				dispose();
			}

		private:
			GLuint m_ProgramPipeline;

			VertexShader* m_pVS;
			PixelShader* m_pPS;
			TessControlShader* m_pTC;
			TessEvaluteShader* m_pTE;
			GeometryShader* m_pGS;
			ComputeShader* m_pCS;
		};

#endif
		struct ShaderValue
		{
			union 
			{
				GLuint vs;
				GLuint cs;
			};

			GLuint ps;
			GLuint tc;
			GLuint te;
			GLuint gs;
		};

		inline bool operator < (const ShaderValue& a, const ShaderValue& b)
		{
#define COMPARE(name)  \
	if(a.name < b.name) \
		return true;    \
				else if (a.name > b.name) \
		return false;

			COMPARE(vs);
			COMPARE(ps);
			COMPARE(tc);
			COMPARE(te);
			COMPARE(gs);

#undef COMPARE
			return false;
		}
#if 0
#define EQUAL(name) if(a.name != b.name) return false; 

		inline bool operator == (const ShaderValue& a, const ShaderValue& b)
		{
			EQUAL(vs);
			EQUAL(ps);
			EQUAL(tc);
			EQUAL(te);
			EQUAL(gs);

			return true;
		}
#endif

		class CPUProgramPipeline : public ProgramPipeline
		{
		public:

			CPUProgramPipeline():m_Program(0){}

			bool isSperateShader(){ return false; }
			void setActiveShader(GLenum type){/* Does not need do anything. */}

			template<GLenum Target>
			void attachShader(GLuint programID, ShaderProgram<Target>* shader)
			{
				if (shader)
				{
					assert(glIsShader(shader->getProgram()));
					glAttachShader(programID, shader->getProgram());
				}
			}

			void setRenderShader(VertexShader* pVS, PixelShader* pPS, TessControlShader* pTC, TessEvaluteShader* pTE, GeometryShader* pGS)
			{
				ShaderValue key = { pVS ? pVS->getProgram() : 0, 
									pPS ? pPS->getProgram() : 0,
									pTC ? pTC->getProgram() : 0,
									pTE ? pTE->getProgram() : 0,
									pGS ? pGS->getProgram() : 0,};

				GLuint programID;
				auto it = m_Programs.find(key);
				if (it != m_Programs.end())
				{
					programID = it->second;
				}
				else
				{
					programID = glCreateProgram();
					attachShader(programID, pVS);
					attachShader(programID, pPS);
					attachShader(programID, pTC);
					attachShader(programID, pTE);
					attachShader(programID, pGS);

					if (m_AttribCount)
					{
						for (int i = 0; i < m_AttribCount; i++)
						{
							glBindAttribLocation(programID, m_PreLoadAttribs[i].location, m_PreLoadAttribs[i].name.c_str());
						}
					}

					_relink(programID);
					m_Programs.insert(std::pair<ShaderValue, GLuint>(key, programID));
				}

				m_Program = programID;
				glUseProgram(programID);
			}
			void setComputeShader(ComputeShader* pCS)
			{
				ShaderValue key = { pCS ? pCS->getProgram() : 0, 0,0,0,0};

				GLuint programID;
				auto it = m_Programs.find(key);
				if (it != m_Programs.end())
				{
					programID = it->second;
				}
				else
				{
					programID = glCreateProgram();
					attachShader(programID, pCS);

					_relink(programID);
					m_Programs.insert(std::pair<ShaderValue, GLuint>(key, programID));
				}

				m_Program = programID;
				glUseProgram(programID);
			}

			GLuint getProgram() { return m_Program; }
			void enable()
			{
				// Doesn't do nothing here!
			}

			void disable()
			{
				glUseProgram(0);
			}

			void setUniform4f(const char* name, float x, float y, float z, float w) override
			{
				GLint location = _getUniformLocation(m_Program, name, false);
				if (location >= 0)
				{
					glUniform4f(location, x, y, z, w);
				}
			}

			void setUniform2f(const char* name, float x, float y) override
			{
				GLint location = _getUniformLocation(m_Program, name, false);
				if (location >= 0)
				{
					glUniform2f(location, x, y);
				}
				
			}

			void setUniform1i(const char* name, int v) override
			{
				GLint location = _getUniformLocation(m_Program, name, false);
				if (location >= 0)
				{
					glUniform1i(location, v);
				}
			}

			void dispose()
			{
				if (m_Program == 0) return;
				for (auto it = m_Programs.begin(); it != m_Programs.end(); it++)
				{
					if (it->second != 0 && glIsProgram(it->second))
					{
						glDeleteProgram(it->second);
					}
				}

				m_Programs.clear();
				m_Program = 0;
			}

			~CPUProgramPipeline()
			{
				dispose();
			}

		private:
			std::map<ShaderValue, GLuint> m_Programs;
			GLuint m_Program;
		};

		ProgramPipeline* ProgramPipeline::create()
		{
#if ENABLE_PROGRAM_PIPELINE
			GLVersion version = GetOpenGLVersion();
			int intVersion = version.toInt();
			if (intVersion >= 410 || (version.es && intVersion >= 310))
			{
				return new GPUProgramPipeline();
			}
			else
#endif
			{
				return new CPUProgramPipeline();
			}
		}
	}
}