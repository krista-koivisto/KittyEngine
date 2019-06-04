/**
 * Kitty engine Vulkan implementation
 * KVulkanBuffer.h
 *
 * Vulkan buffer implementation for the Kitty graphics engine. This
 * functions as an abstraction layer between Vulkan and the Kitty
 * engine, direct access from the end user interface should never
 * happen.
 *
 * \author Krista Koivisto
 * \copyright Read included LICENSE file.
 */

#ifndef KENGINE_KVULKANVERTEXBUFFER_H
#define KENGINE_KVULKANVERTEXBUFFER_H

#include <vulkan/vulkan.h>
#include "KVulkan.h"

namespace Kitty
{
	namespace Vulkan
	{
		class KVulkan;

		class KVulkanBuffer
		{
		private:
			KVulkan *context = nullptr;
			VkDevice device = {};

			// TODO: Write a memory allocator class and use one huge Vulkan buffer instead. Performance++. :)

			/**
			 * \brief Allocate a Vulkan memory buffer.
			 *
			 * Allocate a Vulkan memory buffer with the given properties and usage specifications.
			 *
			 * \param bufferSize Buffer size.
			 * \param usage What will this memory be used for?
			 * \param properties What properties should this memory have?
			 * \return KE_OK on success, error code on fail.
			 */
			KError CreateBuffer(VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);

		public:
			/**
			 * \brief Create a Vulkan memory buffer.
			 *
			 * Create and allocate a Vulkan memory buffer with the given properties and usage specifications.
			 *
			 * \param mainContext Parent Vulkan context.
			 * \param bufferSize Buffer size.
			 * \param usage What will this memory be used for?
			 * \param properties What properties should this memory have?
			 */
			explicit KVulkanBuffer(KVulkan *mainContext,
			                       VkDeviceSize size,
			                       VkBufferUsageFlags usage,
			                       VkMemoryPropertyFlags properties);

			~KVulkanBuffer();

			VkDeviceMemory bufferMemory = VK_NULL_HANDLE;
			VkBuffer buffer = {};
			VkDeviceSize size = 0;
			void* mappedMemory = nullptr;

			/**
			 * \brief Map memory to a pointer for access.
			 */
			void Map()
			{
				vkMapMemory(device, bufferMemory, 0, size, 0, &mappedMemory);
			}

			/**
			 * \brief Unmap memory.
			 */
			void Unmap()
			{
				vkUnmapMemory(device, bufferMemory);
				mappedMemory = nullptr;
			}

			/**
			 * \brief Copy data to the buffer.
			 *
			 * \param data std::vector containing data to pass to the buffer.
			 */
			template <typename T>
			void Fill(const std::vector<T> data)
			{
				if (size < data.size()) throw std::runtime_error(WhatWentWrong(KE_VULKAN_BUFFER_TOO_SMALL));

				void* dest;
				vkMapMemory(device, bufferMemory, 0, size, 0, &dest);
				memcpy(dest, data.data(), size);
				vkUnmapMemory(device, bufferMemory);
			}

			/**
			 * \brief Copy the contents of another buffer into this.
			 *
			 * \param srcBuffer Source buffer.
			 * \param copyRegion [optional] Region of data to copy.
			 */
			void Copy(KVulkanBuffer *srcBuffer, VkBufferCopy copyRegion = {});

			/**
			 * \brief Tries to find a suitable memory type for the buffer to be created with.
			 *
			 * \param typeFilter Type of memory we want.
			 * \param properties Memory properties the memory we want needs to have.
			 * \return Index to a suitable memory type.
			 */
			uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
		};
	}
}


#endif //KENGINE_KVULKANVERTEXBUFFER_H
