

#include "BufferGL.h"

namespace jet
{
	namespace util
	{
		void VertexArrayGL::bind()
		{
			if (!m_VAO)
			{
				glGenVertexArrays(1, &m_VAO);
			}

			GLStates& state = GLStates::get();
			state.bindVAO(m_VAO);
		}

		void VertexArrayGL::load(const BufferData* pArray)
		{
#if defined(_DEBUG)
			GLStates& state = GLStates::get();
			assert(state.getBindingVAO() == m_VAO && m_VAO != 0);
#endif
			if (pArray)
			{
				pArray->ArrayBuffer->bind();
				unsigned index = 0;
				AttribDesc* pDesc = pArray->AttribDescs;
				while (index < pArray->AttribCount)
				{
					const AttribDesc& desc = pArray->AttribDescs[index];
					if (desc.Enable)
					{
						glEnableVertexAttribArray(desc.Index);
						glVertexAttribPointer(desc.Index, desc.Size, ConvertDataTypeToGLenum(desc.Type), desc.Normalized, desc.Stride, desc.Pointer);
						if (glVertexAttribDivisor)
						{
							glVertexAttribDivisor(desc.Index, desc.Divisor);
						}
					}
					else
					{
						glEnableVertexAttribArray(desc.Index);
					}
					index++;
				}

				if (pArray->ElementBuffer)
				{
					pArray->ElementBuffer->bind();
				}
				
			}
		}
#if 0
		void VertexArrayGL::draw(unsigned numInstance)
		{
#if defined(_DEBUG)
			GLStates& state = GLStates::get();
			assert(state.getBindingVAO() == m_VAO);
#endif
			const GLenum mode = ConvertPrimitivesToGLenum(m_ElementData.Primitive);
			if (m_ElementData.ElementBuffer)
			{
				const GLenum type = ConvertDataTypeToGLenum(m_ElementData.ElementType);
				if (numInstance <= 1)
				{
					glDrawElements(mode, m_ElementData.Count, type, (const GLvoid*)0);
				}
				else
				{
					glDrawElementsInstanced(mode, m_ElementData.Count, type, (const GLvoid*)0, numInstance);
				}
			}
			else
			{
				if (numInstance <= 1)
				{
					glDrawArrays(mode, 0, m_ElementData.Count);
				}
				else
				{
					glDrawArraysInstanced(mode, 0, m_ElementData.Count, numInstance);
				}
			}
		}
#endif
		void VertexArrayGL::unbind()
		{
			GLStates& state = GLStates::get();
			state.resetVAO();
		}

		void VertexArrayGL::dispose()
		{
			SAFE_RELEASE_VERTEX_ARRAY(m_VAO);
		}

		VertexArrayGL::~VertexArrayGL()
		{
			dispose();
		}
	}
}