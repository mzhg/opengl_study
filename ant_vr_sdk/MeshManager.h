#pragma once

#include "BufferGL.h"
#include "Shape3D.h"
#include <vector>

namespace jet
{
	namespace util
	{
#define GEN_VERTEX_VNT_DATA(v, n, t) \
	typedef struct VertexDataV##v##N##n##T##t\
	{\
		glm::vec##v f##v##Position; \
		glm::vec##n f##n##Normal;    \
		glm::vec##t f##t##Texcoord; \
		}VertexDataV##v##N##n##T##t; 

#define GEN_VERTEX_VT_DATA(v, t) \
	typedef struct VertexDataV##v##T##t\
		{\
		glm::vec##v f##v##Position; \
		glm::vec##t f##t##Texcoord; \
		}VertexDataV##v##T##t; 


		GEN_VERTEX_VNT_DATA(3, 3, 2);
		GEN_VERTEX_VNT_DATA(4, 3, 2);
		GEN_VERTEX_VT_DATA(2, 2);

		// 存储Shape3D的顶点数据
		class ShapeCache
		{

		};

		// abstract class represents the mesh batching.
		class MeshBatching
		{
		public:
			MeshBatching() :
				m_VertexArray()
			{}


		protected:
			VertexArrayGL m_VertexArray;
		private:
			std::vector<Shape3D*> m_Shapes;
		};

		class MeshManager
		{
		public:
			MeshManager()
			{

			}
			~MeshManager();
		};
	}
}

