/**
 * Kitty Engine
 * KWindowGLFW.cpp
 *
 * This is an implementation of a GLFW window framework using the
 * Kitty::IWindow interface.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "include/KWindowGLFW.h"

namespace Kitty
{
	namespace Window
	{
		KWindowStatus KWindowGLFW::Create(KWindowInfo *windowInfo)
		{
			if (!glfwInit())
				return KW_WINDOW_INIT_FAIL;

			KWindowInfo defaultInfo;

			if (windowInfo == nullptr)
				info = &defaultInfo;
			else
				info = windowInfo;

			// Set window details
			glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

			if (info->canScale)
				glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
			else
				glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

			// Create window
			window = glfwCreateWindow(info->width, info->height, info->title.c_str(), nullptr, nullptr);

			SetCallbacks();

			if (!window)
				return KW_WINDOW_CREATE_FAIL;
			else
				return KW_OK;
		}

		void KWindowGLFW::SetCallbacks()
		{
			// Set the resize callback if one has been assigned
			if (info->resizeCallback != nullptr && info->canScale)
			{
				glfwSetWindowUserPointer(window, this);

				/* We need to pass an anonymous function like this because GLFW requires
				 * pointers to static functions, which ours is not.*/
				auto resizeFunc = [](GLFWwindow *win, int width, int height)
				{
					static_cast<KWindowGLFW *>(glfwGetWindowUserPointer(win))->OnWindowResize(width, height);
				};

				glfwSetWindowSizeCallback(window, resizeFunc);
			}

			if (info->keyboardCallback != nullptr)
			{
				auto keyboardFunc = [](GLFWwindow *win, int key, int code, int action, int mods)
				{
					static_cast<KWindowGLFW *>(glfwGetWindowUserPointer(win))->KeyboardCallback(key, code, action, mods);
				};

				glfwSetKeyCallback(window, keyboardFunc);
			}

			if (info->mouseMoveCallback != nullptr)
			{
				auto mouseFunc = [](GLFWwindow *win, double x, double y)
				{
					static_cast<KWindowGLFW *>(glfwGetWindowUserPointer(win))->MouseMoveCallback(x, y);
				};

				glfwSetCursorPosCallback(window, mouseFunc);
			}
		}

		void KWindowGLFW::KeyboardCallback(int key, int scancode, int action, int mods)
		{
			if (info->keyboardCallback != nullptr)
			{
				// Translate keys to the Kitty Engine standard
				switch(key)
				{
					using namespace Events;
					case GLFW_KEY_UNKNOWN: key = KK_KEY_UNKNOWN; break;
					case GLFW_KEY_LEFT_ALT: key = KK_KEY_ALT; break;
					case GLFW_KEY_RIGHT_ALT: key = KK_KEY_ALT; break;
					case GLFW_KEY_LEFT_CONTROL: key = KK_KEY_CTRL; break;
					case GLFW_KEY_RIGHT_CONTROL: key = KK_KEY_CTRL; break;
					case GLFW_KEY_LEFT_SHIFT: key = KK_KEY_SHIFT; break;
					case GLFW_KEY_RIGHT_SHIFT: key = KK_KEY_SHIFT; break;
					case GLFW_KEY_ESCAPE: key = KK_KEY_ESCAPE; break;
					case GLFW_KEY_ENTER: key = KK_KEY_RETURN; break;
					case GLFW_KEY_TAB: key = KK_KEY_TAB; break;
					case GLFW_KEY_BACKSPACE: key = KK_KEY_BACKSPACE; break;
					case GLFW_KEY_INSERT: key = KK_KEY_INSERT; break;
					case GLFW_KEY_DELETE: key = KK_KEY_DELETE; break;
					case GLFW_KEY_HOME: key = KK_KEY_HOME; break;
					case GLFW_KEY_END: key = KK_KEY_END; break;
					case GLFW_KEY_PAGE_UP: key = KK_KEY_PAGE_UP; break;
					case GLFW_KEY_PAGE_DOWN: key = KK_KEY_PAGE_DOWN; break;
					case GLFW_KEY_LEFT: key = KK_KEY_LEFT; break;
					case GLFW_KEY_RIGHT: key = KK_KEY_RIGHT; break;
					case GLFW_KEY_UP: key = KK_KEY_UP; break;
					case GLFW_KEY_DOWN: key = KK_KEY_DOWN; break;
					case GLFW_KEY_APOSTROPHE: key = KK_KEY_APOSTROPHE; break;
					case GLFW_KEY_COMMA: key = KK_KEY_COMMA; break;
					case GLFW_KEY_MINUS: key = KK_KEY_MINUS; break;
					case GLFW_KEY_PERIOD: key = KK_KEY_PERIOD; break;
					case GLFW_KEY_SLASH: key = KK_KEY_SLASH; break;
					case GLFW_KEY_SEMICOLON: key = KK_KEY_SEMICOLON; break;
					case GLFW_KEY_EQUAL: key = KK_KEY_EQUAL; break;
					case GLFW_KEY_LEFT_BRACKET: key = KK_KEY_LEFT_BRACKET; break;
					case GLFW_KEY_BACKSLASH: key = KK_KEY_BACKSLASH; break;
					case GLFW_KEY_RIGHT_BRACKET: key = KK_KEY_RIGHT_BRACKET; break;
					case GLFW_KEY_GRAVE_ACCENT: key = KK_KEY_GRAVE_ACCENT; break;
					case GLFW_KEY_CAPS_LOCK: key = KK_KEY_CAPS_LOCK; break;
					case GLFW_KEY_F1: key = KK_KEY_F1; break;
					case GLFW_KEY_F2: key = KK_KEY_F2; break;
					case GLFW_KEY_F3: key = KK_KEY_F3; break;
					case GLFW_KEY_F4: key = KK_KEY_F4; break;
					case GLFW_KEY_F5: key = KK_KEY_F5; break;
					case GLFW_KEY_F6: key = KK_KEY_F6; break;
					case GLFW_KEY_F7: key = KK_KEY_F7; break;
					case GLFW_KEY_F8: key = KK_KEY_F8; break;
					case GLFW_KEY_F9: key = KK_KEY_F9; break;
					case GLFW_KEY_F10: key = KK_KEY_F10; break;
					case GLFW_KEY_F11: key = KK_KEY_F11; break;
					case GLFW_KEY_F12: key = KK_KEY_F12; break;
					case GLFW_KEY_KP_1: key = KK_KEY_NUMPAD_1; break;
					case GLFW_KEY_KP_2: key = KK_KEY_NUMPAD_2; break;
					case GLFW_KEY_KP_3: key = KK_KEY_NUMPAD_3; break;
					case GLFW_KEY_KP_4: key = KK_KEY_NUMPAD_4; break;
					case GLFW_KEY_KP_5: key = KK_KEY_NUMPAD_5; break;
					case GLFW_KEY_KP_6: key = KK_KEY_NUMPAD_6; break;
					case GLFW_KEY_KP_7: key = KK_KEY_NUMPAD_7; break;
					case GLFW_KEY_KP_8: key = KK_KEY_NUMPAD_8; break;
					case GLFW_KEY_KP_9: key = KK_KEY_NUMPAD_9; break;
					case GLFW_KEY_KP_DECIMAL: key = KK_KEY_DECIMAL; break;
					case GLFW_KEY_KP_ADD: key = KK_KEY_ADD; break;
					case GLFW_KEY_KP_SUBTRACT: key = KK_KEY_SUBTRACT; break;
					case GLFW_KEY_KP_DIVIDE: key = KK_KEY_DIVIDE; break;
					case GLFW_KEY_KP_MULTIPLY: key = KK_KEY_MULTIPLY; break;
					case GLFW_KEY_KP_EQUAL: key = KK_KEY_EQUAL; break;

					default: break;
				}

				if (key > 256) key = Events::KK_KEY_UNKNOWN;

				info->keyboardCallback(static_cast<uint32_t>(key),
				                      static_cast<uint32_t>(scancode),
				                      static_cast<Events::KKeyboardAction>(action),
				                      static_cast<Events::KKeyboardModifiers>(mods));
			}
		}

		void KWindowGLFW::MouseMoveCallback(double x, double y)
		{
			info->mouseMoveCallback(static_cast<float>(x), static_cast<float>(y));
		}

		void KWindowGLFW::OnWindowResize(int width, int height)
		{
			info->width = width;
			info->height = height;
			if (width == 0 || height == 0) return;

			info->resizeCallback(this, width, height);
		}

		KWindowStatus KWindowGLFW::Update()
		{
			glfwPollEvents();

			if (info->captureMouse)
			{
				glfwSetCursorPos(window, info->width / 2, info->height / 2);

				if (!isCursorHidden)
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
					isCursorHidden = true;
				}
			}
			else
			{
				if (isCursorHidden)
				{
					glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
					isCursorHidden = false;
				}
			}

			if (!glfwWindowShouldClose(window))
			{
				return KW_OK;
			}
			else
			{
				return KW_WINDOW_CLOSE;
			}
		}

		std::vector<const char *> KWindowGLFW::GetExtensions()
		{
			std::vector<const char *> extensions;

			unsigned int extCount = 0;
			const char **ext;

			// Isn't this just handy? Thanks, GLFW! <3
			ext = glfwGetRequiredInstanceExtensions(&extCount);

			for (unsigned int i = 0; i < extCount; ++i)
			{
				extensions.push_back(ext[i]);
			}

			return extensions;
		}

		KWindowStatus KWindowGLFW::CreateVulkanSurface(VkInstance instance, VkSurfaceKHR *surface)
		{
			if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS)
			{
				return KW_WINDOW_INIT_FAIL;
			}

			return KW_OK;
		}

		Vector2<int> KWindowGLFW::GetDimensions()
		{
			return Vector2<int>(info->width, info->height);
		}

		void KWindowGLFW::SetWindowTitle(std::string title)
		{
			glfwSetWindowTitle(window, title.c_str());
		}

		KWindowGLFW::~KWindowGLFW()
		{
			glfwDestroyWindow(window);
			glfwTerminate();
			window = nullptr;
		}
	}
}
