#pragma once

#include "Node.h"
#include "gl_state_define.h"
#include <sstream>

namespace jet
{
	namespace util
	{
	
		typedef struct MeteData
		{

			uint32_t uiLength;
			const uint8_t* pData;
			MeteData() :
				uiLength(0), pData(nullptr){}

			MeteData(uint32_t _uiLength, const uint8_t* _pData):
				uiLength(_uiLength), pData(_pData){}

			~MeteData()
			{
				if (pData)
				{
					delete[] pData;
				}
			}
		}MeteData;

		typedef std::shared_ptr<MeteData> MeteDataPtr;

		template<int Size>
		static char* to_str(char (&out)[Size], const glm::vec3& v)
		{
			sprintf_s(out, "(%f, %f, %f)", v.x, v.y, v.z);
			return out;
		}

		template<int Size>
		static char* to_str(char(&out)[Size], uint16_t i)
		{
			sprintf_s(out, "%d", i);
			return out;
		}

		template<typename T>
		void print_mete_data(const MeteData* pData, const char* msg)
		{
			std::stringstream out;
			char temp[128];
			printf("Mete Data: %s:\n", msg);
			printf("\tByteSize: %d\n", pData->uiLength);
			const T* arr = reinterpret_cast<const T*>(pData->pData);
			int arr_length = pData->uiLength / sizeof(T);

			for (int i = 0; i < arr_length; i++)
			{
				out << to_str(temp, arr[i]) << ",";
			}

			printf("\ Data: %s\n", out.str().c_str());
		}

		enum class MeshAttrib
		{
			NONE,
			POSITION2,
			POSITION3,
			POSITION4,
			NORMAL,
			TEXCOORD2,
			TEXCOORD3,
			INDICES,
			P2T2,

			P3NT2,
			P3N,
			P3NT3,

			P4NT2,
			P4N,
			P4NT3,
			COUNT
		};

		extern "C" uint32_t ParseMeshAttrib(MeshAttrib attrib, std::vector<AttribDesc>& desc);

		class Shape3D
		{
		public:

			static const unsigned short U16_PRIMITIVE_INDEX = 0XFFFF;
			static const unsigned int   U32_PRIMITIVE_INDEX = 0XFFFFFFFF;

			/**
			* The mode of the Mesh specifies both the type of primitive represented
			* by the mesh and how the data should be interpreted.
			*/
			enum class Mode 
			{
				// Undefined primitive type.
				UNKOWN,
				/**
				* A primitive is a single point in space. The size of the points
				* can be specified with {@link Mesh#setPointSize(float) }.
				*/
				POINTS,

				/**
				* A primitive is a line segment. Every two vertices specify
				* a single line. {@link Material#getAdditionalRenderState()} and {@link RenderState#setLineWidth(float)} can be used
				* to set the width of the lines.
				*/
				LINES,

				/**
				* A primitive is a line segment. The first two vertices specify
				* a single line, while subsequent vertices are combined with the
				* previous vertex to make a line. {@link Material#getAdditionalRenderState()} and {@link RenderState#setLineWidth(float)} can
				* be used to set the width of the lines.
				*/
				LINE_STRIP,

				/**
				* Identical to {@link #LineStrip} except that at the end
				* the last vertex is connected with the first to form a line.
				* {@link Material#getAdditionalRenderState()} and {@link RenderState#setLineWidth(float)} can be used
				* to set the width of the lines.
				*/
				LINE_LOOP,

				/**
				* A primitive is a triangle. Each 3 vertices specify a single
				* triangle.
				*/
				TRIANGLES,

				/**
				* Similar to {@link #Triangles}, the first 3 vertices
				* specify a triangle, while subsequent vertices are combined with
				* the previous two to form a triangle.
				*/
				TRIANGLE_STRIP,

