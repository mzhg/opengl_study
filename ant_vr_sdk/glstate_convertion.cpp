#include "GLStates.h"
#include "GLUtil.h"

namespace jet
{
	namespace util
	{
		extern "C"
		{
			GLenum ConvertBlendFuncToGLenum(BlendFunc enumValue)
			{
				switch (enumValue)
				{
				case BlendFunc::ZERO: return GL_ZERO;
				case BlendFunc::ONE: return GL_ONE;
				case BlendFunc::SRC_ALPHA: return GL_SRC_ALPHA;
				case BlendFunc::SRC_ALPHA_INVERT: return GL_ONE_MINUS_SRC_ALPHA;
				case BlendFunc::SRC_COLOR: return GL_SRC_COLOR;
				case BlendFunc::SRC_COLOR_INVERT: return GL_ONE_MINUS_SRC_COLOR;
				case BlendFunc::DST_COLOR: return GL_DST_COLOR;
				case BlendFunc::DST_COLOR_INVERT: return GL_ONE_MINUS_DST_COLOR;
				case BlendFunc::DST_ALPHA: return GL_DST_ALPHA;
				case BlendFunc::DST_ALPHA_INVERT: return GL_ONE_MINUS_DST_ALPHA;
				case BlendFunc::CONSTANT_COLOR: return GL_CONSTANT_COLOR;
				case BlendFunc::CONSTANT_COLOR_INVERT: return GL_ONE_MINUS_CONSTANT_COLOR;
				case BlendFunc::CONSTANT_ALPHA: return GL_CONSTANT_ALPHA;
				case BlendFunc::CONSTANT_ALPHA_INVERT: return GL_ONE_MINUS_CONSTANT_ALPHA;

				default:
					assert(false);
					break;
				}
			}

			BlendFunc ConvertGLenumToBlendFunc(GLenum enumValue)
			{
				switch (enumValue)
				{
				case GL_ZERO: return BlendFunc::ZERO;
				case GL_ONE: return BlendFunc::ONE;
				case GL_SRC_ALPHA: return BlendFunc::SRC_ALPHA;
				case GL_ONE_MINUS_SRC_ALPHA: return BlendFunc::SRC_ALPHA_INVERT;
				case GL_SRC_COLOR: return BlendFunc::SRC_COLOR;
				case GL_ONE_MINUS_SRC_COLOR: return BlendFunc::SRC_COLOR_INVERT;
				case GL_DST_COLOR: return BlendFunc::DST_COLOR;
				case GL_ONE_MINUS_DST_COLOR: return BlendFunc::DST_COLOR_INVERT;
				case GL_DST_ALPHA: return BlendFunc::DST_ALPHA;
				case GL_ONE_MINUS_DST_ALPHA: return BlendFunc::DST_ALPHA_INVERT;
				case GL_CONSTANT_COLOR: return BlendFunc::CONSTANT_COLOR;
				case GL_ONE_MINUS_CONSTANT_COLOR: return BlendFunc::CONSTANT_COLOR_INVERT;
				case GL_CONSTANT_ALPHA: return BlendFunc::CONSTANT_ALPHA;
				case GL_ONE_MINUS_CONSTANT_ALPHA: return BlendFunc::CONSTANT_ALPHA_INVERT;

				default:
					assert(false);
					break;
				}
			}
			GLenum ConvertBlendOpFuncToGLenum(BlendOpFunc enumValue)
			{
				switch (enumValue)
				{
				case BlendOpFunc::ADD: return GL_FUNC_ADD;
				case BlendOpFunc::SUBTRACT: return GL_FUNC_SUBTRACT;
				case BlendOpFunc::SUBTRACT_REVERSE: return GL_FUNC_REVERSE_SUBTRACT;
				case BlendOpFunc::MIN: return GL_MIN;
				case BlendOpFunc::MAX: return GL_MAX;

				default:
					assert(false);
					break;
				}
			}

			BlendOpFunc ConvertGLenumToBlendOpFunc(GLenum enumValue)
			{
				switch (enumValue)
				{
				case GL_FUNC_ADD: return BlendOpFunc::ADD;
				case GL_FUNC_SUBTRACT: return BlendOpFunc::SUBTRACT;
				case GL_FUNC_REVERSE_SUBTRACT: return BlendOpFunc::SUBTRACT_REVERSE;
				case GL_MIN: return BlendOpFunc::MIN;
				case GL_MAX: return BlendOpFunc::MAX;

				default:
					assert(false);
					break;
				}
			}

			GLenum ConvertStencilOpToGLenum(StencilOp enumValue)
			{
				switch (enumValue)
				{
				case StencilOp::KEEP: return GL_KEEP;
				case StencilOp::ZERO: return GL_ZERO;
				case StencilOp::REPLACE: return GL_REPLACE;
				case StencilOp::INCR: return GL_INCR;
				case StencilOp::INCR_WRAP: return GL_INCR_WRAP;
				case StencilOp::DECR: return GL_DECR;
				case StencilOp::DECR_WRAP: return GL_DECR_WRAP;
				case StencilOp::INVERT: return GL_INVERT;

				default:
					assert(false);
					break;
				}
			}

			StencilOp ConvertGLenumToStencilOp(GLenum enumValue)
			{
				switch (enumValue)
				{
				case GL_KEEP: return StencilOp::KEEP;
				case GL_ZERO: return StencilOp::ZERO;
				case GL_REPLACE: return StencilOp::REPLACE;
				case GL_INCR: return StencilOp::INCR;
				case GL_INCR_WRAP: return StencilOp::INCR_WRAP;
				case GL_DECR: return StencilOp::DECR;
				case GL_DECR_WRAP: return StencilOp::DECR_WRAP;
				case GL_INVERT: return StencilOp::INVERT;

				default:
					assert(false);
					break;
				}
			}
			GLenum ConvertCompareFuncToGLenum(CompareFunc enumValue)
			{
				switch (enumValue)
				{
				case CompareFunc::NEVER: return GL_NEVER;
				case CompareFunc::LESS: return GL_LESS;
				case CompareFunc::LEQUAL: return GL_LEQUAL;
				case CompareFunc::GREATER: return GL_GREATER;
				case CompareFunc::GEQUAL: return GL_GEQUAL;
				case CompareFunc::EQUAL: return GL_EQUAL;
				case CompareFunc::NOTEQUAL: return GL_NOTEQUAL;
				case CompareFunc::ALWAYS: return GL_ALWAYS;

				default:
					assert(false);
					break;
				}
			}

			CompareFunc ConvertGLenumToCompareFunc(GLenum enumValue)
			{
				switch (enumValue)
				{
				case GL_NEVER: return CompareFunc::NEVER;
				case GL_LESS: return CompareFunc::LESS;
				case GL_LEQUAL: return CompareFunc::LEQUAL;
				case GL_GREATER: return CompareFunc::GREATER;
				case GL_GEQUAL: return CompareFunc::GEQUAL;
				case GL_EQUAL: return CompareFunc::EQUAL;
				case GL_NOTEQUAL: return CompareFunc::NOTEQUAL;
				case GL_ALWAYS: return CompareFunc::ALWAYS;

				default:
					assert(false);
					break;
				}
			}
			GLenum ConvertPolygonModeToGLenum(PolygonMode enumValue)
			{
				switch (enumValue)
				{
				case PolygonMode::POINT: return GL_POINT;
				case PolygonMode::LINE: return GL_LINE;
				case PolygonMode::FILL: return GL_FILL;

				default:
					assert(false);
					break;
				}
			}

			PolygonMode ConvertGLenumToPolygonMode(GLenum enumValue)
			{
				switch (enumValue)
				{
				case GL_POINT: return PolygonMode::POINT;
				case GL_LINE: return PolygonMode::LINE;
				case GL_FILL: return PolygonMode::FILL;

				default:
					assert(false);
					break;
				}
			}
			GLenum ConvertFaceModeToGLenum(FaceMode enumValue)
			{
				switch (enumValue)
				{
				case FaceMode::NONE: return GL_NONE;
				case FaceMode::FRONT: return GL_FRONT;
				case FaceMode::BACK: return GL_BACK;
				case FaceMode::FRONT_AND_BACK: return GL_FRONT_AND_BACK;

				default:
					assert(false);
					break;
				}
			}

			FaceMode ConvertGLenumToFaceMode(GLenum enumValue)
			{
				switch (enumValue)
				{
				case GL_NONE: return FaceMode::NONE;
				case GL_FRONT: return FaceMode::FRONT;
				case GL_BACK: return FaceMode::BACK;
				case GL_FRONT_AND_BACK: return FaceMode::FRONT_AND_BACK;

				default:
					assert(false);
					break;
				}
			}
			GLenum ConvertLogicFuncToGLenum(LogicFunc enumValue)
			{
				switch (enumValue)
				{
				case LogicFunc::CLEAR: return GL_CLEAR;
				case LogicFunc::SET: return GL_SET;
				case LogicFunc::COPY: return GL_COPY;
				case LogicFunc::COPY_INVERT: return GL_COPY_INVERTED;
				case LogicFunc::NOOP: return GL_NOOP;
				case LogicFunc::INVERT: return GL_INVERT;
				case LogicFunc::AND: return GL_AND;
				case LogicFunc::NAND: return GL_NAND;
				case LogicFunc::OR: return GL_OR;
				case LogicFunc::NOR: return GL_NOR;
				case LogicFunc::XOR: return GL_XOR;
				case LogicFunc::EQUIV: return GL_EQUIV;
				case LogicFunc::AND_REVERSE: return GL_AND_REVERSE;
				case LogicFunc::AND_INVERT: return GL_AND_INVERTED;
				case LogicFunc::OR_REVERSE: return GL_OR_REVERSE;
				case LogicFunc::OR_INVERT: return GL_OR_INVERTED;

				default:
					assert(false);
					break;
				}
			}

			LogicFunc ConvertGLenumToLogicFunc(GLenum enumValue)
			{
				switch (enumValue)
				{
				case GL_CLEAR: return LogicFunc::CLEAR;
				case GL_SET: return LogicFunc::SET;
				case GL_COPY: return LogicFunc::COPY;
				case GL_COPY_INVERTED: return LogicFunc::COPY_INVERT;
				case GL_NOOP: return LogicFunc::NOOP;
				case GL_INVERT: return LogicFunc::INVERT;
				case GL_AND: return LogicFunc::AND;
				case GL_NAND: return LogicFunc::NAND;
				case GL_OR: return LogicFunc::OR;
				case GL_NOR: return LogicFunc::NOR;
				case GL_XOR: return LogicFunc::XOR;
				case GL_EQUIV: return LogicFunc::EQUIV;
				case GL_AND_REVERSE: return LogicFunc::AND_REVERSE;
				case GL_AND_INVERTED: return LogicFunc::AND_INVERT;
				case GL_OR_REVERSE: return LogicFunc::OR_REVERSE;
				case GL_OR_INVERTED: return LogicFunc::OR_INVERT;

				default:
					assert(false);
					break;
				}
			}
		}
	}
}