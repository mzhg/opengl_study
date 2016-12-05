#pragma once
#ifndef _JET_UTIL_POSTPROCESSING_FRAMEWORK_H
#define _JET_UTIL_POSTPROCESSING_FRAMEWORK_H

#include <stdint.h>
#include "TextureUtil.h"
#include <map>
#include <vector>
#include <memory>
#include "Util.h"
#include "GLSLProgram.h"
#include "AssetLoader.h"
#include "GLUtil.h"

namespace jet
{
	namespace util
	{
// #define  ENABLE_PROGRAM_PIPELINE 0

		/// Bad name, later we will use RenderTextrue to instead.
		class RenderTarget : public Texture2D
		{
		public:
			~RenderTarget() { dispose(); }
		};

		/// Bad name, later we will use RenderTextruePool to instead.
		class RenderTargetPool
		{
		public:
			~RenderTargetPool();

			void findFreeElement(const Texture2DDesc& pDesc, std::unique_ptr<RenderTarget>& out);
			void freeUnusedResource(std::unique_ptr<RenderTarget>& in)
			{
				Texture2DDesc desc = in->getDesc();
				m_RenderTargetPool[desc] = std::move(in);
			}

			static RenderTargetPool* getInstance();
			static void shutDown();

		private:
			RenderTargetPool() {}
			RenderTargetPool(RenderTargetPool&) = delete;

			std::map<Texture2DDesc, std::unique_ptr<RenderTarget>> m_RenderTargetPool;

			static RenderTargetPool* m_Instance;
		};

#if 0
		enum PassName
		{
			INPUT_NONE,
			INPUT_COLOR0,
			INPUT_COLOR1,
			INPUT_COLOR2,
			INPUT_COLOR3,
			INPUT_COLOR4,
			INPUT_COLOR5,
			INPUT_COLOR6,
			INPUT_COLOR7,
			INPUT_COLOR8,
			INPUT_DEPTH,

			GAUSS_BLUR,
		};
#else
		typedef int PassName;
#define INPUT_NONE -1
#define INPUT_COLOR0 0
#define INPUT_DEPTH  8
#define GAUSS_BLUR   9
#endif

		struct InputDesc
		{
			PassName DependencyPass;
			uint32_t Slot;
		};

		struct PPRenderPassDesc
		{
			uint32_t InputCount;
			InputDesc InputDescs[8];

			uint32_t OutputCount;
			PassName Name;
		};


		class PostProcessingParameters
		{
		public:
			friend class PostProcessing;

			int getGaussBlurKernal() const  { return GaussBlur_Kernal; }
		private:

			int GaussBlur_Kernal;
		};

		class DefaultScreenQuadVertexShader : public VertexShader
		{
		public:
			void init()
			{
				GLSLProgram::createShaderFromFile("DefaultScreenSpaceVS.vert", this);
			}
		};

		//		template <uint32_t InputCount, uint32_t OutputCount>
		class PPRenderPass
		{
		public:
			PPRenderPass(PassName name)
			{
				memset(&m_PassDesc, 0, sizeof(PPRenderPassDesc));
				m_PassDesc.Name = name;
			}

			PPRenderPassDesc getPassDesc() const { return m_PassDesc; }
			PassName getName() const { return m_PassDesc.Name; }

			friend class PPRenderContext;
			friend class PostProcessing;
		protected:

			void set(uint32_t inputCount, uint32_t outputCount)
			{
				m_PassDesc.InputCount = inputCount;
				m_PassDesc.OutputCount = outputCount;

				for (uint32_t i = 0; i < inputCount; i++)
				{
					m_PassDesc.InputDescs[i] = { INPUT_NONE, 0 };
				}
			}

			Texture2D* getInput(int idx)
			{
				int length = m_PassDesc.InputCount;
				return idx >= length ? NULL : m_PassInputs[idx];
			}

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters) = 0;
			virtual void drawQuad();

			RenderTarget* getOutputTexture(int idx)
			{
				int length = m_PassDesc.OutputCount;
				return idx >= length ? NULL : m_PassOutputs[idx].get();
			}

			virtual Texture2D* getOutput(int idx)
			{
				return getOutputTexture(idx);
			}

			virtual void releaseResource()
			{
				int length = m_PassDesc.InputCount;
				for (int i = 0; i < length; i++)
				{
					RenderTargetPool::getInstance()->freeUnusedResource(m_PassOutputs[i]);
				}
			}

			virtual void computeOutDesc(int index, Texture2DDesc& out) = 0;

		private:
			PPRenderPass(PPRenderPass&) = delete;
			void _process(PPRenderContext* context, const PostProcessingParameters& parameters)
			{
				// TODO other stuff to do here.
				process(context, parameters);
				m_bProcessed = true;
			}

			void setDependency(int slot, PassName name, int depentSlot)
			{
				m_PassDesc.InputDescs[slot].DependencyPass = name;
				m_PassDesc.InputDescs[slot].Slot = depentSlot;
			}

			void reset()
			{
				m_Dependencies = m_FixDependencies;
				m_bProcessed = false;
			}

			void addDependency()
			{
				++m_Dependencies;
			}

