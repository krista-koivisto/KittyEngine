/**
 * Kitty Engine
 * KEngine.h
 *
 * This is the core interface for the Kitty engine. This class is
 * the starting point when using the engine. It is also responsible
 * for providing an interface to the Kitty engine Vulkan abstraction
 * layer as well as setting up the chosen window manager.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KENGINE_H
#define KENGINE_KENGINE_H

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vector>
#include <chrono>
#include "IWindow.h"
#include "KError.h"
#include "KScene.h"
#include "Vulkan/KVulkan.h"
#include "Vulkan/KVulkanDefaults.h"
#include "Vulkan/KVulkanGraphicsPipeline.h"
#include "Vulkan/KVulkanHelpers.h"
#include "Vulkan/KVulkanBuffer.h"
#include "KVectors.h"
#include "ITextureLoader.h"

using namespace Kitty::Error;

namespace Kitty
{
	class KScene;
	class ITexture;
	class ITextureLoader;

	class KEngine
	{
	public:
		/**
		 * \brief Initialize the engine.
		 *
		 * Creates the render window and initializes the Vulkan interface.
		 *
		 * \param windowInfo [optional] KWindowInfo structure containing information on how to set up window.
		 * \param KSettings [optional] Custom settings to pass to Vulkan when it is initialized.
		 * \param windowManager [optional] Pointer to framework you want to create your window. (e.g. Kitty::Window::KWindowGLFW)
		 * \return KE_OK on success, error code on fail.
		 */
		explicit KEngine(Window::KWindowInfo *windowInfo = nullptr,
		                 KSettings *kittySettings = nullptr,
		                 Window::IWindow *windowManager = nullptr);
		~KEngine();

		//! Render window instance
		Window::IWindow *window = nullptr;
		//! Default values for lots of Vulkan functions
		Vulkan::KVulkanDefaults defaults;
		//! Custom Vulkan settings
		Vulkan::KVulkanSettings settings = defaults.vulkanSettings;
		//! Custom Vulkan pipeline settings
		Vulkan::KVulkanGraphicsSettings graphicsSettings = defaults.graphicsPipelineInfo;

		/**
		 * \brief Create a scene instance.
		 *
		 * Create a new scene. Through the scene instance you can load models and textures, it's
		 * where your objects and everything else to be rendered exist.
		 *
		 * \param textureLoader [optional] Pointer to framework you want to load your textures. (e.g. Kitty::Implementations::STB_Texture)
		 * \return Pointer to a scene instance.
		 */
		KScene *CreateScene(ITextureLoader *textureLoader = nullptr);

		/**
		 * \brief Engine main loop.
		 *
		 * This is the main loop of the engine, every time it is called it tells the Vulkan
		 * interface to render and display a new image.
		 *
		 * \return KE_OK while running, KE_STOP on loop stop and error code on fail.
		 */
		KError IsRunning();

		/**
		 * \brief Update the FPS counter to get the latest value.
		 *
		 * Returns the current frames per second value. This value is useful for determining
		 * the performance of your application.
		 *
		 * \param fpsUpdateFrequency FPS update frequency in milliseconds.
		 * \return Latest FPS value.
		 */
		float UpdateFPS(uint32_t fpsUpdateFrequency = 1000);

	private:
		Vulkan::KVulkan *vulkan = nullptr;

		std::vector<KScene*> scenes;

		bool hasUserSetWindowManager = true;

		std::chrono::high_resolution_clock::time_point lastFPSUpdate = std::chrono::high_resolution_clock::now();
		std::chrono::high_resolution_clock::time_point delta = lastFPSUpdate;
		uint32_t fpsCounter = 0;
		float fps = 0;

		/**
		 * \brief Callback target for window interfaces
		 *
		 * This is the resize callback all window framework handlers must target. It
		 * informs Vulkan to update to the new dimensions.
		 *
		 * \param window Pointer back to window object.
		 * \param width New width.
		 * \param height New height.
		 */
		void OnWindowResize(Window::IWindow *window, int width, int height);

		/**
		 * \brief Initializes the Vulkan abstraction layer.
		 *
		 * \return KE_OK on success, error code on fail.
		 */
		KError InitializeVulkan();
	};
}


#endif //KENGINE_KENGINE_H
