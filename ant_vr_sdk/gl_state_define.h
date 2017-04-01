#pragma once
#include <GL\glew.h>
#include "geometry2d.h"
#include "geometry3d.h"

namespace jet
{
	namespace util
	{
		enum class BlendFunc
		{
			ZERO,
			ONE,
			SRC_ALPHA,
			SRC_ALPHA_INVERT,
			SRC_COLOR,
			SRC_COLOR_INVERT,
			DST_COLOR,
			DST_COLOR_INVERT,
			DST_ALPHA,
			DST_ALPHA_INVERT,
			CONSTANT_COLOR,
			CONSTANT_COLOR_INVERT,
			CONSTANT_ALPHA,
			CONSTANT_ALPHA_INVERT,
		};
		enum class BlendOpFunc
		{
			ADD,
			SUBTRACT,
			SUBTRACT_REVERSE,
			MIN,
			MAX
		};

		typedef struct RenderTargetBlendDesc
		{
			bool BlendEnable;
			BlendFunc SrcBlend;
			BlendFunc DestBlend;
			BlendOpFunc   BlendOp;
			BlendFunc SrcBlendAlpha;
			BlendFunc DestBlendAlpha;
			BlendOpFunc BlendOpAlpha;

			RenderTargetBlendDesc() :
				BlendEnable(false),
				SrcBlend(BlendFunc::ONE),
				DestBlend(BlendFunc::ZERO),
				BlendOp(BlendOpFunc::ADD),
				SrcBlendAlpha(BlendFunc::ONE),
				DestBlendAlpha(BlendFunc::ZERO),
				BlendOpAlpha(BlendOpFunc::ADD)
			{}
		}RenderTargetBlendDesc;

		typedef struct MultisampleDesc
		{
			bool MultisampleEanble;
			bool AlphaToCoverageEnable;
			bool AlphaToOneEnable;
			bool CoverageEnable;
			float SampleCoverageValue;
			bool SampleConverageInvert;

			MultisampleDesc() :
				MultisampleEanble(false),
				AlphaToCoverageEnable(false),
				AlphaToOneEnable(false),
				CoverageEnable(false),
				SampleCoverageValue(1.0f),
				SampleConverageInvert(false)
			{}
		}MultisampleDesc;

		typedef struct BlendDesc
		{
			MultisampleDesc Multisample;
			bool IndependentBlendEnable;
			bool AntialiasedLineEnable;
			bool AntialiasedPolygonEnable;
			bool AntialiasedPointEnable;

			RenderTargetBlendDesc RenderTargets[8];
			BlendDesc() :
				Multisample(),
				IndependentBlendEnable(false),
				AntialiasedLineEnable(false),
				AntialiasedPolygonEnable(false),
				AntialiasedPointEnable(false)
			{
				/*
				for (int i = 0; i < 8; i++)
				{
				RenderTargets[i] = RenderTargetBlendDesc();
				}*/
			}
		}BlendDesc;

		enum class StencilOp
		{
			KEEP,
			ZERO,
			REPLACE,
			INCR,
			INCR_WRAP,
			DECR,
			DECR_WRAP,
			INVERT,
		};
		extern "C" GLenum ConvertStencilOpToGLenum(StencilOp func);
		extern "C" StencilOp ConvertGLenumToStencilOp(GLenum func);
		enum class CompareFunc
		{
			NEVER,
			LESS,
			LEQUAL,
			GREATER,
			GEQUAL,
			EQUAL,
			NOTEQUAL,
			ALWAYS,
		};
		extern "C" GLenum ConvertCompareFuncToGLenum(CompareFunc func);
		extern "C" CompareFunc ConvertGLenumToCompareFunc(GLenum func);
		enum class PolygonMode
		{
			POINT,
			LINE,
			FILL,
		};
		extern "C" GLenum ConvertPolygonModeToGLenum(PolygonMode func);
		extern "C" PolygonMode ConvertGLenumToPolygonMode(GLenum func);
		enum class FaceMode
		{
			NONE,
			FRONT,
			BACK,
			FRONT_AND_BACK,
		};
		extern "C" GLenum ConvertFaceModeToGLenum(FaceMode func);
		extern "C" FaceMode ConvertGLenumToFaceMode(GLenum func);
		enum class LogicFunc
		{
			CLEAR,
			SET,
			COPY,
			COPY_INVERT,
			NOOP,
			INVERT,
			AND,
			NAND,
			OR,
			NOR,
			XOR,
			EQUIV,
			AND_REVERSE,
			AND_INVERT,
			OR_REVERSE,
			OR_INVERT,
		};
		extern "C" GLenum ConvertLogicFuncToGLenum(LogicFunc func);
		extern "C" LogicFunc ConvertGLenumToLogicFunc(GLenum func);

