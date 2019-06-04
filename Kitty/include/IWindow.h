/**
 * Kitty Engine
 * IWindow.h
 *
 * This is a virtual abstraction layer for implementing window frameworks.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_IWINDOW_H
#define KENGINE_IWINDOW_H

#include <vulkan/vulkan.h>
#include <functional>
#include "KError.h"
#include "KVectors.h"
#include "string"

namespace Kitty
{
	namespace Window
	{
		class IWindow;

		namespace Events
		{
			/**
			 * \brief Keyboard action codes.
			 */
			enum KKeyboardAction
			{
				KA_RELEASE,
				KA_PRESS,
				KA_REPEAT
			};

			/**
			 * \brief Keyboard modifier codes.
			 */
			enum KKeyboardModifiers
			{
				KM_MOD_SHIFT = 0x0001,
				KM_MOD_CONTROL = 0x002,
				KM_MOD_ALT = 0x0004,
				KM_MOD_SUPER = 0x0008
			};

			/**
			 * \brief Keyboard key input codes.
			 *
			 * The window manager *must* adhere to these codes. Any codes sent but
			 * not implemented here must have different codes from these.
			 */
			enum KKeyboardKeys
			{
				KK_KEY_UNKNOWN = 0x00,
				KK_KEY_BACKSPACE = 0x08,
				KK_KEY_TAB = 0x09,
				KK_KEY_RETURN = 0x0D,
				KK_KEY_SHIFT = 0x10,
				KK_KEY_CTRL = 0x11,
				KK_KEY_ALT = 0x12,
				KK_KEY_CAPS_LOCK = 0x14,
				KK_KEY_ESCAPE = 0x1B,
				KK_KEY_SPACE = 0x20,
				KK_KEY_PAGE_UP = 0x21,
				KK_KEY_PAGE_DOWN,
				KK_KEY_END,
				KK_KEY_HOME,
				KK_KEY_LEFT,
				KK_KEY_UP,
				KK_KEY_RIGHT,
				KK_KEY_DOWN,
				KK_KEY_APOSTROPHE = 0x29,
				KK_KEY_COMMA = 0x2A,
				KK_KEY_INSERT = 0x2D,
				KK_KEY_DELETE = 0x2E,
				KK_KEY_MINUS,
				KK_KEY_PERIOD,
				KK_KEY_SLASH,
				KK_KEY_0 = 0x30,
				KK_KEY_1,
				KK_KEY_2,
				KK_KEY_3,
				KK_KEY_4,
				KK_KEY_5,
				KK_KEY_6,
				KK_KEY_7,
				KK_KEY_8,
				KK_KEY_9,
				KK_KEY_SEMICOLON = 0x3B,
				KK_KEY_EQUAL = 0x3D,
				KK_KEY_A = 0x41,
				KK_KEY_B,
				KK_KEY_C,
				KK_KEY_D,
				KK_KEY_E,
				KK_KEY_F,
				KK_KEY_G,
				KK_KEY_H,
				KK_KEY_I,
				KK_KEY_J,
				KK_KEY_K,
				KK_KEY_L,
				KK_KEY_M,
				KK_KEY_N,
				KK_KEY_O,
				KK_KEY_P,
				KK_KEY_Q,
				KK_KEY_R,
				KK_KEY_S,
				KK_KEY_T,
				KK_KEY_U,
				KK_KEY_V,
				KK_KEY_W,
				KK_KEY_X,
				KK_KEY_Y,
				KK_KEY_Z,
				KK_KEY_LEFT_BRACKET = 0x5B,
				KK_KEY_BACKSLASH = 0x5C,
				KK_KEY_RIGHT_BRACKET = 0x5D,
				KK_KEY_GRAVE_ACCENT = 0x59,
				KK_KEY_NUMPAD_1 = 0x60,
				KK_KEY_NUMPAD_2,
				KK_KEY_NUMPAD_3,
				KK_KEY_NUMPAD_4,
				KK_KEY_NUMPAD_5,
				KK_KEY_NUMPAD_6,
				KK_KEY_NUMPAD_7,
				KK_KEY_NUMPAD_8,
				KK_KEY_NUMPAD_9,
				KK_KEY_MULTIPLY = 0x6A,
				KK_KEY_ADD,
				KK_KEY_SUBTRACT = 0x6D,
				KK_KEY_DECIMAL,
				KK_KEY_DIVIDE,
				KK_KEY_F1 = 0x70,
				KK_KEY_F2,
				KK_KEY_F3,
				KK_KEY_F4,
				KK_KEY_F5,
				KK_KEY_F6,
				KK_KEY_F7,
				KK_KEY_F8,
				KK_KEY_F9,
				KK_KEY_F10,
				KK_KEY_F11,
				KK_KEY_F12,
			};
		}

		/**
		 * \brief Window status codes.
		 */
		enum KWindowStatus
		{
			KW_UNKNOWN_ERR = -1,
			KW_OK = 1,
			KW_WINDOW_INIT_FAIL,
			KW_WINDOW_CREATE_FAIL,
			KW_WINDOW_CLOSE
		};

		/**
		 * \brief Window settings and data.
		 */
		struct KWindowInfo
		{
			KWindowInfo() = default;

			int width = 640;
			int height = 480;
			bool canScale = false;
			bool captureMouse = false;
			std::string title = "Kitty Engine";

			/**
			 * Window resize callback function, KEngine sets this automatically. No touchy unless you
			 * know what you're doing! :<
			 */
			std::function<void(IWindow *window, int width, int height)> resizeCallback = nullptr;

			/**
			 * Window key callback function. You need to set this if you want keyboard input from the
			 * window manager.
			 *
			 * \param keyCode ASCII code of the pressed down key.
			 * \param scanCode System-specific scan code.
			 * \param action Keyboard event related action, was the key pressed or released?
			 * \param mods Any modifier keys pressed down. (CTRL, Shift, Alt or Super (Windows key))
			 */
			std::function<void(uint32_t keyCode, uint32_t scanCode,
			                   Events::KKeyboardAction action,
			                   Events::KKeyboardModifiers mods)> keyboardCallback = nullptr;

			/**
			 * Window mouse movement callback. You need to set this if you want mouse movement input
			 * from the window manager.
			 *
			 * \param x x-coordinate of the mouse (relative to screen)
			 * \param y y-coordinate of the mouse (relative to screen)
			 */
			std::function<void(float x, float y)> mouseMoveCallback = nullptr;
		};

		class IWindow
		{
		public:
			/**
			 * \brief Create new window
			 *
			 * This function creates a new window with the given information. User may
			 * optionally pass window creation information such as title, dimensions and
			 * whether it can be scaled using KWindowInfo.
			 *
			 * \param info [optional] Window information, may pass nullptr instead.
			 * \return KW_OK on success, KW_WINDOW_INIT_FAIL or KW_WINDOW_CREATE_FAIL on fail.
			 */
			virtual KWindowStatus Create(KWindowInfo *info) = 0;

			/**
			 * \brief Update window contents.
			 *
			 * Update (redraw) the window contents and report back if window is closing.
			 *
			 * \return KError KW_OK on success, KW_WINDOW_CLOSE when closing.
			 */
			virtual KWindowStatus Update() = 0;

			/**
			 * \brief Get required Vulkan extension names
			 *
			 * If you are implementing a new window manager using this interface
			 * you will need to have some understanding on Vulkan extensions and
			 * which of them you will need for your window class. It really isn't
			 * as scary as it sounds, there aren't a lot of them!
			 *
			 * This might help (scroll down a little until you see some code):
			 * https://software.intel.com/en-us/articles/api-without-secrets-introduction-to-vulkan-part-2
			 *
			 * \return std::vector of (const char*) extension names.
			 */
			virtual std::vector<const char *> GetExtensions() = 0;

			/**
			 * \brief Create a Vulkan surface.
			 *
			 * Create a Vulkan surface for Vulkan to render to. Same as above, this
			 * may require some Vulkan programming experience. But really, not scary.
			 *
			 * \param instance [in] Vulkan instance.
			 * \param surface [out] Created Vulkan surface.
			 * \return KW_OK on success, KW_WINDOW_INIT_FAIL on fail.
			 */
			virtual KWindowStatus CreateVulkanSurface(VkInstance instance, VkSurfaceKHR *surface) = 0;

			/**
			 * \brief Return window dimensions.
			 *
			 * \return Window dimensions as [Kitty::Vector2i(width, height)]
			 */
			virtual Vector2<int> GetDimensions() = 0;

			/**
			 * \brief Set the window title.
			 */
			virtual void SetWindowTitle(std::string title) = 0;
		};
	}
}


#endif //KENGINE_IWINDOW_H
