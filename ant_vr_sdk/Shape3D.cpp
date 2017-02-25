#include "Shape3D.h"

namespace jet
{
	namespace util
	{
		static const Shape3D::Mode BOX_MODES[] =
		{
			Shape3D::Mode::POINTS,
			Shape3D::Mode::LINES,
			Shape3D::Mode::LINE_STRIP,
			Shape3D::Mode::LINE_LOOP,
			Shape3D::Mode::TRIANGLES,
			Shape3D::Mode::TRIANGLE_FAN_RESTART,
		};

		Shape3D::Shape3D(Mode mode) : m_Mode(mode)
		{
			if (Numeric::indexOf(_countof(BOX_MODES), BOX_MODES, mode) < 0)
			{
				assert(false);
			}
		}

		void Shape3D::createShapeName()
		{
			char name[128];
			sprintf_s(name, "%s_%s", getShapeName(), getModeName(m_Mode));
			m_strName = name;
		}

		void Shape3D::setMode(Mode mode)
		{
			if (mode != m_Mode)
			{
				if (Numeric::indexOf(_countof(BOX_MODES), BOX_MODES, mode) < 0)
				{
					assert(false);
				}

				m_Mode = mode;
				createShapeName();
			}
		}

		Shape3D::~Shape3D()
		{
		}

		const char* Shape3D::getModeName(Mode mode)
		{
			static const char* pModeNames[] = 
			{
				TO_STR(POINTS),
				TO_STR(LINES),
				TO_STR(LINE_STRIP),
				TO_STR(LINE_LOOP),
				TO_STR(TRIANGLES),
				TO_STR(TRIANGLE_STRIP),
				TO_STR(TRIANGLE_FAN),
				TO_STR(TRIANGLE_STRIP_RESTART),
				TO_STR(TRIANGLE_FAN_RESTART),
				TO_STR(HYBRID),
				TO_STR(PATCH)
			};

			return pModeNames[static_cast<int>(mode)];
		}

		static const float CUBE_POSITIONS[] =
		{
			// strip form
			-1.0f, -1.0f, +1.0f,
			+1.0f, -1.0f, +1.0f,
			-1.0f, +1.0f, +1.0f,
			+1.0f, +1.0f, +1.0f,

			-1.0f, -1.0f, -1.0f,
			+1.0f, -1.0f, -1.0f,
			-1.0f, +1.0f, -1.0f,
			+1.0f, +1.0f, -1.0f,
		};

		static const uint16_t CUBE_INDICES_TRIANGLES[] = 
		{
			0,1,2, 2,1,3,  // front 
			4,6,5, 5,6,7,  // back
			0,4,1, 1,4,5,  // bottom
			2,3,6, 6,3,7,  // top
			0,2,4, 4,2,6,  // left
			1,5,3, 3,5,7   // right
		};

		static const int LINE_COUNT = 12;

		static const uint16_t CUBE_INDICES_LINES[] = 
		{
			0,1, 0,2, 2,3, 1,3,
			4,5, 4,6, 6,7, 5,7,
			0,4, 1,5, 2,6, 3,7
		};

		static const uint16_t CUBE_INDICES_QUAD[] =
		{
			0, 1, 2, 3,  // front 
			4, 6, 5, 7,  // back
			0, 4, 1, 5,  // bottom
			2, 3, 6, 7,  // top
			0, 2, 4, 6,  // left
			1, 5, 3, 7   // right
		};

		static const float CUBE_NORMALS[] = 
		{
			0.0f, 0.0f, 1.0f,  // front 
			0.0f, 0.0f, -1.0f, // back
			0.0f, -1.0f, 0.0f, // bottom
			0.0f, 1.0f, 0.0f,  // top
			-1.0f, 0.0f, 0.0f, // left
			1.0f, 0.0f, 0.0f   // right
		};

		static const float CUBE_TEXCOORD[] =
		{
			0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f
		};

		const Shape3D::Mode* Box::getSupportedModes(int& length) const
		{
			length = _countof(BOX_MODES);
			return BOX_MODES;
		}