		typedef struct DepthStencilOPDesc
		{
			StencilOp StencilFailOp;
			StencilOp StencilDepthFailOp;
			StencilOp StencilPassOp;
			CompareFunc StencilFunc;
			int		  StencilWriteMask;
			int		  StencilMask;
			int		  StencilRef;

			DepthStencilOPDesc() :
				StencilFailOp(StencilOp::KEEP),
				StencilDepthFailOp(StencilOp::KEEP),
				StencilPassOp(StencilOp::KEEP),
				StencilFunc(CompareFunc::ALWAYS),
				StencilWriteMask(0xFF),
				StencilMask(0xFF),
				StencilRef(0)
			{}
		}DepthStencilOPDesc;

		typedef struct DepthStencilDesc
		{
			bool DepthEnable;
			bool DepthWriteMask;
			CompareFunc DepthFunc;
			bool StencilEnable;
			DepthStencilOPDesc FrontFace;
			DepthStencilOPDesc BackFace;

			DepthStencilDesc() :
				DepthEnable(false),
				DepthWriteMask(true),
				DepthFunc(CompareFunc::LESS),
				StencilEnable(false),
				FrontFace(),
				BackFace()
			{}
		}DepthStencilDesc;

		typedef struct LogicDesc
		{
			LogicFunc LogicOp;
			bool LogicEnable;

			LogicDesc() :
				LogicOp(LogicFunc::COPY),
				LogicEnable(false){}
		}LogicDesc;

		typedef struct DepthRangeDesc
		{
			double NearVal;
			double FarVal;

			DepthRangeDesc() : NearVal(0.0f), FarVal(1.0f){}
		}DepthRangeDesc;

		typedef struct PolygonOffsetDesc
		{
			float factor;
			float units;
			PolygonOffsetDesc() :
				factor(0.0f), units(0.0f){}
		}PolygonOffsetDesc;

		typedef struct ColorMask
		{		
			bool RedWriteMask;
			bool GreenWriteMask;
			bool BlueWriteMask;
			bool AlphaWriteMask;

			ColorMask(bool r = true, bool g = true, bool b = true, bool a = true) :
				RedWriteMask(r), GreenWriteMask(g),
				BlueWriteMask(b), AlphaWriteMask(a)
			{}
		}ColorMask;

		enum class SpriteCoordOrigin
		{
			LOWER_LEFT,
			UPPER_LEFT,
		};
		extern "C" GLenum ConvertSpriteCoordOriginToGLenum(SpriteCoordOrigin func);
		extern "C" SpriteCoordOrigin ConvertGLenumToSpriteCoordOrigin(GLenum func);

		typedef struct PointSpriteDesc
		{
			bool PointSpriteEnabled;
			bool ProgramPointSizeEnabled;
			GLfloat PointFadeThresholdSize;
			SpriteCoordOrigin PointSpriteCoordOrigin;

			PointSpriteDesc() :
				PointSpriteEnabled(false),
				ProgramPointSizeEnabled(false),
				PointFadeThresholdSize(1.0f),
				PointSpriteCoordOrigin(SpriteCoordOrigin::UPPER_LEFT)
			{}
		}PointSpriteDesc;

		typedef struct ViewportDesc
		{
			Rectangle2i Viewport;
			Rectangle2i ScissorBox;
			bool ScissorEnable;

			ViewportDesc() :
				ScissorEnable(false){}
		}ViewportDesc;

		typedef struct RasterizerDesc
		{
			PolygonMode FillMode;
			FaceMode CullMode;
			bool FrontCounterClockwise;
			bool CullFaceEnable;
			bool DepthClampEnable;
			DepthRangeDesc DepthRange;
			bool Dither;
			bool SRGB;
			bool PolygonOffsetEnable;
			bool RasterizedDiscardEnable;
			bool ProgramPointSizeEnable;
			bool PrimitiveRestartEnable;
			GLuint PrimitiveRestartIndex;
			PolygonOffsetDesc PolygonOffset;
			LogicDesc LogicMode;
			
			ColorMask ColorWriteMask;
			PointSpriteDesc PointSprite;

			RasterizerDesc() :
				FillMode(PolygonMode::FILL),
				CullMode(FaceMode::BACK),
				FrontCounterClockwise(false),  // ccw
				CullFaceEnable(false),
				DepthClampEnable(false),
				DepthRange(),
				Dither(false),
				SRGB(false),
				LogicMode(),
				PolygonOffsetEnable(false),
				PolygonOffset(),
				RasterizedDiscardEnable(false),
				ProgramPointSizeEnable(false),
				PrimitiveRestartEnable(false),
				PrimitiveRestartIndex(0),
				ColorWriteMask(),
				PointSprite()
			{}
		}RasterizerDesc;

