#pragma once

#include "GLUtil.h"
#include "GLStates.h"
namespace jet
{
	namespace util
	{
		enum class MappingBits
		{
			READ = 1,
			WRITE = 2,
			READ_WRITE = READ | WRITE,
		};

		template<BufferTarget Target>
		class BufferGL : public Disposeable
		{
		public:
			BufferGL() :
			m_Buffer(0),
			m_Size(0)
#if defined(_DEBUG)
			,
			m_Mapped(false)
#endif
			{}

			void bind()
			{
				if (!m_Buffer)
				{
					glGenBuffers(1, &m_Buffer);
				}

				GLStates::get().bindBuffer(m_Buffer, Target);
			}
			void load(GLsizei size, const GLubyte* pData, BufferUsage usage = BufferUsage::STATIC_DRAW)
			{
				assert(size);
				m_Useage = usage;
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}
#endif
				CHECK_GL(glBufferData(__TARGET, size, pData, ConvertBufferUsageToGLenum(usage)));
			}

			void update(GLintptr offset, GLsizei size, const GLubyte* pData)
			{
				assert(size);
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}
#endif
				CHECK_GL(glBufferSubData(__TARGET, offfset, size, pData));
			}

			template<typename T>
			T* map(GLintptr offset, GLsizei length, MappingBits bits = MappingBits::READ_WRITE)
			{
				GLbitfield bits;

				switch (bits)
				{
				case jet::util::READ:
					bits = GL_MAP_READ_BIT;
					break;
				case jet::util::WRITE:
					bits = GL_MAP_WRITE_BIT;
					break;
				case jet::util::READ_WRITE:
					bits = (GL_MAP_READ_BIT | GL_MAP_WRITE_BIT);
					break;
				default:
					assert(false);
					break;
				}
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}

				m_Mapped = true;
#endif
				GLvoid* p = glMapBufferRange(__TARGET, offset, length, bits);
				CHECK_GL_ERROR;
				assert(p);
				return dynamic_cast<T*>(p);
			}

			void unmap(){ 
#if defined(_DEBUG)
				GLStates& state = GLStates::get();
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
					assert(false);
				}

				assert(m_Mapped);
				m_Mapped = false;
#endif
				glUnmapBuffer(__TARGET); 
			}

			void unbind()
			{
				GLStates& state = GLStates::get();
#if defined(_DEBUG)
				if (state.getBindingBuffer(Target) != m_Buffer)
				{
					printf("No buffer binding.\n");
				}
#endif
				state.resetBuffer(Target);
			}
			void dispose()
			{
				SAFE_RELEASE_BUFFER(m_Buffer);
			}

			virtual ~BufferGL()
			{
				dispose();
			}

			GLuint getSize() const 
			{
				return m_Size;
			}

			BufferTarget getTarget() const
			{
				return Target
			}
		private:
			GLuint m_Buffer;
			GLuint m_Size;
			BufferUsage m_Useage;

#if defined(_DEBUG)
			bool  m_Mapped;
#endif

			static const GLenum __TARGET = 0; // ConvertBufferTargetToGLenum(Target);
		};

		
		typedef BufferGL<BufferTarget::ARRAY> ArrayBufferGL;
		typedef BufferGL<BufferTarget::ELEMENT> ElementBufferGL;
		typedef BufferGL<BufferTarget::UNIFORM> UniformBufferGL;

		typedef struct BufferData
		{
			ArrayBufferGL* ArrayBuffer;
			ElementBufferGL* ElementBuffer;
			AttribDesc* AttribDescs;
			unsigned AttribCount;

			BufferData() :
				ArrayBuffer(nullptr),
				AttribDescs(nullptr),
				AttribCount(0),
				ElementBuffer(nullptr)
			{}

			BufferData(ArrayBufferGL* pArray, unsigned count, AttribDesc* pDesc, ElementBufferGL* pElement) :
				ArrayBuffer(pArray), AttribDescs(pDesc), AttribCount(count), ElementBuffer(pElement){}

		}BufferData;

		class VertexArrayGL : public Disposeable
		{
		public:
			VertexArrayGL() :m_VAO(0),
				m_BufferData()
			{} 

			void bind();

			void load(const BufferData* pArray);

//			void draw(unsigned count, GLint first = 0, unsigned numInstance = 1);

			void unbind();
			void dispose() override;
			~VertexArrayGL();

		private:
			GLuint m_VAO;

			BufferData m_BufferData;
		};
	}
}