		template<int P, int N, int T>
		static MeteDataPtr getBoxVertex(Shape3D::Mode mode, bool indexed)
		{
			MeteDataPtr out = MeteDataPtr(new MeteData);
			const glm::vec3* pSources = (glm::vec3*)CUBE_POSITIONS;
			const glm::vec3* pNormals = (glm::vec3*)CUBE_NORMALS;
			const glm::vec2* pTexcoords = (glm::vec2*)CUBE_TEXCOORD;

			switch (mode)
			{
			case jet::util::Shape3D::Mode::POINTS:
			case jet::util::Shape3D::Mode::LINES:
			{
				out->uiLength = _countof(CUBE_POSITIONS) / 3 * sizeof(float) * P;
				out->pData = new uint8_t[out->uiLength];
				float* pVertexs = (float*)(out->pData);

				for (int i = 0; i < _countof(CUBE_POSITIONS) / 3; i++)
				{
					for (int j = 0; j < 3; j++)
					{
						*pVertexs++ = pSources[i][j];
					}

					if (P > 3)
					{
						*pVertexs++ = 1.0f;
					}
				}
			}
				break;
			case jet::util::Shape3D::Mode::TRIANGLES:
				if (indexed)
				{
					const int COUNT = _countof(CUBE_INDICES_QUAD);
					out->uiLength = sizeof(float) * COUNT* (P + N + T);
					uint8_t* pData = new uint8_t[out->uiLength];
					float* pVertexs = (float*)(pData);

					int vtxIdx = 0;
					for (uint16_t index : CUBE_INDICES_QUAD)
					{
						// Fill the vertex.
						for (int j = 0; j < Numeric::min(P, 3); j++)
						{
							*pVertexs++ = pSources[index][j];
						}

						if (P > 3)
						{
							*pVertexs++ = 1.0f;
						}

						// Fill the normal
						int faceIdx = vtxIdx / 4;
						for (int j = 0; j < Numeric::min(N, 3); j++)
						{
							*pVertexs++ = pNormals[faceIdx][j];
						}

						// Fill the texcoord.
						for (int j = 0; j < Numeric::min(T, 2); j++)
						{
							*pVertexs++ = pTexcoords[vtxIdx % 4][j];
						}

						vtxIdx++;
					}
					out->pData = pData;
				}
				else
				{
					out->uiLength = sizeof(float) * (P + N + T) * _countof(CUBE_INDICES_TRIANGLES);
					uint8_t* pData = new uint8_t[out->uiLength];

					float* pVertexs = (float*)(pData);

					int vtxIdx = 0;
					for (uint16_t index : CUBE_INDICES_TRIANGLES)
					{
						// Fill the vertex.
						if (P)
						{
							assert(P >= 3);
							for (int j = 0; j < Numeric::min(P, 3); j++)
							{
								*pVertexs++ = pSources[index][j];
							}

							if (P > 3)
							{
								*pVertexs++ = 1.0f;
							}
						}
						

						// Fill the normal
						if (N)
						{
							assert(N == 3);
							int faceIdx = vtxIdx / 6;
							for (int j = 0; j < 3; j++)
							{
								*pVertexs++ = pNormals[faceIdx][j];
							}
						}
					
						// Fill the texcoord.
						if (T)
						{
							assert(T == 2);
							int tex_index;
							int _vtxIdx = vtxIdx % 6;
							int triIdx = _vtxIdx / 3;
							if (triIdx == 0)
							{
								tex_index = _vtxIdx;
							}
							else
							{
								if (_vtxIdx == 3) tex_index = 2;
								else if (_vtxIdx == 4) tex_index = 1;
								else tex_index = 3;
							}
							for (int j = 0; j < Numeric::min(T, 2); j++)
							{
								*pVertexs++ = pTexcoords[tex_index][j];
							}
						}
						

						vtxIdx++;
					}

					out->pData = pData;
				}
				break;
			default:
				break;
			}

			return out;
		}