		extern "C" GLenum ConvertBlendFuncToGLenum(BlendFunc func);
		extern "C" BlendFunc ConvertGLenumToBlendFunc(GLenum func);
		extern "C" GLenum ConvertBlendOpFuncToGLenum(BlendOpFunc func);
		extern "C" BlendOpFunc ConvertGLenumToBlendOpFunc(GLenum func);

		enum class BufferTarget
		{
			ARRAY,
			COPY_READ,
			COPY_WRITE,
			DISPATCH_INDIRECT,
			DRAW_INDIRECT,
			ELEMENT,
			PIXEL_PACK,
			PIXEL_UNPACK,
			TEXTURE,
			QUERY,
			ATOMIC_COUNTER,
			SHADER_STORAGE,
			TRANSFORM_FEEDBACK,
			UNIFORM,
			COUNT
		};

		extern "C" GLenum ConvertBufferTargetToGLenum(BufferTarget func);
		extern "C" BufferTarget ConvertGLenumToBufferTarget(GLenum func);

		enum class FramebufferTarget
		{
			FRAMEBUFFER,
			DRAW,
			READ,
		};
		extern "C" GLenum ConvertFramebufferTargetToGLenum(FramebufferTarget func);
		extern "C" FramebufferTarget ConvertGLenumToFramebufferTarget(GLenum func);

		enum class HintTarget
		{
			LINE_SMOOTH,
			POLYGON_SMOOTH,
			TEXTURE_COMPRESSION,
			FRAGMENT_SHADER_DERIVATIVE,
		};
		extern "C" GLenum ConvertHintTargetToGLenum(HintTarget func);
		extern "C" HintTarget ConvertGLenumToHintTarget(GLenum func);

		enum class HintMode
		{
			FASTEST,
			NICEST,
			DONT_CARE,
		};
		extern "C" GLenum ConvertHintModeToGLenum(HintMode func);
		extern "C" HintMode ConvertGLenumToHintMode(GLenum func);

		enum class ProvokeMode
		{
			FIRST,
			LAST,
		};
		extern "C" GLenum ConvertProvokeModeToGLenum(ProvokeMode func);
		extern "C" ProvokeMode ConvertGLenumToProvokeMode(GLenum func);

		typedef struct ShaderBufferDesc
		{
			GLuint Buffer;  // The name of a buffer object 
			BufferTarget Target; // Specify the target of the bind operation. target must be one of ATOMIC_COUNTER, TRANSFORM_FEEDBACK, UNIFORM, or SHADER_STORAGE.
			GLuint Index;
			GLuint Offset;
			GLuint Size;

			ShaderBufferDesc(GLuint buffer, BufferTarget target, GLuint index, GLuint offset = 0, GLuint size = 0) :
				Buffer(buffer), Target(target), Index(index), Offset(offset), Size(size){}

			ShaderBufferDesc(){}
		}ShaderBufferDesc;

		typedef struct PackPixelStoreDesc
		{
			bool PackSwapBytes;
			bool PackLSBFirst;

			GLuint PackRowLength;
			GLuint PackSkipRows;
			GLuint PackSkipImages;
			GLuint PackSkipPixels;
			GLuint PackAlignment;
			GLuint PackImageHeight;
			/*
			GLuint PackSkipImages;
			GLuint PackCompressedBlockWidth;
			GLuint PackCompressedBlockHeight;
			GLuint PackCompressedBlockDepth;
			GLuint PackCompressedBlockSize;
			*/

			PackPixelStoreDesc() :
				PackSwapBytes(false), PackLSBFirst(false),
				PackRowLength(0), PackSkipRows(0),
				PackSkipImages(0), PackSkipPixels(0),
				PackAlignment(4), PackImageHeight(0)
			{}
		}PackPixelStoreDesc;

