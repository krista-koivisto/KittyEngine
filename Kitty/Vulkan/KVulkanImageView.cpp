/**
 * Kitty engine Vulkan implementation
 * KVulkanImageView.cpp
 *
 * Vulkan image view implementation for the Kitty graphics engine.
 * This functions as an abstraction layer between Vulkan and the
 * Kitty engine, direct access from the end user interface should
 * never happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanImageView.h"

namespace Kitty
{
	namespace Vulkan
	{
		KError KVulkanImageView::Initialize(VkImageViewCreateInfo createInfo, VkImage image)
		{
			createInfo.image = image;

			if (vkCreateImageView(context->device->device, &createInfo, nullptr, &imageView) != VK_SUCCESS)
			{
				return KE_VULKAN_IMAGEVIEW_FAIL;
			}

			return KE_OK;
		}

		KVulkanImageView::~KVulkanImageView()
		{
			vkDestroyImageView(context->device->device, imageView, nullptr);
		}
	}
}
