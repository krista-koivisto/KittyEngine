/**
 * Kitty engine Vulkan implementation
 * KVulkan.h
 *
 * Vulkan core implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKAN_H
#define KENGINE_KVULKAN_H

#define KE_MAX_DYNAMIC_LIGHTS 128

#include <iostream>
#include <algorithm>
#include <vulkan/vulkan.h>
#include "KVulkanDefaults.h"
#include "KVulkanDevice.h"
#include "KVulkanSwapChain.h"
#include "KVulkanRenderPass.h"
#include "KVulkanDescriptorPool.h"
#include "KVulkanGraphicsPipeline.h"
#include "KVulkanFramebuffer.h"
#include "KVulkanCommandPool.h"
#include "KVulkanHelpers.h"
#include "../KMesh.h"
#include "../IWindow.h"
#include "../KError.h"
#include "KVulkanImage.h"

using namespace Kitty::Error;

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkanDevice;
		class KVulkanSwapChain;
		class KVulkanRenderPass;
		class KVulkanDescriptorPool;
		class KVulkanGraphicsPipeline;
		class KVulkanFramebuffer;
		class KVulkanCommandPool;
		class KVulkanImage;
		class KVulkanImageView;

		//! Requested validation layers
		const std::vector<const char *> validationLayers = {"VK_LAYER_LUNARG_standard_validation"};

#ifdef NDEBUG
		const bool enableValidationLayers = false;
#else
		const bool enableValidationLayers = true;
#endif

		class KVulkan
		{
		private:
			KVulkanDefaults defaults;

			/**
			 * \brief Initialize the Vulkan instance.
			 *
			 * \param applicationInfo [optional] Custom application info, e.g. app name and version.
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeInstance(VkApplicationInfo *applicationInfo = nullptr);

			/**
			 * \brief Initialize Vulkan.
			 *
			 * Initializes the Vulkan instance.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError Initialize();

			/**
			 * \brief Initialize Vulkan's chain of graphical entities.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeGraphics();

			/**
			 * \brief Initialize Vulkan device abstraction layer.
			 *
			 * Selects, creates and initializes physical and logical devices.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeDevice();

			/**
			 * \brief Initialize main swap chain instance.
			 *
			 * Initializes the swap chain instance. Custom swap chain creation info may
			 * be passed, as well as requests for desired formats and present modes. Requests
			 * are not guaranteed to be fulfilled, they will only be granted if the device is
			 * capable of it. In other instances the swap chain initializer will pick another
			 * one.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeSwapChain();

			/**
			 * \brief Initialize descriptor pool and layouts.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeDescriptorLayouts();

			/**
			 * \brief Initialize graphics pipelines.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeGraphicsPipelines();

			/**
			 * \brief Initialize the main render pass.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeRenderPass();

			/**
			 * \brief Initialize the frame buffer instance.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeFramebuffer();

			/**
			 * \brief Initialize the main command pool.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeCommandPools();

			/**
			 * \brief Initialize semaphores.
			 *
			 * Semaphores are used for synchronizing Vulkan operations.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeSemaphores();

			/**
			 * \brief Initialize the depth buffer.
			 *
			 * \return KE_OK on success, error code on fail.
			 */
			KError InitializeDepthBuffer();

			/**
			 * \brief Get all available Vulkan extensions.
			 *
			 * \return Available Vulkan extensions.
			 */
			std::vector<VkExtensionProperties> GetAvailableExtensions();

			/**
			 * \brief Get all available validation layers.
			 *
			 * \return Available validation layers.
			 */
			std::vector<VkLayerProperties> GetAvailableValidationLayers();

			/**
			 * \brief Validate support for requested extensions.
			 *
			 * \param available std::vector of available VkExtensionProperties structures.
			 * \param requested std::vector of char* names of requested extensions.
			 * \return true if all requested extensions are supported, otherwise false.
			 */
			bool ValidateExtensionSupport(std::vector<VkExtensionProperties> available,
			                              std::vector<const char *> requested);

			/**
			 * \brief Validate support for requested validation layers.
			 *
			 * \param available std::vector of available VkLayerProperties structures.
			 * \param requested std::vector of requested char* validation layer names.
			 * \return true if all requested layers are supported, otherwise false.
			 */
			bool ValidateValidationLayerSupport(std::vector<VkLayerProperties> available,
			                                    std::vector<const char *> requested);

			/**
			 * \brief Initialize debugging if enabled.
			 *
			 * \param debugInfo Debug callback function information.
			 * \return true on success, false on fail.
			 */
			bool InitDebug(VkDebugReportCallbackCreateInfoEXT *debugInfo = nullptr);

			/**
			 * \brief Destroy the initiated debug instance.
			 */
			void DestroyDebug();

			/**
			 * \brief Destroy the descriptor pool and free descriptor set buffers.
			 */
			void DestroyDescriptorPool();

			/**
			 * \brief Destroy the command pool and free its buffers.
			 */
			void DestroyCommandPools();

			/**
			 * \brief Destroy the swap chain and everything created after it.
			 */
			void DestroySwapChain();

			/**
			 * \brief Validation layer debug callback.
			 *
			 * \param flags Debug message type (e.g. error, debug, performance etc)
			 * \param objType What type of object is responsible for this call?
			 * \param obj Object responsible for the call.
			 * \param location Where (in your code) was this call triggered?
			 * \param code Error code.
			 * \param layerPrefix Validation layer prefix.
			 * \param msg Debug message.
			 * \param userData User data, if any given.
			 * \return VK_FALSE, always.
			 */
			static VKAPI_ATTR VkBool32 VKAPI_CALL VKDebugCallback(
					VkDebugReportFlagsEXT flags,
					VkDebugReportObjectTypeEXT objType,
					uint64_t obj,
					size_t location,
					int32_t code,
					const char *layerPrefix,
					const char *msg,
					void *userData)
			{
				std::cerr << "[" << location  << "] " << "Validation layer: " << msg << std::endl;
				return VK_FALSE;
			}

		public:
			KVulkan(Window::IWindow *targetWindow, KVulkanSettings *vulkanSettings, KVulkanGraphicsSettings *vulkanGraphicsSettings);
			~KVulkan();

			std::function<void()> outdatedSwapChainCallback = nullptr;

			KVulkanSettings *settings;
			KVulkanGraphicsSettings *graphicsSettings;

			Window::IWindow *window;
			KVulkanDevice *device = nullptr;
			KVulkanSwapChain *swapChain = nullptr;
			KVulkanDescriptorPool *descPool = nullptr;
			KVulkanRenderPass *mainRenderPass = nullptr;
			KVulkanGraphicsPipeline *mainPipeline = nullptr;
			KVulkanGraphicsPipeline *instancePipeline = nullptr;
			KVulkanFramebuffer *frameBuffer = nullptr;
			KVulkanCommandPool *cmdPool = nullptr;
			KVulkanCommandPool *transferCmdPool = nullptr;
			KVulkanImage *depthImage = nullptr;
			KVulkanImageView *depthImageView = nullptr;
			VkSemaphore imageAvailableSemaphore = {};
			VkSemaphore renderFinishedSemaphore = {};
			VkInstance instance = VK_NULL_HANDLE;
			VkDebugReportCallbackEXT callback = VK_NULL_HANDLE;
			VkSurfaceKHR surface = VK_NULL_HANDLE;

			VkDescriptorSetLayout lightsDescriptorLayout = {};
			VkDescriptorSetLayout vertexDescriptorLayout = {};
			VkDescriptorSetLayout fragmentDescriptorLayout = {};
			VkDescriptorSetLayout vxUniformBufferDescriptorLayout = {};

			/**
			 * \brief Instruct Vulkan to draw the next frame.
			 */
			void DrawFrame();

			/**
			 * \brief Wait for Vulkan to finish what it's doing.
			 */
			void FinishDrawing();

			/**
			 * \brief Reinitialize main command pool.
			 *
			 * This needs to be done every time something major happens. If you need this frequently
			 * it might be a better idea to use the command pool callbacks so you can modify your
			 * command pools instead.
			 */
			void RecreateCommandPool();

			/**
			 * \Recreate the descriptor pool.
			 *
			 * This needs to be done every time a descriptor set changes in any way. This should not
			 * be done frequently, only when absolutely needed.
			 */
			void RecreateDescriptorPool();

			/**
			 * \brief Recreate the swap chain.
			 *
			 * This needs to be done every time the extents need to change. For example when the
			 * window size or resolution has changed.
			 */
			void RecreateSwapChain();
		};
	}
}


#endif //KENGINE_KVULKAN_H