		typedef struct UnPackPixelStoreDesc
		{
			bool UnpackSwapBytes;
			bool UnpackLSBFirst;

			GLuint UnpackRowLength;
			GLuint UnpackSkipRows;
			GLuint UnpackSkipImages;
			GLuint UnpackSkipPixels;
			GLuint UnpackAlignment;
			GLuint UnpackImageHeight;
			/*
			GLuint UnpackSkipImages;
			GLuint UnpackCompressedBlockWidth;
			GLuint UnpackCompressedBlockHeight;
			GLuint UnpackCompressedBlockDepth;
			GLuint UnpackCompressedBlockSize;
			*/

			UnPackPixelStoreDesc() :
				UnpackSwapBytes(false), UnpackLSBFirst(false),
				UnpackRowLength(0), UnpackSkipRows(0),
				UnpackSkipImages(0), UnpackSkipPixels(0),
				UnpackAlignment(4), UnpackImageHeight(0)
			{}
		}UnPackPixelStoreDesc;

		enum class DataType
		{
			UNKOWN,
			INT8,
			UINT8,
			INT16,
			UINT16,
			INT32,
			UINT32,
			HALF,
			FLOAT,
			DOUBLE,
		};
		extern "C" GLenum ConvertDataTypeToGLenum(DataType func);
		extern "C" DataType ConvertGLenumToDataType(GLenum func);
		extern "C" GLuint MeasureDataTypeSize(DataType func);

		typedef	struct AttribDesc
		{
//			bool Enable;
			GLuint Index;
			GLuint Size;
			DataType Type;
			bool Normalized;
			GLuint Stride;
			GLuint Divisor;
			GLvoid* Pointer;

			AttribDesc(/*bool enable,*/
				GLuint index = 0,
				GLuint size = 4,
				DataType type = DataType::FLOAT,
				bool normalized = false,
				GLuint stride = 0,
				GLuint divisor = 0,
				GLvoid* pointer = nullptr) :
				/*Enable(enable),*/ Index(index), Size(size), Type(type), Normalized(normalized), Stride(stride), Divisor(divisor), Pointer(pointer){}

			bool operator== (const AttribDesc& b) const
			{
				return Divisor == b.Divisor &&
					Index == b.Index && Normalized == b.Normalized &&
					Pointer == b.Pointer && Size == b.Size &&
					Stride == b.Stride && Type == b.Type;
			}

			bool operator!= (const AttribDesc& b) const
			{
				return Divisor != b.Divisor ||
					Index != b.Index || Normalized != b.Normalized ||
					Pointer != b.Pointer || Size != b.Size ||
					Stride != b.Stride || Type != b.Type;
			}
		}AttribDesc;

		typedef struct GeometryAttribDesc
		{
			uint32_t AttribCount;
			AttribDesc* AttribDescs;

			bool operator == (const GeometryAttribDesc& o) const
			{
				if (AttribCount != o.AttribCount)
				{
					return false;
				}

				for (uint32_t i = 0; i < AttribCount; i++)
				{
					if (AttribDescs[i] != o.AttribDescs[i])
					{
						return false;
					}
				}

				return true;
			}

			bool operator != (const GeometryAttribDesc& o) const
			{
				if (AttribCount != o.AttribCount)
				{
					return true;
				}

				for (uint32_t i = 0; i < AttribCount; i++)
				{
					if (AttribDescs[i] != o.AttribDescs[i])
					{
						return true;
					}
				}

				return false;
			}

		}GeometryAttribDesc;

		typedef struct GeometryBufferDesc
		{
			uint32_t BufferCount;
			GeometryAttribDesc* BufferDescs;

			bool operator == (const GeometryBufferDesc& o) const
			{
				if (BufferCount != o.BufferCount)
				{
					return false;
				}

				for (uint32_t i = 0; i < BufferCount; i++)
				{
					if (BufferDescs[i] != o.BufferDescs[i])
					{
						return false;
					}
				}

				return true;
			}
		}GeometryBufferDesc;

		typedef struct GeometryAttribData
		{
			uint32_t AttribCount;
			uint32_t* AttribSize;
			uint8_t** AttribData;
			DataType ElementDataType;
			uint32_t ElementSize;
			uint8_t* ElementData;
		}GeometryAttribData;

//		extern bool operator == (const AttribDesc& a, const AttribDesc& b);

		enum class Primitives
		{
			POINTS,
			LINE_STRIP,
			LINE_LOOP,
			LINES,
			LINE_STRIP_ADJACENCY,
			LINES_ADJACENCY,
			TRIANGLE_STRIP,
			TRIANGLE_FAN,
			TRIANGLES,
			TRIANGLE_STRIP_ADJACENCY,
			TRIANGLES_ADJACENCY,
			PATCHES,
		};
		extern "C" GLenum ConvertPrimitivesToGLenum(Primitives func);
		extern "C" Primitives ConvertGLenumToPrimitives(GLenum func);

