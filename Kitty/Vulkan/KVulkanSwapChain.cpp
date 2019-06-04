/**
 * Kitty engine Vulkan implementation
 * KVulkanSwapChain.cpp
 *
 * Vulkan swap chain implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the
 * Kitty engine, direct access from the end user interface should
 * never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanSwapChain.h"

namespace Kitty
{
	namespace Vulkan
	{
		KError KVulkanSwapChain::Initialize(VkSurfaceFormatKHR *desiredFormat,
		                                    VkPresentModeKHR *desiredPresentMode,
		                                    VkSwapchainCreateInfoKHR *swapChainCreateInfo)
		{
			supportDetails = QuerySwapChainSupport(context->device->pDevice);

			if (!IsSwapChainAdequate(supportDetails))
			{
				return KE_VULKAN_SWAPCHAIN_FAIL;
			}

			VkSurfaceFormatKHR surfaceFormat = ChooseSwapSurfaceFormat(supportDetails.formats, desiredFormat);
			VkPresentModeKHR presentMode = ChooseSwapPresentMode(supportDetails.presentModes, desiredPresentMode);
			VkExtent2D extent = ChooseSwapExtent(supportDetails.capabilities);

			auto createInfo = defaults.ObtainValues(swapChainCreateInfo, &defaults.swapChainCreateInfo);
			uint32_t imageCount = supportDetails.capabilities.minImageCount;

			if (!createInfo.minImageCount)
			{
				// Triple buffering should have an extra image buffer
				if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR) imageCount++;
			}
			else
			{
				// If a user supplied a preferred minimum image count, ensure it doesn't go below the device minimum.
				if (supportDetails.capabilities.minImageCount <= createInfo.minImageCount)
				{
					imageCount = createInfo.minImageCount;
				}
			}

			// Check that we're not going over the max limit (0 means infinite)
			if (supportDetails.capabilities.maxImageCount > 0 && imageCount > supportDetails.capabilities.maxImageCount)
			{
				imageCount = supportDetails.capabilities.maxImageCount;
			}

			createInfo.minImageCount = imageCount;
			if (!createInfo.surface) createInfo.surface = context->surface;
			if (!createInfo.imageFormat) createInfo.imageFormat = surfaceFormat.format;
			if (!createInfo.imageColorSpace) createInfo.imageColorSpace = surfaceFormat.colorSpace;
			if (!createInfo.imageExtent.height && !createInfo.imageExtent.width) createInfo.imageExtent = extent;
			if (!createInfo.preTransform) createInfo.preTransform = supportDetails.capabilities.currentTransform;
			if (!createInfo.presentMode) createInfo.presentMode = presentMode;
			createInfo.oldSwapchain = VK_NULL_HANDLE;

			std::vector<uint32_t> indices = context->device->GetQueueFamilyIndices();
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = static_cast<uint32_t>(indices.size());
			createInfo.pQueueFamilyIndices = indices.data();

			if (vkCreateSwapchainKHR(context->device->device, &createInfo, nullptr, &swapChain) != VK_SUCCESS)
			{
				return KE_VULKAN_SWAPCHAIN_FAIL;
			}

			vkGetSwapchainImagesKHR(context->device->device, swapChain, &imageCount, nullptr);
			swapChainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(context->device->device, swapChain, &imageCount, swapChainImages.data());

			swapChainImageFormat = surfaceFormat.format;
			swapChainExtent = extent;

			swapChainImageViews.resize(swapChainImages.size());

			for (size_t i = 0; i < swapChainImageViews.size(); i++)
			{
				swapChainImageViews[i] = new KVulkanImageView(context);
				swapChainImageViews[i]->Initialize(defaults.imageViewCreateInfo, swapChainImages[i]);
			}

			return KE_OK;
		}

		KVulkanSwapChain::SwapChainSupportDetails
		KVulkanSwapChain::QuerySwapChainSupport(VkPhysicalDevice physicalDevice)
		{
			SwapChainSupportDetails details = {};

			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, context->surface, &details.capabilities);

			uint32_t formatCount = 0;
			vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, context->surface, &formatCount, nullptr);

			if (formatCount != 0)
			{
				details.formats.resize(formatCount);
				vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, context->surface, &formatCount,
				                                     details.formats.data());
			}

			uint32_t presentModeCount = 0;
			vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, context->surface, &presentModeCount, nullptr);

			if (presentModeCount != 0)
			{
				details.presentModes.resize(presentModeCount);
				vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice,
				                                          context->surface,
				                                          &presentModeCount,
				                                          details.presentModes.data());
			}

			return details;
		}

		bool KVulkanSwapChain::IsSwapChainAdequate(KVulkanSwapChain::SwapChainSupportDetails swapChainDetails)
		{
			return !swapChainDetails.formats.empty() && !swapChainDetails.presentModes.empty();
		}

		VkSurfaceFormatKHR
		KVulkanSwapChain::ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &availableFormats,
		                                          VkSurfaceFormatKHR *desiredFormat)
		{
			auto requestedFormat = defaults.ObtainValues(desiredFormat, &defaults.desiredSurfaceFormat);

			// Surface has no preferred format? That means we can use our desired format! Yay!!
			if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED)
			{
				return requestedFormat;
			}

			// Maybe it'll let us use that pretty sRGB color space anyway? If that pesky user didn't override it, that is...
			for (const auto &availableFormat : availableFormats)
			{
				if (availableFormat.format == requestedFormat.format &&
				    availableFormat.colorSpace == requestedFormat.colorSpace)
				{
					return requestedFormat;
				}
			}

			// No? Fine. Be that way. We'll just pick the first available surface format then...
			return availableFormats[0];
		}

		VkPresentModeKHR
		KVulkanSwapChain::ChooseSwapPresentMode(const std::vector<VkPresentModeKHR> &availablePresentModes,
		                                        VkPresentModeKHR *desiredPresentMode)
		{
			auto requestedFormat = defaults.ObtainValues(desiredPresentMode, &defaults.desiredPresentMode);

			// Default to V-Sync if no other mode can be set
			VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

			for (const auto &availablePresentMode : availablePresentModes)
			{
				if (availablePresentMode == requestedFormat)
				{
					return availablePresentMode;
				}
				else if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					// Choose triple buffering over V-Sync if the requested format can't be set
					bestMode = availablePresentMode;
				}
			}

			return bestMode;
		}

		VkExtent2D KVulkanSwapChain::ChooseSwapExtent(const VkSurfaceCapabilitiesKHR &capabilities)
		{
			// Does the window manager allow us to render the swap images at a resolution differing from the window size?
			if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			{
				// No? Alright, then we already have the right value.
				return capabilities.currentExtent;
			}
			else
			{
				// Yes? OK, then we'll have to find the most fitting value for our current window size ourselves.
				Vector2<int> windowSize = context->window->GetDimensions();
				VkExtent2D actualExtent = {static_cast<uint32_t>(windowSize.x), static_cast<uint32_t>(windowSize.y)};

				actualExtent.width = std::max(capabilities.minImageExtent.width,
				                              std::min(capabilities.maxImageExtent.width, actualExtent.width));
				actualExtent.height = std::max(capabilities.minImageExtent.height,
				                               std::min(capabilities.maxImageExtent.height, actualExtent.height));

				return actualExtent;
			}
		}

		KVulkanSwapChain::~KVulkanSwapChain()
		{
			for (auto &swapChainImageView : swapChainImageViews)
			{
				delete (swapChainImageView);
			}

			vkDestroySwapchainKHR(context->device->device, swapChain, nullptr);
		}
	}
}
