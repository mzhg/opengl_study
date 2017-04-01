#pragma once
#include <memory>
#include "GLSLProgram.h"
#include "GLStates.h"

namespace jet
{
	namespace util
	{
		enum class AttribType
		{
			POSITION,
			NORMAL,
			TEXCOORD,
			COLOR,
			TANGENT,
			ELEMENT_INDICES,
			COUNT
		};

		enum class AttribLocationDef
		{
			POSITION,
			NORMAL,
			TEXCOORD,
			COLOR,
			TANGENT,
			ELEMENT_INDICES,
		};

		class Material
		{
		public:
			Material();
			~Material();

			virtual bool isDepthEnabled() const { return true; }
//			virtual bool isBlendEnabled() const { return false; }
			virtual bool isTranslucent() const { return m_Color.a < 1.0f; }

			/*Set the global mask color*/
			virtual void setColor(float r, float g, float b)
			{
				m_Color.r = r;
				m_Color.g = g;
				m_Color.b = b;
			}

			virtual bool isMatch(Material* pMat)
			{
				if (!pMat)
				{
					return false;
				}

				// default hehavior.
				if (getUniqueName() == pMat->getUniqueName())
				{
					return true;
				}
				else
				{
					return false;
				}
			}

			// sub-class must be implementing it.
			virtual const std::string& getUniqueName() = 0;
			virtual bool isInstanceBatch() const { return false; }

		protected:
			void createProgram(uint32_t count, const ShaderSourceItem* items);

		protected:
			
			GLSLProgram* m_Program;
			/*Global uniform color. Default value is white. Every material must be support this function.¡¡*/
			glm::vec4 m_Color;
			
		};

		typedef std::shared_ptr<Material> MaterialPtr;
	}
}