		enum class TextureFilter
		{
			NEAREST,
			LINEAR,
			NEAREST_MIPMAP_NEAREST,
			LINEAR_MIPMAP_NEAREST,
			NEAREST_MIPMAP_LINEAR,
			LINEAR_MIPMAP_LINEAR,
		};
		extern "C" GLenum ConvertTextureFilterToGLenum(TextureFilter func);
		extern "C" TextureFilter ConvertGLenumToTextureFilter(GLenum func);
		enum class TextureWrap
		{
			CLAMP,
			MIRRORED_REPEAT,
			REPEAT,
			MIRROR_CLAMP,
			BORDER,
		};
		extern "C" GLenum ConvertTextureWrapToGLenum(TextureWrap func);
		extern "C" TextureWrap ConvertGLenumToTextureWrap(GLenum func);

		enum class ColorSwizzle
		{
			RED,
			GREEN,
			BLUE,
			ALPHA,
			ZERO,
			ONE,
		};
		extern "C" GLenum ConvertColorSwizzleToGLenum(ColorSwizzle func);
		extern "C" ColorSwizzle ConvertGLenumToColorSwizzle(GLenum func);

		typedef struct SamplerDesc
		{
			TextureFilter MinFilter;
			TextureFilter MagFilter;
			TextureWrap   WrapS;
			TextureWrap   WrapT;
			TextureWrap   WrapR;
			GLfloat       BorderColor[4];
			GLint         Anisotropic;
			bool          CompareToRef;
			CompareFunc   CompareMode;

			SamplerDesc() :
				MinFilter(TextureFilter::LINEAR),
				MagFilter(TextureFilter::LINEAR),
				WrapS(TextureWrap::CLAMP),
				WrapT(TextureWrap::CLAMP),
				WrapR(TextureWrap::CLAMP),
				Anisotropic(0),
				CompareToRef(false),
				CompareMode(CompareFunc::LESS),
				BorderColor()
			{}
		}SamplerDesc;

		enum class TextureTarget
		{
			TEXTURE1D,
			TEXTURE2D,
			TEXTURE3D,
			TEXTURE1D_ARRAY,
			TEXTURE2D_ARRAY,
			RECTANGLE,
			CUBE_MAP,
			CUBE_MAP_ARRAY,
			TEXTURE2D_MULTISAMPLE,
			TEXTURE2D_MULTISAMPLE_ARRAY,
		};
		extern "C" GLenum ConvertTextureTargetToGLenum(TextureTarget func);
		extern "C" TextureTarget ConvertGLenumToTextureTarget(GLenum func);

		static const GLenum TEXTURE_TARGETS[] =
		{
			GL_TEXTURE_1D,
			GL_TEXTURE_2D,
			GL_TEXTURE_3D,
			GL_TEXTURE_1D_ARRAY,
			GL_TEXTURE_2D_ARRAY,
			GL_TEXTURE_RECTANGLE,
			GL_TEXTURE_CUBE_MAP,
			GL_TEXTURE_CUBE_MAP_ARRAY,
			GL_TEXTURE_2D_MULTISAMPLE,
			GL_TEXTURE_2D_MULTISAMPLE_ARRAY
		};

		enum class BufferUsage
		{
			STREAM_DRAW,
			STREAM_READ,
			STREAM_COPY,
			STATIC_DRAW,
			STATIC_READ,
			STATIC_COPY,
			DYNAMIC_DRAW,
			DYNAMIC_READ,
			DYNAMIC_COPY,
		};
		extern "C" GLenum ConvertBufferUsageToGLenum(BufferUsage func);
		extern "C" BufferUsage ConvertGLenumToBufferUsage(GLenum func);

#define GEN_VERTEX_VNT_DATA(v, n, t) \
	typedef struct MeshDataV##v##N##n##T##t\
			{\
		glm::vec##v f##v##Position; \
		glm::vec##n f##n##Normal;    \
		glm::vec##t f##t##Texcoord; \
			}MeshDataV##v##N##n##T##t; 

#define GEN_VERTEX_VT_DATA(v, t) \
	typedef struct MeshDataV##v##T##t\
			{\
		glm::vec##v f##v##Position; \
		glm::vec##t f##t##Texcoord; \
			}MeshDataV##v##T##t; 

		GEN_VERTEX_VNT_DATA(3, 3, 2);
		GEN_VERTEX_VNT_DATA(4, 3, 2);
		GEN_VERTEX_VT_DATA(2, 2);

		template<typename VertexType>
		struct VertexData
		{
			unsigned uiVertexCount;
			const VertexType* pVertexData;
			unsigned uiElementCount;
			DataType ElementType;
			const unsigned char* pElementData;
		};
	}
}