				/**
				* Similar to {@link #Triangles}, the first 3 vertices
				* specify a triangle, each 2 subsequent vertices are combined
				* with the very first vertex to make a triangle.
				*/
				TRIANGLE_FAN,
				TRIANGLE_STRIP_RESTART,
				TRIANGLE_FAN_RESTART,

				/**
				* A combination of various triangle modes. It is best to avoid
				* using this mode as it may not be supported by all renderers.
				* The {@link Mesh#setModeStart(int[]) mode start points} and
				* {@link Mesh#setElementLengths(int[]) element lengths} must
				* be specified for this mode.
				*/
				HYBRID,
				/**
				* Used for Tesselation only. Requires to set the number of vertices
				* for each patch (default is 3 for triangle tesselation)
				*/
				PATCH,
				COUNT
			};

			Shape3D(Mode mode = Mode::TRIANGLES);
			~Shape3D();

			void setMode(Mode mode);
			Mode getMode() const    { return m_Mode; }

			virtual const Mode* getSupportedModes(int& length)const = 0;
			virtual const MeshAttrib* getSupportAttribs(uint32_t& length) const = 0;
			virtual const MeshAttrib getSupportCombinedAttrib() const = 0;
			virtual MeteDataPtr getVertexData(MeshAttrib attrib, bool indexed = true) const = 0;
			virtual const std::string& getUniqueName() const { return m_strName; }
			virtual int getNumLodLevels() const { return 1; }

			// Return the vertex count about the specified primitives of the shape , when indexed is false, return the count of the fully expand vertexs.
			virtual int getVertexCount(bool indexed = true) const = 0;
			// To avoid confuse to the getVertexCount(bool) method, provide a new method for the getting the number of the indices.
			virtual int getIndiceCount() const = 0;
			// Get the data type of the indices, default to return UINT16.
			virtual DataType getIndiceType() const { return DataType::UINT16; }
			
//			virtual int getTriangleCount(bool indexed = true) const = 0;
//			virtual int getLineCount(bool indexed = true) const = 0;
			virtual void getBound(BoundingVolume* pBound) const = 0;

			static const char* getModeName(Mode mode);
			static GLenum convertModeToGLenum(Mode mode);

		protected:
			virtual const char* getShapeName() const = 0;
			
			void createShapeName();

		protected:
			Mode m_Mode;
			std::string m_strName;
		};

		typedef std::shared_ptr<Shape3D> ShapePtr;

		class Box : public Shape3D
		{
		public:
			Box(Mode mode = Mode::TRIANGLES) : Shape3D(mode){ createShapeName(); }

			const Mode* getSupportedModes(int& length) const override;
			MeteDataPtr getVertexData(MeshAttrib attrib, bool indexed = true) const override;
			const MeshAttrib* getSupportAttribs(uint32_t& length) const override;
			const MeshAttrib getSupportCombinedAttrib() const override;
//			const std::string& getUniqueName() const override;
			int getVertexCount(bool indexed = true) const override;
			int getIndiceCount() const override;
//			int getTriangleCount(bool indexed = true) const override;
			void getBound(BoundingVolume* pBound) const override;
		protected:
			const char* getShapeName() const override;
		};

		typedef struct ShapeKey
		{
			std::string Name;
			bool Indexed;

			ShapeKey(const std::string& name, bool indexed = true) :
				Name(name), Indexed(indexed){}

			bool operator == (const ShapeKey& o) const
			{
				return Name == o.Name && Indexed == o.Indexed;
			}
		}ShapeKey;

//		typedef std::shared_ptr<Box> BoxPtr;
	}
}

namespace std
{
	template<> struct hash<jet::util::ShapeKey>
	{
		typedef jet::util::ShapeKey argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const
		{
			result_type const h1(std::hash<bool>{}(s.Indexed));
			result_type const h2(std::hash<std::string>{}(s.Name));
			return h1 ^ (h2 << 1); // or use boost::hash_combine (see Discussion)
		}
	};
}

