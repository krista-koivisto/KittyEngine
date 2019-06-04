/**
 * Kitty Engine
 * KWindowGLFW.h
 *
 * This is an implementation of a GLFW window framework using the
 * Kitty::IWindow interface.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */


#ifndef KENGINE_GLFW_WINDOW_H
#define KENGINE_GLFW_WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>

#include "KEngine.h"
#include "IWindow.h"
#include "KVectors.h"

namespace Kitty
{
	namespace Window
	{
		class KWindowGLFW : public IWindow
		{
		private:
			GLFWwindow *window = nullptr;
			KWindowInfo *info;

			bool isCursorHidden = false;

			void SetCallbacks();

			/**
			 * \brief Callback for resize.
			 *
			 * Callback function relaying new size information to KEngine, which
			 * passes it on to Vulkan.
			 *
			 * \param width
			 * \param height
			 */
			void OnWindowResize(int width, int height);

			/**
			 * \brief Keyboard callback function.
			 *
			 * Sends keyboard events to the IWindow callback function, if set.
			 *
			 * \param key
			 * \param scancode
			 * \param action
			 * \param mods
			 */
			void KeyboardCallback(int key, int scancode, int action, int mods);

			/**
			 * \brief Mouse move callback function.
			 *
			 * Sends mouse movement events to the IWindow callback function, if set.
			 *
			 * \param x
			 * \param y
			 */
			void MouseMoveCallback(double x, double y);

		public:
			KWindowGLFW() = default;
			~KWindowGLFW();

			KWindowStatus Create(KWindowInfo *windowInfo) override;
			KWindowStatus Update() override;
			std::vector<const char *> GetExtensions() override;
			KWindowStatus CreateVulkanSurface(VkInstance instance, VkSurfaceKHR *surface) override;
			Vector2<int> GetDimensions() override;
			void SetWindowTitle(std::string title) override;
		};
	}
}


#endif //KENGINE_GLFW_WINDOW_H
