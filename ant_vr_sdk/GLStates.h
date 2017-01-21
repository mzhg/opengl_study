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
			ONE,
			ZERO,
			SRC_ALPHA,
			SRC_ALPHA_INVERT,
			SRC_COLOR,
			SRC_COLOR_INVERT,
		};

		enum class BlendOpFunc
		{
			ADD,
			MINUS,
			MUL,
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
				for (int i = 0; i < 8; i++)
				{
					RenderTargets[i] = RenderTargetBlendDesc();
				}
			}
		}BlendDesc;

		enum class StencilOp
		{
			INCR,
			INCR_WRAP,
			DECR,
			DECR_WRAP,
			KEEP,
		};

		enum class CompareFunc
		{
			NEVER,
			ALWAYS,
			LESS,
			LESS_EQUAL,
			GREATER,
			GREATER_EQUAL,
			EQUAL,
			NOTEQUAL,
		};

		typedef struct DepthStencilOPDesc
		{
			union 
			{
				struct
				{
					StencilOp StencilFailOp : 8;
					StencilOp StencilDepthFailOp : 8;
					StencilOp StencilPassOp : 8;
					CompareFunc StencilFunc : 8;
				};

				int value;
			};

			DepthStencilOPDesc():
				StencilFailOp(StencilOp::KEEP),
				StencilDepthFailOp(StencilOp::KEEP),
				StencilPassOp(StencilOp::KEEP),
				StencilFunc(CompareFunc::ALWAYS)
			{}
		}DepthStencilOPDesc;

		typedef struct DepthStencilDesc
		{
			bool DepthEnable;
			bool DepthWriteMask;
			CompareFunc DepthFunc;
			bool StencilEnable;
			int StencilMask;
			DepthStencilOPDesc FrontFace;
			DepthStencilOPDesc BackFace;

			DepthStencilDesc() :
				DepthEnable(false),
				DepthWriteMask(true),
				DepthFunc(CompareFunc::LESS),
				StencilEnable(false),
				StencilMask(0xFF),
				FrontFace(),
				BackFace()
			{}
		}DepthStencilDesc;

		enum class PolygonMode
		{
			FILL,
			LINE
		};

		enum class FaceMode
		{
			NONE,
			FRONT,
			BACK,
			FRONT_AND_BACK,
		};

		enum class LogicFunc
		{
			CLEAR,
			SET,
			COPY,
			COPY_INVERT,
			NOOP,
			INVERT,
			AND,
			MAND,
			OR,
			NOR,
			XOR,
			EQUIV,
			AND_REVERSE,
			AND_INVERT,
			OR_REVERSE,
			OR_INVERT,
		};

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
			float NearVal;
			float FarVal;

			DepthRangeDesc() : NearVal(0.0f), FarVal(1.0f){}
		}DepthRangeDesc;

		typedef struct PolygonOffsetDesc
		{
			float factor;
			float units;
			PolygonOffsetDesc() :
				factor(0.0f), units(0.0f){}
		}PolygonOffsetDesc;

		typedef struct RasterizerDesc
		{
			PolygonMode FillMode;
			FaceMode CullMode;
			bool FrontCounterClockwise;
			float DepthBias;
//			float depthBiasClamp;
//			float slopeScaledDepthBias;
			bool DepthClampEnable;
			DepthRangeDesc DepthRange;
			bool ScissorEnable;
			bool MultisampleEanble;
			bool AntialiasedLineEnable;
			bool AntialiasedPolygonEnable;
			bool ClipPlaneEnable;
			bool Dither;
			bool SRGB;
			bool PolygonOffsetEnable;
			bool RasterizedDiscardEnable;
			bool ProgramPointSizeEnable;
			bool PrimitiveRestartEnable;
			PolygonOffsetDesc PolygonOffset;
			Plane3f ClipPlanes[6];
			LogicDesc LogicMode;

			RasterizerDesc():
				FillMode(PolygonMode::FILL),
				CullMode(FaceMode::BACK),
				FrontCounterClockwise(false),  // ccw
				DepthBias(0.0f),
				DepthClampEnable(false),
				DepthRange(),
				Dither(false),
				ScissorEnable(false),
				SRGB(false),
				MultisampleEanble(false),
				AntialiasedLineEnable(false),
				ClipPlaneEnable(false),
				LogicMode(),
				PolygonOffsetEnable(false),
				PolygonOffset(),
				RasterizedDiscardEnable(false),
				ProgramPointSizeEnable(false),
				PrimitiveRestartEnable(false)
			{}
		}RasterizerDesc;

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

			void setDSState(const DepthStencilOPDesc* pDS);
			void restoreDSState();
			void resetDSState(bool force = false);

			void setRSState(const RasterizerDesc* pRaster);
			void restoreRSState();
			void resetRSState(bool force = false);

			void setViewport(GLint x, GLint y, GLint width, GLint height);
			void setViewports(unsigned int count, const Rectangle2i* pViewports);
			void restoreViewport(unsigned int index = 0);

			void bindProgram(GLuint program);
			void restoreProgram();
			void resetProgram(bool force = false);

			void bindFramebuffer(GLuint framebuffer);
			void restoreFramebuffer();
			void resetFramebuffer(bool force = false);
			void bindTextures(unsigned int count, const TextureGL* pTextures, const int* units = nullptr);
			void restoreTextures();
			void resetTextures(bool force = false);

			BlendDesc getBlendState() const { return m_BlendState; }
			DepthStencilOPDesc getDepthStencilState() const { return m_DepthStencilState; }
			RasterizerDesc getRasterizerState() const { return m_RasterizerState; }

			GLuint getProgram() const { return m_ProgramState; }
			GLuint getFramebuffer() const { return m_FramebufferState; }

			~GLStates();

		private:

			void setBlendState(const BlendDesc& blend);
			void setDSState(const DepthStencilOPDesc& ds);
			void setRSState(const RasterizerDesc& raster);


			GLStates();
			GLStates(GLStates&) = delete;
			GLStates operator=(GLStates&) = delete;

			BlendDesc m_BlendState;
			DepthStencilOPDesc m_DepthStencilState;
			RasterizerDesc  m_RasterizerState;

			GLuint m_ProgramState;  // Current program
			GLuint m_FramebufferState;  // Current framebuffer object

			struct BindTexture
			{
				GLenum Target;
				GLuint TextureID;
			};

			BindTexture m_TextureStates[80];
			Rectangle2i m_ViewportState;
		};

	}
}

