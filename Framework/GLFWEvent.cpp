
#include "GLFWEvent.h"

#include <GLFW\glfw3.h>

namespace jet
{
	namespace util
	{
#define null nullptr
		InputAdapter::InputAdapter(KeyboardCallback* pKeyboardCB, MouseCallback* pMouseCB, WindowEventCallback* pWindowCB):
			m_pKeyboardCallback(pKeyboardCB), m_pMouseCallback(pMouseCB), m_pWindowEventCallback(pWindowCB), m_keyChars()
		{
			m_keyChars.clear();
		}

		bool InputAdapter::WindowPos(int xpos, int ypos)
		{
			if (m_pWindowEventCallback != nullptr)
			{
				m_pWindowEventCallback->WindowPos(xpos, ypos);
			}

			return m_pWindowEventCallback != null;
		}

		bool InputAdapter::WindowClose() 
		{
			if (m_pWindowEventCallback != nullptr)
			{
				m_pWindowEventCallback->WindowClose();
			}
			return m_pWindowEventCallback != null;

		}

		bool InputAdapter::WindowFocus(bool focused)
		{
			if (m_pWindowEventCallback != nullptr)
			{
				m_pWindowEventCallback->WindowFocus(focused);
			}

			return m_pWindowEventCallback != null;
		}

		bool InputAdapter::WindowIconify(bool iconified)
		{
			if (m_pWindowEventCallback != nullptr){
				m_pWindowEventCallback->WindowIconify(iconified);
			}

			return m_pWindowEventCallback != null;
		}

		static bool isPrintableKey(int key){
			if (key == -1)  // Unkown key
				return false;

			return key < 0x100;
		}

		bool InputAdapter::Key(int key, int scancode, int action, int mods)
		{
			m_iCurrKey= key;
			m_iKeyAction = action;

			auto it = m_keyChars.find(key);
			char keyChar = 0;
			if (it != m_keyChars.end())
			{
				keyChar = static_cast<char>(it->second);
				m_keyChars.erase(it);
			}

			if (m_pKeyboardCallback != nullptr)
			{
				switch (action) 
				{
				case GLFW_PRESS:
					if (!isPrintableKey(key))
					{
						m_pKeyboardCallback->OnkeyPressed(key, keyChar);
					}
					break;
				case GLFW_REPEAT:
					if (!isPrintableKey(key))
					{
						m_pKeyboardCallback->OnkeyTyped(key, keyChar);
					}
					break;
				case GLFW_RELEASE:
					if (!isPrintableKey(key))
					{
						m_pKeyboardCallback->OnkeyReleased(key, keyChar);
					}
					else
					{
						m_pKeyboardCallback->OnkeyReleased(key, keyChar);
					}
					break;
				default:
					break;
				}
			}

			return m_pKeyboardCallback != null;
		}

		bool InputAdapter::CharMods(unsigned int codepoint, int mods)
		{
			if (m_pKeyboardCallback != nullptr)
			{
				switch (m_iKeyAction) 
				{
				case GLFW_PRESS:
					m_pKeyboardCallback->OnkeyPressed(m_iCurrKey, (char)codepoint);
					break;
				case GLFW_REPEAT:
					m_pKeyboardCallback->OnkeyTyped(m_iCurrKey, (char)codepoint);
					break;
				default:
					break;
				}
			}

//			m_keyChars.put(currKey, codepoint);
			m_keyChars[m_iCurrKey] = codepoint;
			return true;
		}

		bool InputAdapter::MouseButton(int button, int action, int mods)
		{
			switch (action) {
			case GLFW_PRESS:
				m_bButtonState[button] = true;
				if (m_pMouseCallback != null)
				{
					m_pMouseCallback->OnMousePressed(m_iMouseX, m_iMouseY, static_cast<Button>(1 << button));
				}
				break;
			case GLFW_RELEASE:
				m_bButtonState[button] = false;
				if (m_pMouseCallback != null)
				{
					m_pMouseCallback->OnMouseReleased(m_iMouseX, m_iMouseY, static_cast<Button>(1 << button));
				}
				break;
			default:
				break;
			}

			return true;
		}

