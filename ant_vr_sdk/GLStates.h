#pragma once
#include <GL\glew.h>
#include "Texture.h"
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

			RenderTargetBlendDesc():
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

			DepthStencilOPDesc():
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

			RasterizerDesc():
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
				ScissorBox()
			{}
		}RasterizerDesc;



		extern "C" GLenum ConvertBlendFuncToGLenum(BlendFunc func);
		extern "C" BlendFunc ConvertGLenumToBlendFunc(GLenum func);
		extern "C" GLenum ConvertBlendOpFuncToGLenum(BlendOpFunc func);
		extern "C" BlendOpFunc ConvertGLenumToBlendOpFunc(GLenum func);

		// Single-ton class.
		class GLStates
		{
		public:
			static GLStates& getGLStates();

			// Retrive all of the gl-states from the opengl context.
			void restoreAll();
			// Reset all of the states to default setting.
			void resetAll(bool force = false);
			// Set the Blend state, pass NULL to make it default.
			void setBlendState(const BlendDesc* pBlend);
			void restoreBlendState();
			void resetBlendState(bool force = false);

			void setDSState(const DepthStencilDesc* pDS);
			void restoreDSState();
			void resetDSState(bool force = false);

			void setRSState(const RasterizerDesc* pRaster);
			void restoreRSState();
			void resetRSState(bool force = false);

			void setViewport(GLint x, GLint y, GLint width, GLint height)
			{
				Rectangle2i viewport = {x,y,width, height};
				setViewports(1, &viewport);
			}
			void setViewports(unsigned int count, const Rectangle2i* pViewports);
			void restoreViewport(unsigned int index = 0);

			void bindProgram(GLuint program);
			void restoreProgram();
			void resetProgram(bool force = false);

			void bindFramebuffer(GLuint framebuffer);
			void restoreFramebuffer();
			void resetFramebuffer(bool force = false);
			
			void bindTextures(unsigned int count, const TextureGL** pTextures, const int* units = nullptr);
			void restoreTextures();
			void resetTextures(bool force = false);

			void bindVAO(GLuint vao);
			void restoreVAO();
			void resetVAO(bool force = false);

			BlendDesc getBlendState() const { return m_BlendState; }
			DepthStencilDesc getDepthStencilState() const { return m_DepthStencilState; }
			RasterizerDesc getRasterizerState() const { return m_RasterizerState; }

			GLuint getProgram() const { return m_ProgramState; }
			GLuint getFramebuffer() const { return m_FramebufferState; }

			~GLStates();

		private:

			void setBlendState(const BlendDesc& blend);
			void setDSState(const DepthStencilDesc& ds);
			void setRSState(const RasterizerDesc& raster);


			GLStates();
			GLStates(GLStates&) = delete;
			GLStates operator=(GLStates&) = delete;

			BlendDesc m_BlendState;
			DepthStencilDesc m_DepthStencilState;
			RasterizerDesc  m_RasterizerState;

			GLuint m_ProgramState;  // Current program
			GLuint m_FramebufferState;  // Current framebuffer object
			GLuint m_VertexArrayState;  // Current Vertex Array Object

			struct BindTexture
			{
				GLenum Target;
				GLuint TextureID;
				GLuint Unit;

				BindTexture() :
					Target(0), TextureID(0), Unit(0){}
			};

			BindTexture m_TextureStates[80];
			unsigned int m_TextureCount;
			Rectangle2i m_ViewportStates[16];
			unsigned int m_ViewportCount;
		};

	}
}