		static MeteDataPtr getBoxIndices(Shape3D::Mode mode, bool indexed)
		{
			MeteDataPtr out = MeteDataPtr(new MeteData);
			const glm::vec2* pSources = (glm::vec2*)CUBE_TEXCOORD;
			switch (mode)
			{
			case jet::util::Shape3D::Mode::POINTS:
			{
				out->uiLength = sizeof(uint16_t) * _countof(CUBE_POSITIONS) / 3;
				uint8_t* pData = new uint8_t[out->uiLength];
				uint16_t* pIndices = (uint16_t*)(pData);
				for (int i = 0; i < _countof(CUBE_POSITIONS) / 3; i++)
				{
					pIndices[i] = i;
				}
				out->pData = pData;
			}
				break;
			case jet::util::Shape3D::Mode::LINES:
			{
				
				out->uiLength = sizeof(CUBE_INDICES_LINES);
				uint8_t* pData = new uint8_t[out->uiLength];
				memcpy(pData, CUBE_INDICES_LINES, out->uiLength);
				out->pData = pData;
			}
				break;
			case jet::util::Shape3D::Mode::TRIANGLES:
			{
				out->uiLength = 36 * 2;
				uint8_t* pData = new uint8_t[out->uiLength];
				uint16_t* pIndices = (uint16_t*)(pData);

				for (int i = 0; i < _countof(CUBE_INDICES_QUAD)/4; i++)
				{
					int startIdx = 4 * i;
					*pIndices++ = startIdx;
					*pIndices++ = startIdx + 1;
					*pIndices++ = startIdx + 2;

					*pIndices++ = startIdx + 2;
					*pIndices++ = startIdx + 1;
					*pIndices++ = startIdx + 3;
				}
				out->pData = pData;

				print_mete_data<uint16_t>(out.get(), "Box Indices");
			}
				
				break;
			default:
				break;
			}

			return out;
		}

		MeteDataPtr Box::getVertexData(MeshAttrib attrib, bool indexed) const
		{
			switch (attrib)
			{
			case jet::util::MeshAttrib::POSITION2:
			case jet::util::MeshAttrib::P2T2:
			case jet::util::MeshAttrib::P3NT3:
			case jet::util::MeshAttrib::P4NT3:
				assert(false);
			case jet::util::MeshAttrib::POSITION3:
				return getBoxVertex<3, 0, 0>(m_Mode, indexed);
				break;
			case jet::util::MeshAttrib::POSITION4:
				return getBoxVertex<4, 0, 0>(m_Mode, indexed);
				break;
			case jet::util::MeshAttrib::NORMAL:
				return getBoxVertex<0, 3, 0>(m_Mode, indexed);
				break;
			case jet::util::MeshAttrib::TEXCOORD2:
				return getBoxVertex<0, 0, 2>(m_Mode, indexed);
				break;
			case jet::util::MeshAttrib::TEXCOORD3:
				break;
			case jet::util::MeshAttrib::INDICES:
				return getBoxIndices(m_Mode, indexed);
				break;
			case jet::util::MeshAttrib::P3NT2:
				return getBoxVertex<3, 3, 2>(m_Mode, indexed);
				break;
			case jet::util::MeshAttrib::P3N:
				return getBoxVertex<3, 3, 0>(m_Mode, indexed);
				break;
			
			case jet::util::MeshAttrib::P4NT2:
				return getBoxVertex<4, 3, 2>(m_Mode, indexed);
				break;
			case jet::util::MeshAttrib::P4N:
				return getBoxVertex<4, 3, 0>(m_Mode, indexed);
				break;
			
			default:
				break;
			}
		}

		const char* Box::getShapeName() const
		{
			static const char* name = "Box";
			return name;
		}

		int Box::getVertexCount(bool indexed) const
		{
			switch (m_Mode)
			{
			case jet::util::Shape3D::Mode::POINTS:  
			case jet::util::Shape3D::Mode::LINES:   return _countof(CUBE_POSITIONS) / 3;
			case jet::util::Shape3D::Mode::TRIANGLES: return indexed ? 24 : 36;
			case jet::util::Shape3D::Mode::LINE_STRIP:
			case jet::util::Shape3D::Mode::LINE_LOOP:
			case jet::util::Shape3D::Mode::TRIANGLE_STRIP:
			case jet::util::Shape3D::Mode::TRIANGLE_FAN:
			case jet::util::Shape3D::Mode::TRIANGLE_STRIP_RESTART:
			case jet::util::Shape3D::Mode::TRIANGLE_FAN_RESTART:
			case jet::util::Shape3D::Mode::HYBRID:
			case jet::util::Shape3D::Mode::PATCH:
			default:
				return 0;
			}
		}

		int Box::getTriangleCount(bool indexed) const
		{
			return  12;
		}

		void Box::getBound(BoundingVolume* pBound) const
		{
			const glm::vec3* pSources = (glm::vec3*)CUBE_POSITIONS;
			for (int i = 0; i < 8; i++)
			{
				// TODO
			}
		}
	}
}
