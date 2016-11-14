
#include "BaseApp.h"


namespace jet{
	namespace util{

		static BaseApp* g_pApp = nullptr;
		static void onclose(GLFWwindow* window)
		{
			// TODO
		}

		static void onresize(GLFWwindow* window, int width, int height)
		{

		}

		void SetupCallbacks(BaseApp& app)
		{
			g_pApp = &app;
			glfwSetWindowCloseCallback(app.m_pWindow, onclose);


		}
	}
}
