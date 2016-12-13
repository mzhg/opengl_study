
#include "BaseApp.h"


namespace jet{
	namespace util{

		static GLFWCallback* g_pGLFWCallback = nullptr;
		static BaseApp*      g_BaseApp = nullptr;

		static void onclose(GLFWwindow* window)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->WindowClose();
			}
		}

		static void windowFocus(GLFWwindow* window, int focused)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->WindowFocus(focused != 0);
			}
		}

		static void windowIconify(GLFWwindow* window, int focused)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->WindowIconify(focused!=0);
			}
		}

		static void CursorEntered(GLFWwindow* window, int focused)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->CursorEnter(focused!=0);
			}
		}

		static void WindowPos(GLFWwindow* window, int posx, int posy)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->WindowPos(posx, posy);
			}
		}

		static void CursorPos(GLFWwindow* window, double posx, double posy)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->CursorPos(posx, posy);
			}
		}

		static void Scroll(GLFWwindow* window, double xoffset, double yoffset)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->Scroll(xoffset, yoffset);
			}
		}

		static void Key(GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->Key(key, scancode, action, mods);
			}
		}

		static void MouseButton(GLFWwindow* window, int button, int action, int mods)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->MouseButton(button, action, mods);
			}
		}



		static void CharMods(GLFWwindow* window, unsigned int codepoint, int mods)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_pGLFWCallback->CharMods(codepoint, mods);
			}
		}



		static void onresize(GLFWwindow* window, int width, int height)
		{
			if (g_pGLFWCallback != nullptr)
			{
				g_BaseApp->onResize(width, height);
			}
		}

		void SetupCallbacks(BaseApp* app)
		{
			g_BaseApp = app;
			g_pGLFWCallback = app->m_pGLFWCallback;
			glfwSetWindowCloseCallback(app->m_pWindow, onclose);
			glfwSetWindowFocusCallback(app->m_pWindow, windowFocus);
			glfwSetWindowIconifyCallback(app->m_pWindow, windowIconify);
			glfwSetWindowPosCallback(app->m_pWindow, WindowPos);
			glfwSetCharModsCallback(app->m_pWindow, CharMods);
			glfwSetCursorEnterCallback(app->m_pWindow, CursorEntered);
			glfwSetCursorPosCallback(app->m_pWindow, CursorPos);
			glfwSetKeyCallback(app->m_pWindow, Key);
			glfwSetMouseButtonCallback(app->m_pWindow, MouseButton);
			glfwSetScrollCallback(app->m_pWindow, Scroll);
			glfwSetFramebufferSizeCallback(app->m_pWindow, onresize);
		}
	}
}