			void setDependencies(int dependencies) { m_FixDependencies = dependencies; }
			int getDependencyCount() const
			{
				return m_Dependencies;
			}

			bool resolveDependencies()
			{
				if (m_Dependencies > 0)
				{
					--m_Dependencies;

					if (m_Dependencies == 0)
					{
						// the internal reference is released
						//				pooledRenderTarget.safeRelease();
						//				pooledRenderTarget = null;
						return true;
					}
				}

				return false;
			}

			void setInputTextures(Texture2D** _inputTextures)
			{
				//				mInputTextures = _inputTextures; 
				for (uint32_t i = 0; i < m_PassDesc.InputCount; i++)
				{
					m_PassInputs[i] = _inputTextures[i];
				}
			}

			void setOutputRenderTexture(int slot, std::unique_ptr<RenderTarget>& texture)
			{
				m_PassOutputs[slot] = std::move(texture);
			}
		protected:
			PPRenderPassDesc m_PassDesc;
		private:
			int m_Dependencies;
			int m_FixDependencies;
			bool m_bProcessed;

			Texture2D* m_PassInputs[8];
			std::unique_ptr<RenderTarget> m_PassOutputs[8];
		};

		class PPRenderPassInput : public PPRenderPass
		{
		public:
			PPRenderPassInput(Texture2D* texture, uint32_t index = 0) : PPRenderPass(static_cast<PassName>(INPUT_COLOR0 + index)),
				m_ptex2D(texture)
			{
				// 0 input; 1 output.
				set(0, 0);
			}

			virtual void process(PPRenderContext* context, const PostProcessingParameters& parameters) override {/* Nothing need to do here. */ }

			Texture2D* getOutput(int idx) override
			{
				return m_ptex2D;
			}

			void computeOutDesc(int index, Texture2DDesc& out) override
			{
				out = m_ptex2D->getDesc();
			}

		private:

			Texture2D* m_ptex2D;
		};

		class PPRenderContext
		{

		public:

			// The function only called once.
			void initlizeGL();

			void begin();

			void setShader(PixelShader* pPixelShader, VertexShader* pVertexShader = NULL)
			{
				assert(m_InBeginBlock);
				if (pVertexShader == NULL)
				{
					pVertexShader = m_pDefaultScreenQuadVS;
				}

				m_Program->setRenderShader(pVertexShader, pPixelShader);
			}

			void setUniform2f(const char* name, float x, float y) {m_Program->setUniform2f(name, x,y);}
			void setUniform1i(const char* name, int x)			  {m_Program->setUniform1i(name, x);}
#if ENABLE_PROGRAM_PIPELINE
			void setActiveShader(GLenum target) { m_Program->setActiveShader(target); }
#endif
			void setRenderTargets(uint32_t count, const RenderTarget** pColorTargets, const RenderTarget* pDepthStencilTarget);

			void clearColorTarget(uint32_t location, const GLfloat clearColors[4]);

			void setParameters(class PostProcessingParameters* pParameters) { m_Parameters = pParameters; }

			void clearDepthStencilTarget(float clearDepth, int clearStencil);

			void drawQuad();

			// unbind resources.
			void end(bool renderQuad = false);

			void setRenderPasses(std::vector<std::shared_ptr<PPRenderPass>>& inRenderPasses)
			{
				m_RenderPasses.clear();
				m_RenderPassList.clear();

				size_t count = inRenderPasses.size();
				for (int i = 0; i < count; i++)
				{
					std::shared_ptr<PPRenderPass>& pass = inRenderPasses[i];

					m_RenderPasses[pass->getName()] = pass;
					m_RenderPassList.push_back(pass);
				}
			}

			void renderTo(Texture2D* src, Texture2D* dst);

			void performancePostProcessing();

			void shutDown();

		private:
			void resolveDependencies(std::vector<PPRenderPass*>& currentDependencyPasses)
			{
				for (auto it = currentDependencyPasses.begin(); it != currentDependencyPasses.end(); it++)
				{
					if ((*it)->resolveDependencies())
					{
						(*it)->releaseResource();
					}
				}
			}

		private:
			std::map<PassName, std::shared_ptr<PPRenderPass>> m_RenderPasses;
			std::vector<std::shared_ptr<PPRenderPass>> m_RenderPassList;
			std::shared_ptr<PPRenderPass> m_PrevPass;
			std::shared_ptr<PPRenderPass> m_CurrentPass;

			ProgramPipeline* m_Program;
			DefaultScreenQuadVertexShader* m_pDefaultScreenQuadVS;
			PixelShader*				   m_pDefualtScreenQuadPS;

			GLuint m_Framebuffer;

			GLint m_Viewport[4];
			GLuint m_BindingColorTextureCount;
			bool   m_HaveDepthStencilBinding;
			Texture2DDesc m_BindingColorTextureDesc[8];
			Texture2DDesc m_BindingDepthTextureDesc;

			bool m_IsRendered;
			bool m_InBeginBlock;
			bool m_IsSupportVertexID;
			bool m_IsSupportVertexArray;
			GLuint m_VertexArray;
			GLuint m_VertexBuffer;

			class PostProcessingParameters* m_Parameters;
		};
	}
}


#endif



