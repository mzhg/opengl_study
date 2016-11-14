
#include "BaseApp.h"
#include <stdio.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace jet{
	namespace util
	{
		/*
		class Monitor
		{
		public:
			static GLFWmonitor* g_pPrimaryMonitor;

			Monitor(GLFWmonitor* moniter)
			{
				m_pHandler = moniter;
			}

			bool isPrimary() const { return m_pHandler == g_pPrimaryMonitor; }
//			operator GLFWmonitor*() { return m_pHandler; }
			GLFWmonitor* get() const { return m_pHandler; }

		private:
			static std::vector<Monitor> g_pMonitors;
			GLFWmonitor* m_pHandler;

		private:
			static void retriveMonitors()
			{
				g_pPrimaryMonitor = glfwGetPrimaryMonitor();

				GLint size;
				GLFWmonitor** monitors = glfwGetMonitors(&size);
				for (int i = 0; i < size; i++)
				{
					g_pMonitors.push_back(Monitor(*monitors));
					monitors++;
				}
			}

		public:

			//
			// Should call glfwInit() first before call this method.
			// @return
			
			static const Monitor& getPrimaryMonitor()
			{

				static Monitor g_empty = Monitor(nullptr);
				if (g_pMonitors.size() == 0)
				{
					retriveMonitors();
				}

				for (int i = 0; i < static_cast<int>(g_pMonitors.size()); i++)
				{
					Monitor& m = g_pMonitors[i];
					if (m.isPrimary())
						return m;
				}

				return g_empty;
			}

		};
	*/
//		extern void SetupCallbacks(BaseApp* app);
		int BaseApp::Run(BaseApp* app, const char* pTitle, const GLContextConfig& desc)
		{
//			const int EXIT_FAILURE = -1;
			GLFWwindow* window;
			GLFWmonitor* monitor = NULL;
			GLContextConfig& config = app->m_ConfigDesc;

			bool& running = app->m_bRunning;
			bool& glfwInited = app->m_bGLfwInited;
			bool& needCreateWindow = app->m_bNeedCreateWindow;
			bool& windowCreated = app->m_bWindowCreated;
			bool fullScreenMode = app->m_bFullScreenMode;

			running = true;
			bool lwjglInited = false;

			while (running){
				if (!glfwInited){
					//    			GLFW.glfwSetErrorCallback(Callbacks.errorCallbackPrint(System.err));

					if (!glfwInit())
					{
						fprintf(stderr, "Unable to initialize GLFW\n");
						return EXIT_FAILURE;
					}

					glfwInited = true;
				}

				bool needResize = false;
				if (needCreateWindow || !windowCreated)
				{ // re-create window.
					if (windowCreated){
						glfwDestroyWindow(window);
					}

					glfwDefaultWindowHints();
					glfwWindowHint(GLFW_RESIZABLE, app->m_bResizeable ? 1 : 0);

					glfwWindowHint(GLFW_RED_BITS, config.RedBits);
					glfwWindowHint(GLFW_GREEN_BITS, config.GreenBits);
					glfwWindowHint(GLFW_BLUE_BITS, config.BlueBits);
					glfwWindowHint(GLFW_ALPHA_BITS, config.AlphaBits);

					glfwWindowHint(GLFW_DEPTH_BITS, config.DepthBits);
					glfwWindowHint(GLFW_STENCIL_BITS, config.StencilBits);

					glfwWindowHint(GLFW_AUX_BUFFERS, config.AuxBuffers);
					glfwWindowHint(GLFW_SAMPLES, config.MultiSamplers);

					glfwWindowHint(GLFW_STEREO, config.Stereo ? 1 : 0);
					glfwWindowHint(GLFW_SRGB_CAPABLE, config.sRGBCapable ? 1 : 0);

					glfwWindowHint(GLFW_CLIENT_API, config.ClientAPI);
					glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, config.DebugContext ? 1 : 0);
//					glfwWindowHint(GLFW_OPENGL_PROFILE, config.glProfile);

					if (fullScreenMode){
						if (monitor == nullptr)
							monitor = glfwGetPrimaryMonitor();
					}

					// Create the window
					window = glfwCreateWindow(config.Width, config.Height, pTitle, monitor, 0);
					if (window == nullptr)
					{
						fprintf(stderr, "Failed to create the GLFW window\n");
						return EXIT_FAILURE;
					}

					app->m_pWindow = window;
					windowCreated = true;

					if (!fullScreenMode){
//						VideoMode video = Monitor.getPrimaryMonitor().getVideoMode();
						GLFWmonitor* monitor = glfwGetPrimaryMonitor();
						const GLFWvidmode* video = glfwGetVideoMode(monitor);

						// Center our window
						glfwSetWindowPos(
							window,
							(video->width - config.Width) / 2,
							(video->height - config.Height) / 2
							);
					}

					// Make the OpenGL context current
					glfwMakeContextCurrent(window);

					// Make the window visible
					glfwShowWindow(window);
					SetupCallbacks(app);

					needCreateWindow = false;
					needResize = true;
				}

				if (!lwjglInited){
					lwjglInited = true;

					// This line is critical for LWJGL's interoperation with GLFW's
					// OpenGL context, or any context that is managed externally.
					// LWJGL detects the context that is current in the current thread,
					// creates the ContextCapabilities instance and makes the OpenGL
					// bindings available for use.
					// GLContext.createFromCurrent();
//					GL.createCapabilities();  
					glewInit();

					app->onCreate();
				}

				if (needResize){
					needResize = false;
					if (running)
						app->onResize(config.Width, config.Height);
				}

				/*
				block();
				renderFrame();
				*/

				app->onRender();

				glfwSwapBuffers(window);
				// Poll for window events. The key callback above will only be
				// invoked during this call.
				glfwPollEvents();
				

				if (glfwWindowShouldClose(window)){
					running = false;
					break;
				}
			}

			for (int i = 0; i < 1/*glListeners.size()*/; i++){
				app->onDispose();
			}

			// Release window and window callbacks
			glfwDestroyWindow(window);
			// Terminate GLFW and release the GLFWerrorfun
			glfwTerminate();

			return 0;
		}

		int BaseApp::Run(BaseApp* app, const char* pTitle, GLuint width, GLuint height)
		{
			GLContextConfig desc;
			desc.Width = width;
			desc.Height = height;

			return BaseApp::Run(app, pTitle,desc);
		}

		/*
		void BaseApp::onresize(GLFWwindow* window, int width, int height)
		{
			this->onResize(static_cast<GLuint>(width), static_cast<GLuint>(height));
		}
		*/

		void BaseApp::onUpdate(float) {}
	}
};