		bool InputAdapter::CursorPos(double xpos, double ypos)
		{
			m_iMouseX = static_cast<int>(xpos);
			m_iMouseY = static_cast<int>(ypos);

			int mouseX = m_iMouseX;
			int mouseY = m_iMouseY;
			int lastX = m_iLastX;
			int lastY = m_iLastY;

			if (m_pMouseCallback != null)
			{
				int bits = 0;
				for (int i = 0; i < BUTTON_COUNT; i++)
				{
					if (m_bButtonState[i])
					{
						bits |= 1 << i;
					}
				}

				if (bits == 0)
				{ // no button pressed
					m_pMouseCallback->OnMouseMoved(mouseX, mouseY, mouseX - lastX, mouseY - lastY);
				}
				else
				{
					m_pMouseCallback->OnMouseDraged(mouseX, mouseY, mouseX - lastX, mouseY - lastY, static_cast<Button>(bits));
				}
			}

			lastX = mouseX;
			lastY = mouseY;

			return true;
		}

		bool InputAdapter::CursorEnter(bool entered)
		{
			if (m_pMouseCallback != null)
			{
				m_pMouseCallback->OnCursorEnter(entered);
			}
			return true;
		}

		bool InputAdapter::Scroll(double xoffset, double yoffset) {
			if (m_pMouseCallback != null){
				m_pMouseCallback->OnScrolled(static_cast<int>(yoffset));
			}

			return true;
		}


		bool GLFWCallbackChain::WindowPos(int xpos, int ypos)
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->WindowPos(xpos, ypos))
				{
					return true;
				}
			}
		}
		bool GLFWCallbackChain::WindowClose()
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->WindowClose())
				{
					return true;
				}
			}
		}
		bool GLFWCallbackChain::WindowFocus(bool focused)
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->WindowFocus(focused))
				{
					return true;
				}
			}
		}

		bool GLFWCallbackChain::WindowIconify(bool iconified)
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->WindowIconify(iconified))
				{
					return true;
				}
			}
		}
		bool GLFWCallbackChain::Key(int key, int scancode, int action, int mods)
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->Key(key, scancode, action, mods))
				{
					return true;
				}
			}
		}

		bool GLFWCallbackChain::CharMods(unsigned int codepoint, int mods)
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->CharMods(codepoint, mods))
				{
					return true;
				}
			}
		}

		bool GLFWCallbackChain::MouseButton(int button, int action, int mods)
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->MouseButton(button, action,mods))
				{
					return true;
				}
			}
		}
		bool GLFWCallbackChain::CursorPos(double xpos, double ypos)
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->CursorPos(xpos, ypos))
				{
					return true;
				}
			}
		}
		bool GLFWCallbackChain::CursorEnter(bool entered)
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->CursorEnter(entered))
				{
					return true;
				}
			}
		}

		bool GLFWCallbackChain::Scroll(double xoffset, double yoffset)
		{
			for (int i = 0; i < m_Chains.size(); i++)
			{
				if (m_Chains[i]->Scroll(xoffset, yoffset))
				{
					return true;
				}
			}
		}

		void GLFWCallbackChain::addGLFWCallback(GLFWCallback* pCallback)
		{
			if (pCallback != nullptr)
			{
				for (int i = 0; i < m_Chains.size(); i++)
				{
					if (m_Chains[i] == pCallback)
					{
						return;
					}
				}

				m_Chains.push_back(pCallback);
			}
		}

		void GLFWCallbackChain::removeGLFWCallback(GLFWCallback* pCallback)
		{
			if (pCallback != nullptr)
			{

				for (auto i = m_Chains.begin(); i != m_Chains.end(); i++)
				{
					if (*i == pCallback)
					{
						m_Chains.erase(i);
						return;
					}
				}
			}
		}
	}
}