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

		typedef struct BlendDesc
		{
			bool AlphaToCoverageEnable;
			bool IndependentBlendEnable;

			RenderTargetBlendDesc RenderTargets[8];
			BlendDesc() :
				AlphaToCoverageEnable(false),
				IndependentBlendEnable(false)
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

		typedef struct ClipPlaneDesc
		{
			double NormX, NormY, NormZ, W;
			bool ClipPlaneEnable;

			ClipPlaneDesc() :
				NormX(0), NormY(0), NormZ(0), W(0),
				ClipPlaneEnable(false){}
		}ClipPlaneDesc;

		typedef struct ColorMask
		{
			bool RedWriteMask;
			bool GreenWriteMask;
			bool BlueWriteMask;
			bool AlphaWriteMask;

			ColorMask() :
				RedWriteMask(true), GreenWriteMask(true),
				BlueWriteMask(true), AlphaWriteMask(true)
			{}

			ColorMask(bool r, bool g, bool b, bool a) :
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

		typedef struct RasterizerDesc
		{
			PolygonMode FillMode;
			FaceMode CullMode;
			bool FrontCounterClockwise;
			bool CullFaceEnable;
			//			float DepthBias;
			//			float depthBiasClamp;
			//			float slopeScaledDepthBias;
			bool DepthClampEnable;
			DepthRangeDesc DepthRange;
			bool ScissorEnable;
			bool MultisampleEanble;
			bool AntialiasedLineEnable;
			bool AntialiasedPolygonEnable;
			bool Dither;
			bool SRGB;
			bool PolygonOffsetEnable;
			bool RasterizedDiscardEnable;
			bool ProgramPointSizeEnable;
			bool PrimitiveRestartEnable;
			//			ClipPlaneDesc ClipPlanes[6];
			PolygonOffsetDesc PolygonOffset;
			LogicDesc LogicMode;
			Rectangle2i ScissorBox;
			ColorMask ColorWriteMask;
			PointSpriteDesc PointSprite;

			RasterizerDesc() :
				FillMode(PolygonMode::FILL),
				CullMode(FaceMode::BACK),
				FrontCounterClockwise(false),  // ccw
				CullFaceEnable(false),
				//				DepthBias(0.0f),
				DepthClampEnable(false),
				DepthRange(),
				Dither(false),
				ScissorEnable(false),
				SRGB(false),
				MultisampleEanble(false),
				AntialiasedLineEnable(false),
				LogicMode(),
				PolygonOffsetEnable(false),
				PolygonOffset(),
				RasterizedDiscardEnable(false),
				ProgramPointSizeEnable(false),
				PrimitiveRestartEnable(false),
				ScissorBox(),
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
			GLuint UnpackSkipImages;
			GLuint UnpackCompressedBlockWidth;
			GLuint UnpackCompressedBlockHeight;
			GLuint UnpackCompressedBlockDepth;
			GLuint UnpackCompressedBlockSize;
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


		typedef	struct AttribDesc
		{
			bool Enable;
			GLuint Index;

		}AttribDesc;

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
	}
}