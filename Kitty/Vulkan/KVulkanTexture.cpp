/**
 * Kitty engine Vulkan implementation
 * KVulkanTexture.cpp
 *
 * Vulkan texture implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanTexture.h"

namespace Kitty
{
	namespace Vulkan
	{
		KVulkanTexture::KVulkanTexture(KVulkan *mainContext, KVulkanSettings *vulkanSettings)
		{
			context = mainContext;
			settings = vulkanSettings;

			// Set a single texel texture by default
			unsigned char texel[4] = {255, 255, 255, 255};
			KError ret = SetImage2D_8R8G8B8A(texel, 1, 1);
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));
		}

		KError KVulkanTexture::SetImage2D_8R8G8B8A(unsigned char *buffer, uint32_t texWidth, uint32_t texHeight)
		{
			VkDevice device = context->device->device;
			VkDeviceSize imageSize = static_cast<uint64_t>(texWidth) * static_cast<uint64_t>(texHeight) * 4;

			if (!buffer)
			{
				throw std::runtime_error(WhatWentWrong(KE_TEXTURE_LOAD_FAIL));
			}

			auto usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			auto props = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
			auto staging = new Vulkan::KVulkanBuffer(context, imageSize, usage, props);

			void* data;
			vkMapMemory(device, staging->bufferMemory, 0, imageSize, 0, &data);
			memcpy(data, buffer, static_cast<size_t>(imageSize));
			vkUnmapMemory(device, staging->bufferMemory);

			if (image != nullptr)
			{
				delete(image);
				image = nullptr;
			}

			image = new KVulkanImage(context, texWidth, texHeight, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL,
			                         VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			                         VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

			image->TransitionImageLayout(VK_FORMAT_R8G8B8A8_UNORM,
			                      VK_IMAGE_LAYOUT_UNDEFINED,
			                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

			CopyFromBuffer2D(staging->buffer, image->image,
			                 static_cast<uint32_t>(texWidth),
			                 static_cast<uint32_t>(texHeight));

			// Prepare image for shader access
			image->TransitionImageLayout(VK_FORMAT_R8G8B8A8_UNORM,
			                      VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			                      VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

			delete(staging);

			CreateImageView();

			InitializeTextureSampler();

			return KE_OK;
		}

		void KVulkanTexture::CopyFromBuffer2D(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
		{
			VkCommandBuffer commandBuffer = context->cmdPool->InitiateCommand();

			VkBufferImageCopy region = {};
			region.bufferOffset = 0;
			region.bufferRowLength = 0;
			region.bufferImageHeight = 0;

			region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			region.imageSubresource.mipLevel = 0;
			region.imageSubresource.baseArrayLayer = 0;
			region.imageSubresource.layerCount = 1;

			region.imageOffset = {0, 0, 0};
			region.imageExtent = {width, height, 1};

			vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

			context->cmdPool->FinalizeCommand(commandBuffer, context->device->graphicsQueue);
		}

		void KVulkanTexture::CreateImageView()
		{
			if (textureImageView != nullptr)
			{
				vkDestroyImageView(context->device->device, textureImageView, nullptr);
				textureImageView = nullptr;
			}

			VkImageViewCreateInfo viewInfo = {};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = image->image;
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
			viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			viewInfo.subresourceRange.baseMipLevel = 0;
			viewInfo.subresourceRange.levelCount = 1;
			viewInfo.subresourceRange.baseArrayLayer = 0;
			viewInfo.subresourceRange.layerCount = 1;

			if (vkCreateImageView(context->device->device, &viewInfo, nullptr, &textureImageView) != VK_SUCCESS)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_IMAGEVIEW_FAIL));
			}
		}

		void KVulkanTexture::InitializeTextureSampler()
		{
			if (textureSampler != nullptr)
			{
				vkDestroySampler(context->device->device, textureSampler, nullptr);
				textureSampler = nullptr;
			}

			auto samplerInfo = settings->textureSamplerInfo;

			// Make sure Anisotropic filtering is possible.
			if (!context->device->features.VkFeatures.samplerAnisotropy)
			{
				settings->textureSamplerInfo.anisotropyEnable = VK_FALSE;
				samplerInfo.maxAnisotropy = 1;
			}

			if (vkCreateSampler(context->device->device, &samplerInfo, nullptr, &textureSampler) != VK_SUCCESS)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_TEXTURE_SAMPLER_FAIL));
			}
		}

		KVulkanTexture::~KVulkanTexture()
		{
			VkDevice device = context->device->device;
			vkDestroyImageView(device, textureImageView, nullptr);
			vkDestroySampler(device, textureSampler, nullptr);
			delete(image);
		}
	}
}
