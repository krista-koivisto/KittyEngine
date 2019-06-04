/**
 * Kitty engine Vulkan implementation
 * KVulkanImage.cpp
 *
 * Vulkan image implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanImage.h"

namespace Kitty
{
	namespace Vulkan
	{
		KVulkanImage::KVulkanImage(KVulkan *mainContext, uint32_t width, uint32_t height, VkFormat format,
		                           VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
		{
			context = mainContext;
			KError ret = Initialize(width, height, format, tiling, usage, properties);

			if (!ret)
			{
				throw std::runtime_error(WhatWentWrong(ret));
			}
		}

		KError KVulkanImage::Initialize(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
		                                VkImageUsageFlags usage, VkMemoryPropertyFlags properties)
		{
			VkDevice device = context->device->device;
			VkImageCreateInfo imageInfo = {};
			imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
			imageInfo.imageType = VK_IMAGE_TYPE_2D;
			imageInfo.format = format;
			imageInfo.tiling = tiling;
			imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			imageInfo.extent.width = width;
			imageInfo.extent.height = height;
			imageInfo.extent.depth = 1;
			imageInfo.mipLevels = 1;
			imageInfo.arrayLayers = 1;
			imageInfo.usage = usage;
			imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
			imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

			if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS)
			{
				return KE_TEXTURE_LOAD_FAIL;
			}

			VkMemoryRequirements memRequirements = {};
			vkGetImageMemoryRequirements(device, image, &memRequirements);

			auto temp = new KVulkanBuffer(context, 1, usage, properties);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = temp->FindMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			delete(temp);

			if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS)
			{
				return KE_TEXTURE_ALLOC_FAIL;
			}

			vkBindImageMemory(device, image, imageMemory, 0);

			return KE_OK;
		}

		void KVulkanImage::TransitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
		{
			VkCommandBuffer commandBuffer = context->cmdPool->InitiateCommand();

			VkImageMemoryBarrier barrier = {};
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = oldLayout;
			barrier.newLayout = newLayout;

			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

			barrier.image = image;
			barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;

			// Is this a depth buffer?
			if (newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

				// Check if depth buffering can have a stencil bit set
				if (format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT)
				{
					barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				}
			}
			else
			{
				barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			}

			VkPipelineStageFlags sourceStage = {};
			VkPipelineStageFlags destinationStage = {};

			if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED)
			{
				switch (newLayout)
				{
					case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
						barrier.srcAccessMask = 0;
						barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
						sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
						destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
						break;

					case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
						barrier.srcAccessMask = 0;
						barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
						sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
						destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
						break;

					default:
						throw std::runtime_error(WhatWentWrong(KE_UNSUPPORTED_LAYOUT));
				}
			}
			else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
			{
				barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
				barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
				sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
				destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
			}

			vkCmdPipelineBarrier(commandBuffer, sourceStage, destinationStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
			context->cmdPool->FinalizeCommand(commandBuffer, context->device->graphicsQueue);
		}

		KVulkanImage::~KVulkanImage()
		{
			VkDevice device = context->device->device;
			vkDestroyImage(device, image, nullptr);
			vkFreeMemory(device, imageMemory, nullptr);
		}
	}
}
