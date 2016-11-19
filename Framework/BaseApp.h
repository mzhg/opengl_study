

#pragma once

#define GLEW_STATIC
// #define GLFW_INCLUDE_ES2
#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stddef.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace jet{
	namespace util{

		typedef struct GLContextConfig
		{
			GLuint RedBits;
			GLuint GreenBits;
			GLuint BlueBits;
			GLuint AlphaBits;

			GLuint DepthBits;
			GLuint StencilBits;

			GLuint RefreshRate;

			GLuint Width;
			GLuint Height;

			GLuint MultiSamplers;

			GLint MajorVersion;
			GLint MinorVersion;

			GLuint AuxBuffers;
			bool sRGBCapable;
			bool IsOpenGLESContext;
			bool   DebugContext;
			bool  Stereo;
//			GLint  GLProfile;

			GLContextConfig() :
				RedBits(8), GreenBits(8), BlueBits(8), AlphaBits(8), DepthBits(16),
				StencilBits(0), RefreshRate(60), Width(1280), Height(720), MultiSamplers(1)
			{
				AuxBuffers = 0;
				sRGBCapable = false;
				IsOpenGLESContext = false;
				DebugContext = true;
				Stereo = false;

				// This only works for opengl es context.
				MajorVersion = 2;
				MinorVersion = 0;
//				GLProfile
			}
		}GLContextConfig;

		typedef struct VideoMode
		{
			GLuint RedBits;
			GLuint GreenBits;
			GLuint BlueBits;

			GLuint RefreshRate;

			GLuint Width;
			GLuint Height;
		}VideoMode;

		class KeyboardCallback
		{

		};

		class MouseCallback
		{

		};

		class BaseApp
		{
			
		public:
			BaseApp() : m_bRunning(false), m_bFullScreenMode(false), m_bResizeable(false),
				m_bGLfwInited(false), m_bWindowCreated(false), m_bNeedCreateWindow(false),
				m_pWindow(nullptr), m_pMonitor(nullptr), m_ConfigDesc()
			{}

			virtual void onCreate() = 0;
			virtual void onResize(GLuint width, GLuint height) = 0;
			virtual void onUpdate(float delta = 0.0);
			virtual void onRender() = 0;
			virtual void onDispose() = 0;

			void setSwapInterval(int interval) { if (m_pWindow) glfwSwapInterval(interval); }
			bool isRunning() { return m_bRunning; }

			static int Run(BaseApp* app, const char* pTitle, GLuint width = 1280, GLuint height = 720);
			static int Run(BaseApp* app, const char* pTitle, const GLContextConfig& desc);

			const GLContextConfig& getConfig() const  { return m_ConfigDesc; }
			GLContextConfig& getConfig()  { return m_ConfigDesc; }

			friend void SetupCallbacks(BaseApp*);

			/*
		private:

			void onresize(GLFWwindow* window, int width, int height);
			*/

		private:
			GLFWwindow* m_pWindow;
			GLFWmonitor* m_pMonitor;
			GLContextConfig m_ConfigDesc;

			MouseCallback* m_pMouseCallback;
			KeyboardCallback* m_pKeyboardCallback;

			bool m_bRunning;
			bool m_bFullScreenMode;
			bool m_bResizeable;
			bool m_bGLfwInited;
			bool m_bWindowCreated;
			bool m_bNeedCreateWindow;
		};
	}
}