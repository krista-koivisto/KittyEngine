/**
 * Kitty engine Vulkan implementation
 * KVulkanBuffer.cpp
 *
 * Vulkan buffer implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#include "../include/Vulkan/KVulkanBuffer.h"

namespace Kitty
{
	namespace Vulkan
	{
		KVulkanBuffer::KVulkanBuffer(KVulkan *mainContext, VkDeviceSize size, VkBufferUsageFlags usage,
		                             VkMemoryPropertyFlags properties)
		{
			context = mainContext;
			device = context->device->device;
			KError ret = CreateBuffer(size, usage, properties);
			if (ret != KE_OK) throw std::runtime_error(WhatWentWrong(ret));
		}

		KError KVulkanBuffer::CreateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties)
		{
			size = bufferSize;

			VkBufferCreateInfo bufferInfo = {};
			bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
			bufferInfo.size = size;
			bufferInfo.usage = usage;

			std::vector<uint32_t> indices = context->device->GetQueueFamilyIndices();
			bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
			bufferInfo.queueFamilyIndexCount = static_cast<uint32_t>(indices.size());
			bufferInfo.pQueueFamilyIndices = indices.data();

			if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
			{
				return KE_VULKAN_BUFFER_CREATE_FAIL;
			}

			VkMemoryRequirements memRequirements = {};
			vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

			VkMemoryAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
			allocInfo.allocationSize = memRequirements.size;
			allocInfo.memoryTypeIndex = FindMemoryType(memRequirements.memoryTypeBits, properties);

			if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS)
			{
				return KE_VULKAN_BUFFER_CREATE_FAIL;
			}

			vkBindBufferMemory(device, buffer, bufferMemory, 0);

			return KE_OK;
		}

		void KVulkanBuffer::Copy(KVulkanBuffer *srcBuffer, VkBufferCopy copyRegion)
		{
			if (!copyRegion.size) copyRegion.size = srcBuffer->size;
			if (copyRegion.size > size)
			{
				throw std::runtime_error(WhatWentWrong(KE_VULKAN_BUFFER_TOO_SMALL));
			}

			VkCommandBuffer commandBuffer = context->transferCmdPool->InitiateCommand();
			vkCmdCopyBuffer(commandBuffer, srcBuffer->buffer, buffer, 1, &copyRegion);
			context->transferCmdPool->FinalizeCommand(commandBuffer, context->device->transferQueue);
		}

		uint32_t KVulkanBuffer::FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties)
		{
			VkPhysicalDeviceMemoryProperties memProperties = {};
			vkGetPhysicalDeviceMemoryProperties(context->device->pDevice, &memProperties);

			for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
			{
				if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
				{
					return i;
				}
			}

			throw std::runtime_error(WhatWentWrong(KE_VULKAN_MEMORY_FAIL));
		}

		KVulkanBuffer::~KVulkanBuffer()
		{
			vkDestroyBuffer(device, buffer, nullptr);
			vkFreeMemory(device, bufferMemory, nullptr);
		}
	}
}
