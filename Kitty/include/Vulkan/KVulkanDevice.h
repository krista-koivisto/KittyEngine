/**
 * Kitty engine Vulkan implementation
 * KVulkanDevice.h
 *
 * Vulkan device implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANDEVICE_H
#define KENGINE_KVULKANDEVICE_H

#include <vulkan/vulkan.h>
#include <set>
#include <map>
#include <vector>
#include "KVulkan.h"
#include "../KError.h"

using namespace Kitty::Error;

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkan;

		class KVulkanDevice
		{
		private:
			KVulkan *context = nullptr;
			KVulkanDefaults defaults;

			std::vector<VkQueue> graphicsQueues;

		public:
			explicit KVulkanDevice(KVulkan *mainContext) : context(mainContext) {}
			~KVulkanDevice();

			struct KDeviceFeatures
			{
				//! Device ID
				uint32_t id = 0;
				//! Device name
				std::string name = "";
				//! Is this a discrete (external, dedicated) or internal GPU?
				bool isDiscrete = false;
				//! GPU VRAM size in bytes
				uint64_t vram = 0;
				//! Supported Vulkan API version
				uint32_t apiVersion = 0;
				//! Depth format in use.
				VkFormat depthFormat = {};
				//! Unprocessed Vulkan feature data
				VkPhysicalDeviceFeatures VkFeatures = {};
				//! Unprocessed Vulkan limits data
				VkPhysicalDeviceLimits VkLimits = {};

				uint32_t graphicsFamily = UINT32_MAX;
				uint32_t presentFamily = UINT32_MAX;
				uint32_t transferFamily = UINT32_MAX;

				bool hasCompleteFamilies()
				{
					return (graphicsFamily != UINT32_MAX && presentFamily != UINT32_MAX && transferFamily != UINT32_MAX);
				}
			};

			KDeviceFeatures features = {};
			VkPhysicalDevice pDevice = VK_NULL_HANDLE;
			VkDevice device = VK_NULL_HANDLE;
			VkQueue graphicsQueue = VK_NULL_HANDLE;
			VkQueue presentQueue = VK_NULL_HANDLE;
			VkQueue transferQueue = VK_NULL_HANDLE;

			/**
			 * \brief Create and initialize Vulkan devices.
			 *
			 * Selects, creates and initializes physical and logical devices. All in one
			 * big swoop of magic!
			 *
			 * \param requestedFeatures [optional] Which features will we be needing?
			 * \param devCreateInfo [optional] Device creation info
			 * \param requestedDeviceExtensions [optional] Custom device extensions to use.
			 * \return KE_OK on success, error code on fail.
			 */
			KError Initialize(VkPhysicalDeviceFeatures *requestedFeatures = nullptr,
			                  VkDeviceCreateInfo *devCreateInfo = nullptr,
			                  std::vector<const char *> *requestedDeviceExtensions = nullptr);

			/**
			 * \brief Create and initialize Vulkan logical device.
			 *
			 * \param deviceExtensions Needed device extensions.
			 * \param requestedFeatures [optional] Which features will we be needing?
			 * \param devCreateInfo [optional] Device creation info
			 * \return KE_OK on success, error code on fail.
			 */
			KError CreateLogicalDevice(std::vector<const char *> deviceExtensions,
			                           VkPhysicalDeviceFeatures *requestedFeatures = nullptr,
			                           VkDeviceCreateInfo *devCreateInfo = nullptr);

			/**
			 * \brief Get list of physical devices available.
			 *
			 * \return std::vector containing list of VkPhysicalDevices.
			 */
			std::vector<VkPhysicalDevice> GetPhysicalDevices();

			/**
			 * \brief Retrieve device features.
			 *
			 * Get device features from Vulkan and provide some human
			 * readable versions of some frequently required features
			 * like VRAM and GPU type.
			 *
			 * \param physicalDevice Device to get features for.
			 * \return KDeviceFeatures containing found features.
			 */
			KDeviceFeatures GetDeviceFeatures(VkPhysicalDevice physicalDevice);

			/**
			 * \brief Find formats supported by physical device.
			 *
			 * \param candidates Candidates to assess support for.
			 * \param tiling Tiling mode to assess support for.
			 * \param features Features to assess support for.
			 * \return The most fitting supported format.
			 */
			VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice,
			                             const std::vector<VkFormat>& candidates,
			                             VkImageTiling tiling,
			                             VkFormatFeatureFlags features);

			/**
			 * \brief Verify that device supports requested functions.
			 *
			 * \param physicalDevice Physical device.
			 * \param deviceExtensions std::vector of requested (const char*) extensions.
			 * \return true if all extensions are supported, false otherwise.
			 */
			bool AreExtensionsSupported(VkPhysicalDevice physicalDevice, std::vector<const char *> deviceExtensions);

			/**
			 * \brief Finds the most fitting device.
			 *
			 * This function uses the RateDevice function to pick the most
			 * qualified card based on its rating.
			 *
			 * \param devices Vector list of devices found on the system.
			 * \return VkPhysicalDevice pointing to the most qualified device.
			 */
			VkPhysicalDevice FindMostFittingDevice(std::vector<VkPhysicalDevice> devices);

			/**
			 * \brief Rates devices based on features.
			 *
			 * This function rates devices based on their features, discrete
			 * cards are always picked over integrated cards. After that the
			 * rating is decided based on VRAM size.
			 *
			 * \param device Vulkan physical device handle.
			 * \return Card rating. A rating of 0 means the card didn't pass at all.
			 */
			uint32_t RateDevice(VkPhysicalDevice device);

			/**
			 * \brief Get queue family indices for passing on to resource creation.
			 *
			 * \return Vector containing unique queue family indices.
			 */
			std::vector<uint32_t> GetQueueFamilyIndices();
		};
	}
}


#endif //KENGINE_KVULKANDEVICE_H
