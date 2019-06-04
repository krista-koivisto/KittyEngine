/**
 * Kitty engine Vulkan implementation
 * KVulkanDevice.cpp
 *
 * Vulkan device implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanDevice.h"

namespace Kitty
{
	namespace Vulkan
	{
		KError KVulkanDevice::Initialize(VkPhysicalDeviceFeatures *requestedFeatures,
		                                 VkDeviceCreateInfo *devCreateInfo,
		                                 std::vector<const char *> *requestedDeviceExtensions)
		{
			KError ret;

			auto pDevices = GetPhysicalDevices();
			pDevice = FindMostFittingDevice(pDevices);

			if (pDevice == nullptr || pDevice == VK_NULL_HANDLE)
			{
				return KE_VULKAN_NO_DEVICE;
			}

			auto deviceExtensions = defaults.ObtainValues(requestedDeviceExtensions, &defaults.deviceExtensions);
			if (!AreExtensionsSupported(pDevice, deviceExtensions))
			{
				return KE_VULKAN_NO_DEVICE;
			}

			features = GetDeviceFeatures(pDevice);

			ret = CreateLogicalDevice(deviceExtensions, requestedFeatures, devCreateInfo);
			if (ret != KE_OK) return ret;

			return KE_OK;
		}

		KError KVulkanDevice::CreateLogicalDevice(std::vector<const char *> deviceExtensions,
		                                          VkPhysicalDeviceFeatures *requestedFeatures,
		                                          VkDeviceCreateInfo *devCreateInfo)
		{
			std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
			std::set<uint32_t> uniqueQueueFamilies = {features.graphicsFamily,
			                                          features.presentFamily,
			                                          features.transferFamily};

			VkPhysicalDeviceFeatures deviceFeatures = defaults.ObtainValues(requestedFeatures,
			                                                                &defaults.deviceFeatures);

			VkDeviceCreateInfo createInfo = defaults.ObtainValues(devCreateInfo, &defaults.deviceCreateInfo);
			if (!createInfo.pEnabledFeatures) createInfo.pEnabledFeatures = &deviceFeatures;

			if (!createInfo.pQueueCreateInfos)
			{
				float queuePriority = 1.0f;
				for (uint32_t queueFamily : uniqueQueueFamilies)
				{
					auto queueCreateInfo = defaults.queueCreateInfo;
					queueCreateInfo.queueFamilyIndex = queueFamily;
					queueCreateInfo.queueCount = 1;
					queueCreateInfo.pQueuePriorities = &queuePriority;
					queueCreateInfos.push_back(queueCreateInfo);
				}

				createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
				createInfo.pQueueCreateInfos = queueCreateInfos.data();
			}

			createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
			createInfo.ppEnabledExtensionNames = deviceExtensions.data();

			if (enableValidationLayers)
			{
				createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
				createInfo.ppEnabledLayerNames = validationLayers.data();
			}

			if (vkCreateDevice(pDevice, &createInfo, nullptr, &device) != VK_SUCCESS)
			{
				return KE_VULKAN_DEVICE_FAIL;
			}

			for (int i = 0; i < createInfo.pQueueCreateInfos->queueCount; ++i)
			{
				vkGetDeviceQueue(device, features.graphicsFamily, 0, &graphicsQueue);
				graphicsQueues.push_back(graphicsQueue);
			}

			vkGetDeviceQueue(device, features.presentFamily, 0, &presentQueue);
			vkGetDeviceQueue(device, features.transferFamily, 0, &transferQueue);

			return KE_OK;
		}

		std::vector<VkPhysicalDevice> KVulkanDevice::GetPhysicalDevices()
		{
			uint32_t deviceCount = 0;
			vkEnumeratePhysicalDevices(context->instance, &deviceCount, nullptr);

			if (deviceCount == 0)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_NO_DEVICE));
			}

			std::vector<VkPhysicalDevice> devices(deviceCount);
			vkEnumeratePhysicalDevices(context->instance, &deviceCount, devices.data());

			return devices;
		}

		KVulkanDevice::KDeviceFeatures KVulkanDevice::GetDeviceFeatures(VkPhysicalDevice physicalDevice)
		{
			KDeviceFeatures devFeatures;

			VkPhysicalDeviceProperties deviceProperties = {};
			VkPhysicalDeviceFeatures deviceFeatures = {};
			VkPhysicalDeviceMemoryProperties deviceMemory = {};
			vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);
			vkGetPhysicalDeviceFeatures(physicalDevice, &deviceFeatures);
			vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemory);

			devFeatures.id = deviceProperties.deviceID;
			devFeatures.name = deviceProperties.deviceName;
			devFeatures.apiVersion = deviceProperties.apiVersion;
			devFeatures.isDiscrete = (deviceProperties.deviceType & VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU);
			devFeatures.VkFeatures = deviceFeatures;
			devFeatures.VkLimits = deviceProperties.limits;

			devFeatures.depthFormat = FindSupportedFormat(physicalDevice,
			                                              {   VK_FORMAT_D32_SFLOAT,
			                                                  VK_FORMAT_D32_SFLOAT_S8_UINT,
			                                                  VK_FORMAT_D24_UNORM_S8_UINT   },
			                                              VK_IMAGE_TILING_OPTIMAL,
			                                              VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

			// GPU VRAM
			for (int i = 0; i < deviceMemory.memoryHeapCount; ++i)
			{
				if (deviceMemory.memoryHeaps[i].flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
				{
					devFeatures.vram += deviceMemory.memoryHeaps[i].size;
				}
			}

			// Queue families
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

			if (queueFamilyCount == 0) throw std::runtime_error(WhatWentWrong(KE_VULKAN_NO_DEVICE));

			std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());

			// Set to "-1" so we can check whether family support was found
			devFeatures.graphicsFamily = UINT32_MAX;
			devFeatures.presentFamily = UINT32_MAX;
			devFeatures.transferFamily = UINT32_MAX;

			// Get device queue families
			for (uint32_t i = 0; i < queueFamilies.size(); ++i)
			{
				if (queueFamilies[i].queueCount > 0)
				{
					VkBool32 presentSupport = 0;
					vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, context->surface, &presentSupport);

					if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
					{
						devFeatures.graphicsFamily = i;
					}

					if (queueFamilies[i].queueFlags & VK_QUEUE_TRANSFER_BIT &&
						!(queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT))
					{
						devFeatures.transferFamily = i;
					}

					if (presentSupport)
					{
						devFeatures.presentFamily = i;
					}

					if (devFeatures.hasCompleteFamilies())
					{
						break;
					}
				}
			}

			// If no specific transfer family was found, use the graphics family for transfers too
			if (devFeatures.presentFamily != UINT32_MAX &&
				devFeatures.graphicsFamily != UINT32_MAX &&
				devFeatures.transferFamily == UINT32_MAX)
			{
				devFeatures.transferFamily = devFeatures.graphicsFamily;
			}

			if (!devFeatures.hasCompleteFamilies())
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_NO_DEVICE));
			}

			return devFeatures;
		}

		VkFormat KVulkanDevice::FindSupportedFormat(const VkPhysicalDevice physicalDevice,
		                                            const std::vector<VkFormat>& candidates,
		                                            VkImageTiling tiling,
		                                            VkFormatFeatureFlags features)
		{
			for (VkFormat format : candidates)
			{
				VkFormatProperties props = {};
				vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

				if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features)
				{
					return format;
				}
				else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features)
				{
					return format;
				}
			}

			throw std::runtime_error(WhatWentWrong(KE_VULKAN_DEVICE_FAIL));
		}

		bool KVulkanDevice::AreExtensionsSupported(VkPhysicalDevice physicalDevice,
		                                           std::vector<const char *> deviceExtensions)
		{
			uint32_t extensionCount;
			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

			if (extensionCount == 0) return false;

			std::vector<VkExtensionProperties> availableExtensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, availableExtensions.data());

			std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

			for (const auto &extension : availableExtensions)
			{
				requiredExtensions.erase(extension.extensionName);
			}

			return requiredExtensions.empty();
		}

		VkPhysicalDevice KVulkanDevice::FindMostFittingDevice(std::vector<VkPhysicalDevice> devices)
		{
			std::multimap<uint32_t, VkPhysicalDevice> candidates;

			for (const auto &device : devices)
			{
				uint32_t rating = RateDevice(device);
				candidates.insert(std::make_pair(rating, device));
			}

			if (candidates.rbegin()->first > 0)
			{
				return candidates.rbegin()->second;
			}
			else
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_NO_DEVICE));
			}
		}

		uint32_t KVulkanDevice::RateDevice(VkPhysicalDevice device)
		{
			uint32_t rating = 0;

			KDeviceFeatures devFeatures = GetDeviceFeatures(device);

			// Not getting anywhere without shaders
			if (!devFeatures.VkFeatures.geometryShader)
			{
				return 0;
			}

			// Discrete GPUs always win
			if (devFeatures.isDiscrete)
			{
				rating = 0xCA7F00D; // Puurrrr...
			}

			// VRAM in MB
			rating += devFeatures.vram / (1024 * 1024);

			return rating;
		}

		std::vector<uint32_t> KVulkanDevice::GetQueueFamilyIndices()
		{
			std::vector<uint32_t> ret;

			ret.push_back(features.graphicsFamily);

			// Only return unique family indices, transferFamily is always the same as graphics if not unique.
			if (features.graphicsFamily != features.presentFamily) ret.push_back(features.presentFamily);
			if (features.graphicsFamily != features.transferFamily) ret.push_back(features.transferFamily);

			return ret;
		}

		KVulkanDevice::~KVulkanDevice()
		{
			vkDestroyDevice(device, nullptr);
		}
	}
}
