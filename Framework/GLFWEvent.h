

#pragma once

#include <map>
#include <vector>

namespace jet
{
	namespace util
	{
		class WindowEventCallback
		{
		public:
			/**
			* The window move callback.
			*
			* @param xpos   the new x-coordinate, in pixels, of the upper-left corner of the client area of the window
			* @param ypos   the new y-coordinate, in pixels, of the upper-left corner of the client area of the window
			*/
			virtual bool WindowPos(int xpos, int ypos) = 0;

			/**
			* The window close callback.
			*/
			virtual bool WindowClose() = 0;

			/**
			* The window focus callback.
			*
			* @param focused true if the window was focused, or false if it was defocused
			*/
			virtual bool WindowFocus(bool focused) = 0;

			/**
			* The window iconify callback.
			*
			* @param iconified true if the window was iconified, or false if it was restored
			*/
			virtual bool WindowIconify(bool iconified) = 0;
		};

		class GLFWCallback : public WindowEventCallback
		{
		public:
			/**
			* The key callback.
			*
			* @param key    the keyboard key that was pressed or released
			* @param action the key action. One of:<p/>{@link GLFW#GLFW_PRESS}, {@link GLFW#GLFW_RELEASE}, {@link GLFW#GLFW_REPEAT}
			* @param mods   bitfield describing which modifiers keys were held down
			*/
			virtual bool Key(int key, int scancode, int action, int mods) = 0;


			/**
			* This Unicode character with modifiers callback. It is called for each input character, regardless of what modifier keys are held down.
			*
			* @param codepoint the Unicode code point of the character
			* @param mods      bitfield describing which modifier keys were held down
			*/
			virtual bool CharMods(unsigned int codepoint, int mods) = 0;

			/**
			* The mouse button callback.
			*
			* @param button the mouse button that was pressed or released
			* @param action the button action. One of:<p/>{@link GLFW#GLFW_PRESS}, {@link GLFW#GLFW_RELEASE}
			* @param mods   bitfield describing which modifiers keys were held down
			*/
			virtual bool MouseButton(int button, int action, int mods) = 0;

			/**
			* The cursor move callback.
			*
			* @param xpos   the new x-coordinate, in screen coordinates of the cursor
			* @param ypos   the new y-coordinate, in screen coordinates of the cursor
			*/
			virtual bool CursorPos(double xpos, double ypos) = 0;

			/**
			* The cursor enter callback.
			*
			* @param entered true if the cursor enter the window's client area, or false if it left it
			*/
			virtual bool CursorEnter(bool entered) = 0;

			/**
			* The scroll callback.
			*
			* @param xoffset the scroll offset along the x-axis
			* @param yoffset the scroll offset along the y-axis
			*/
			virtual bool Scroll(double xoffset, double yoffset) = 0;
		};

		/// Abstract class define the keybaord events
		class KeyboardCallback
		{
		public:

			KeyboardCallback(){}
			virtual bool OnkeyPressed(int keycode, char keychar) = 0;
			virtual bool OnkeyReleased(int keycode, char keychar) = 0;
			virtual bool OnkeyTyped(int keycode, char keychar) = 0;

		private:
			KeyboardCallback(KeyboardCallback&) = delete;
		};

		enum Button
		{
			LEFT = 1,
			RIGHT = 2,
			MIDDLE = 4,

			BUTTON_1 = 1,
			BUTTON_2 = 2,
			BUTTON_3 = 4,
			BUTTON_4 = 8,
			BUTTON_5 = 16,
			BUTTON_6 = 32,
			BUTTON_7 = 64,
			BUTTON_8 = 128
		};

		class MouseCallback
		{
		public:

			MouseCallback(){}
			virtual bool OnMousePressed(int x, int y, Button button) = 0;
			virtual bool OnMouseReleased(int x, int y, Button button) = 0;
			virtual bool OnMouseMoved(int x, int y, int dx, int dy) = 0;
			virtual bool OnMouseDraged(int x, int y, int dx, int dy, Button button) = 0;
			virtual bool OnScrolled(int wheel) = 0;
			virtual bool OnCursorEnter(bool entered){ return false; }

		private:
			MouseCallback(MouseCallback&) = delete;
		};

		class GLFWCallbackChain : public GLFWCallback
		{
		public:
			bool WindowPos(int xpos, int ypos) override;
			bool WindowClose() override;
			bool WindowFocus(bool focused) override;
			bool WindowIconify(bool iconified) override;
			bool Key(int key, int scancode, int action, int mods) override;
			bool CharMods(unsigned int codepoint, int mods) override;
			bool MouseButton(int button, int action, int mods) override;
			bool CursorPos(double xpos, double ypos) override;
			bool CursorEnter(bool entered) override;
			bool Scroll(double xoffset, double yoffset) override;

			void addGLFWCallback(GLFWCallback* pCallback);
			void removeGLFWCallback(GLFWCallback* pCallback);
			std::vector<GLFWCallback*>& getGLFWCallbacks() { return m_Chains; }

		private:
			std::vector<GLFWCallback*> m_Chains;
		};

		class InputAdapter : public GLFWCallback
		{
		public:
			InputAdapter(KeyboardCallback* =nullptr, MouseCallback* = nullptr, WindowEventCallback* = nullptr);

			virtual bool WindowPos(int xpos, int ypos) override;
			virtual bool WindowClose() override;
			virtual bool WindowFocus(bool focused) override;
			virtual bool WindowIconify(bool iconified) override;
			virtual bool Key(int key, int scancode, int action, int mods) override;
			virtual bool CharMods(unsigned int codepoint, int mods) override;
			virtual bool MouseButton(int button, int action, int mods) override;
			virtual bool CursorPos(double xpos, double ypos) override;
			virtual bool CursorEnter(bool entered) override;
			virtual bool Scroll(double xoffset, double yoffset) override;

			void SetKeyEventCallback(KeyboardCallback* pCallback) { m_pKeyboardCallback = pCallback; }
			const KeyboardCallback* GetKeyboardCallback()   const { return m_pKeyboardCallback; }

			void SetMouseCallback(MouseCallback* pCallback) { m_pMouseCallback = pCallback; }
			const MouseCallback* GetMouseCallback()   const { return m_pMouseCallback; }

			void SetWindowEventCallback(WindowEventCallback* pCallback) { m_pWindowEventCallback = pCallback; }
			const WindowEventCallback* GetWindowEventCallback()   const { return m_pWindowEventCallback; }

		private:

			static const int BUTTON_COUNT = 8;
			KeyboardCallback* m_pKeyboardCallback;
			MouseCallback*    m_pMouseCallback;
			WindowEventCallback*	  m_pWindowEventCallback;

			std::map<int, int> m_keyChars;
			int m_iCurrKey;
			int m_iKeyAction;

			bool m_bButtonState[BUTTON_COUNT];

			int m_iMouseX, m_iMouseY;
			int m_iLastX, m_iLastY;

		private:
			InputAdapter(InputAdapter&) = delete;
		};
	}
}
